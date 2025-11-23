#pragma once
#include "../AudioEffect.h"

class CompressorEffect : public AudioEffect
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) override
    {
        compressor.prepare(spec);
    }

    void process(const juce::dsp::ProcessContextReplacing<float>& context) override
    {
        compressor.process(context);
    }

    void reset() override
    {
        compressor.reset();
    }

    void configure(const YAML::Node& config) override
    {
        if (config["threshold"]) compressor.setThreshold(config["threshold"].as<float>());
        if (config["ratio"]) compressor.setRatio(config["ratio"].as<float>());
        if (config["attack"]) compressor.setAttack(config["attack"].as<float>());
        if (config["release"]) compressor.setRelease(config["release"].as<float>());
    }

    void configure(const juce::ValueTree& config) override
    {
        if (config.hasProperty("threshold")) compressor.setThreshold(config.getProperty("threshold"));
        if (config.hasProperty("ratio")) compressor.setRatio(config.getProperty("ratio"));
        if (config.hasProperty("attack")) compressor.setAttack(config.getProperty("attack"));
        if (config.hasProperty("release")) compressor.setRelease(config.getProperty("release"));
    }

private:
    juce::dsp::Compressor<float> compressor;
};
