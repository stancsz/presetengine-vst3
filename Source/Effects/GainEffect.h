#pragma once
#include "../AudioEffect.h"

class GainEffect : public AudioEffect
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) override
    {
        gain.prepare(spec);
        gain.setRampDurationSeconds(0.05); // Smooth parameter changes
    }

    void process(const juce::dsp::ProcessContextReplacing<float>& context) override
    {
        gain.process(context);
    }

    void reset() override
    {
        gain.reset();
    }

    void configure(const YAML::Node& config) override
    {
        if (config["gain"])
            gain.setGainLinear(config["gain"].as<float>());

        if (config["gain_db"])
            gain.setGainDecibels(config["gain_db"].as<float>());
    }

    void configure(const juce::ValueTree& config) override
    {
        if (config.hasProperty("gain"))
            gain.setGainLinear(config.getProperty("gain"));

        if (config.hasProperty("gain_db"))
            gain.setGainDecibels(config.getProperty("gain_db"));
    }

private:
    juce::dsp::Gain<float> gain;
};
