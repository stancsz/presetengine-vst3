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
    // Visualization - FFT
    enum
    {
        fftOrder = 11,
        fftSize = 1 << fftOrder
    };

    struct FFTData {
        juce::dsp::FFT forwardFFT { fftOrder };
        juce::dsp::WindowingFunction<float> window { fftSize, juce::dsp::WindowingFunction<float>::hann };
        
        std::vector<float> fifo;
        std::vector<float> fftData;
        int fifoIndex = 0;
        bool nextFFTBlockReady = false;
        
        FFTData() : fifo(fftSize), fftData(2 * fftSize) {}
        
        void pushNextSample(float sample)
        {
            if (fifoIndex == fftSize)
            {
                if (!nextFFTBlockReady)
                {
                    std::fill(fftData.begin(), fftData.end(), 0.0f);
                    std::copy(fifo.begin(), fifo.end(), fftData.begin());
                    nextFFTBlockReady = true;
                }
                fifoIndex = 0;
            }
            
            fifo[fifoIndex++] = sample;
        }
    };
    
    FFTData inputFFT;
    FFTData outputFFT;

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    EffectChain effectChain;
    juce::String currentConfigCode;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetEngineAudioProcessor)
};
