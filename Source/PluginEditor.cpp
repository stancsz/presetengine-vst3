#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
YamlPresetPluginAudioProcessorEditor::YamlPresetPluginAudioProcessorEditor (YamlPresetPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Apply custom look and feel
    setLookAndFeel(&lookAndFeel);

    setSize (600, 500);

    // Header
    titleLabel.setText("Yaml Preset Plugin", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    // Code Editor
    codeEditor.setMultiLine(true);
    codeEditor.setReturnKeyStartsNewLine(true);
    codeEditor.setTabKeyUsedAsCharacter(true);
    codeEditor.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 14.0f, juce::Font::plain));
    codeEditor.setText(audioProcessor.getCurrentConfig());

    // Simple line number simulation (gutter)
    codeEditor.setLeftIndent(10);
    addAndMakeVisible(codeEditor);

    // Status
    statusLabel.setText("Ready.", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(statusLabel);

    // Apply Button
    applyButton.setButtonText("Apply Configuration");
    applyButton.onClick = [this] {
        auto result = audioProcessor.loadConfig(codeEditor.getText());
        if (result.wasOk())
        {
            statusLabel.setText("Configuration Loaded Successfully.", juce::dontSendNotification);
            statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
        }
        else
        {
            statusLabel.setText(result.getErrorMessage(), juce::dontSendNotification);
            statusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
        }
    };
    addAndMakeVisible(applyButton);
}

YamlPresetPluginAudioProcessorEditor::~YamlPresetPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void YamlPresetPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void YamlPresetPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    // Header
    auto headerArea = area.removeFromTop(40);
    titleLabel.setBounds(headerArea);

    // Footer (Button and Status)
    auto footerArea = area.removeFromBottom(40);
    statusLabel.setBounds(footerArea.removeFromLeft(footerArea.getWidth() / 2));
    applyButton.setBounds(footerArea.removeFromRight(150));

    // Spacing
    area.removeFromTop(10);
    area.removeFromBottom(10);

    // Editor
    codeEditor.setBounds(area);
}
