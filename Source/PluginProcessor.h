#pragma once

#include <JuceHeader.h>
#include "EffectChain.h"

class PresetEngineAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PresetEngineAudioProcessor();
    ~PresetEngineAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::Result loadConfig(const juce::String& config);
    juce::String getCurrentConfig() const { return currentConfigCode; }
    juce::ValueTree getCurrentConfigTree() const { return effectChain.getCurrentConfig(); }

    // Visualization
    struct VisualBuffer {
        std::vector<float> data;
        std::atomic<int> writePos { 0 };
        
        void setSize(int size) {
            data.resize(size, 0.0f);
            writePos = 0;
        }
        
        void push(const juce::AudioBuffer<float>& buffer) {
            int numSamples = buffer.getNumSamples();
            int size = (int)data.size();
            if (size == 0) return;
            
            // Just take channel 0 for simplicity
            auto* channelData = buffer.getReadPointer(0);
            
            int currentPos = writePos.load(std::memory_order_relaxed);
            
            for (int i = 0; i < numSamples; ++i) {
                data[currentPos] = channelData[i];
                currentPos = (currentPos + 1) % size;
            }
            
            writePos.store(currentPos, std::memory_order_relaxed);
        }
    };
    
    VisualBuffer inputVisuals;
    VisualBuffer outputVisuals;

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    EffectChain effectChain;
    juce::String currentConfigCode;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetEngineAudioProcessor)
};
