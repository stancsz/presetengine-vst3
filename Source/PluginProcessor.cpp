#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
YamlPresetPluginAudioProcessor::YamlPresetPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

YamlPresetPluginAudioProcessor::~YamlPresetPluginAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout YamlPresetPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    // We keep these dummy parameters or remove them.
    // The user wants the structure to be defined by code.
    // We can expose a "Code" parameter as a big string, but APVTS params are usually float/bool/choice.
    // The code/state is better handled in getStateInformation.

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String YamlPresetPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool YamlPresetPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool YamlPresetPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_WantsMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool YamlPresetPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double YamlPresetPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int YamlPresetPluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int YamlPresetPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void YamlPresetPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String YamlPresetPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void YamlPresetPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void YamlPresetPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    effectChain.prepare(spec);
}

void YamlPresetPluginAudioProcessor::releaseResources()
{
    effectChain.reset();
}

bool YamlPresetPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void YamlPresetPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    effectChain.process(buffer);
}

//==============================================================================
bool YamlPresetPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* YamlPresetPluginAudioProcessor::createEditor()
{
    return new YamlPresetPluginAudioProcessorEditor (*this);
}

//==============================================================================
void YamlPresetPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save the current config.
    // For now, we assume the config string is stored in a member or we reconstruct it.
    // Actually, we should store the 'source code' that generated the chain.
    juce::MemoryOutputStream stream(destData, true);
    stream.writeString(currentConfigCode);
}

void YamlPresetPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, sizeInBytes, false);
    juce::String config = stream.readString();

    loadConfig(config);
}

juce::Result YamlPresetPluginAudioProcessor::loadConfig(const juce::String& config)
{
    currentConfigCode = config;

    // Detect format
    if (config.trim().startsWith("<"))
        return effectChain.loadFromXml(config);
    else
        return effectChain.loadFromYaml(config);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new YamlPresetPluginAudioProcessor();
}
