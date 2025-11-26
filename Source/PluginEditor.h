#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ModernLookAndFeel.h"
#include <cmath>

//==============================================================================
/**
 * A dummy spectrum visualizer component that draws a pleasing gradient curve.
 * In a real plugin, this would read FFT data from the processor.
 */
class SpectrumComponent : public juce::Component, public juce::Timer
{
public:
    SpectrumComponent(PresetEngineAudioProcessor& p) : processor(p)
    {
        startTimerHz(30);
    }

    ~SpectrumComponent() override
    {
        stopTimer();
    }

    void timerCallback() override
    {
        if (processor.inputFFT.nextFFTBlockReady)
        {
            processor.inputFFT.forwardFFT.performFrequencyOnlyForwardTransform(processor.inputFFT.fftData.data());
            processor.inputFFT.nextFFTBlockReady = false;
            repaint();
        }
        
        if (processor.outputFFT.nextFFTBlockReady)
        {
            processor.outputFFT.forwardFFT.performFrequencyOnlyForwardTransform(processor.outputFFT.fftData.data());
            processor.outputFFT.nextFFTBlockReady = false;
            repaint();
        }
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff121212));
        
        g.setOpacity(1.0f);
        
        auto drawSpectrum = [&](const std::vector<float>& data, juce::Colour color)
        {
            juce::Path path;
            path.startNewSubPath(0, (float)getHeight());
            
            const float mindB = -100.0f;
            const float maxdB = 0.0f;
            const int fftSize = PresetEngineAudioProcessor::fftSize;
            const int halfSize = fftSize / 2;
            
            for (int i = 0; i < halfSize; ++i)
            {
                float skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)halfSize) * 0.2f);
                
                // Map skewed proportion to FFT bin index
                float fftDataIndex = juce::jlimit(0.0f, (float)halfSize, skewedProportionX * (float)halfSize);
                
                // Calculate magnitude in dB
                float magnitude = data[(int)fftDataIndex];
                float levelDb = juce::Decibels::gainToDecibels(magnitude) - juce::Decibels::gainToDecibels((float)fftSize);
                
                // Map dB to height (0.0 to 1.0)
                float level = juce::jmap(juce::jlimit(mindB, maxdB, levelDb), mindB, maxdB, 0.0f, 1.0f);
                
                path.lineTo(skewedProportionX * (float)getWidth(), (float)getHeight() - level * (float)getHeight());
            }
            
            g.setColour(color);
            g.strokePath(path, juce::PathStrokeType(1.0f));
            
            // Fill
            path.lineTo((float)getWidth(), (float)getHeight());
            path.closeSubPath();
            g.setColour(color.withAlpha(0.2f));
            g.fillPath(path);
        };

        drawSpectrum(processor.inputFFT.fftData, juce::Colours::grey);
        drawSpectrum(processor.outputFFT.fftData, juce::Colour(0xff00bcd4));
    }

private:
    PresetEngineAudioProcessor& processor;
};

//==============================================================================
/**
*/
class PresetEngineAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PresetEngineAudioProcessorEditor (PresetEngineAudioProcessor&);
    ~PresetEngineAudioProcessorEditor() override;

    //==============================================================================
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void rebuildUi();
    juce::String parsePythonToYaml(const juce::String& pythonCode);

    PresetEngineAudioProcessor& audioProcessor;

    ModernLookAndFeel lookAndFeel;

    juce::Label     titleLabel;
    
    // Left Column Components
    SpectrumComponent spectrumComponent;

    juce::ComboBox  languageBox;
    juce::ComboBox  presetBox;
    juce::TextButton exampleButton; // Keep for manual trigger if needed, or remove? Let's keep as "Reset" or similar, or just remove.
    // Actually, let's keep exampleButton but rename/repurpose or just rely on combo box change.
    // The user code uses exampleButton, I'll keep it for now to minimize diff, but maybe hide it or make it "Load".
    
    juce::TextEditor codeEditor;
    juce::TextButton applyButton;
    juce::Label     statusLabel;

    juce::String getPresetSource(int presetId, int languageId);

    // Right Column Components
    juce::Viewport  viewport;
    std::unique_ptr<juce::Component> container;
    juce::OwnedArray<juce::Component> effectComponents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetEngineAudioProcessorEditor)
};
