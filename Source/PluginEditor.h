#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ModernLookAndFeel.h"

//==============================================================================
/**
*/
class PresetEngineAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PresetEngineAudioProcessorEditor (PresetEngineAudioProcessor&);
    ~PresetEngineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void updateViewVisibility();
    void rebuildUi();

    PresetEngineAudioProcessor& audioProcessor;
    ModernLookAndFeel lookAndFeel;

    juce::Label     titleLabel;

    // View Toggle
    juce::TextButton toggleViewButton;
    bool isVisualMode = false;

    // Visual View Components
    juce::Viewport  viewport;
    std::unique_ptr<juce::Component> container;
    juce::OwnedArray<juce::Component> effectComponents;

    // Code View Components
    juce::ComboBox  languageBox;
    juce::TextButton exampleButton;
    juce::TextEditor codeEditor;
    juce::TextButton applyButton;

    // Status
    juce::Label     statusLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetEngineAudioProcessorEditor)
};
