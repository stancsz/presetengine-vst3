#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ModernLookAndFeel.h"

//==============================================================================
/**
*/
class YamlPresetPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    YamlPresetPluginAudioProcessorEditor (YamlPresetPluginAudioProcessor&);
    ~YamlPresetPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    YamlPresetPluginAudioProcessor& audioProcessor;

    ModernLookAndFeel lookAndFeel;

    juce::Label     titleLabel;
    juce::TextEditor codeEditor;
    juce::TextButton applyButton;
    juce::Label     statusLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YamlPresetPluginAudioProcessorEditor)
};
