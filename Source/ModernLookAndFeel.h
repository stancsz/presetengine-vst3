#pragma once
#include <JuceHeader.h>

class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel()
    {
        // Dark theme colors
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff1e1e1e));
        setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff252526));
        setColour(juce::TextEditor::textColourId, juce::Colour(0xffd4d4d4));
        setColour(juce::TextEditor::highlightColourId, juce::Colour(0xff264f78));
        setColour(juce::CaretComponent::caretColourId, juce::Colours::white);

        setColour(juce::TextButton::buttonColourId, juce::Colour(0xff0e639c));
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff1177bb));
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);

        setColour(juce::Label::textColourId, juce::Colour(0xffcccccc));
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 6.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

        auto baseColour = backgroundColour;
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker(0.2f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.2f);

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(button.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }

    juce::Typeface::Ptr getTypefaceForFont(const juce::Font& f) override
    {
        // Try to return a monospaced font if requested, otherwise default
        if (f.getTypefaceName() == juce::Font::getDefaultMonospacedFontName())
            return juce::LookAndFeel_V4::getTypefaceForFont(f);

        return juce::LookAndFeel_V4::getTypefaceForFont(f);
    }
};
