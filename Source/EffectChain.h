#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include "AudioEffect.h"
#include <vector>
#include <memory>
#include <atomic>

class EffectChain
{
public:
    EffectChain();
    ~EffectChain();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Rebuilds the chain from a YAML string
    juce::Result loadFromYaml(const juce::String& yamlString);

    // Rebuilds the chain from a JUCE XML string
    juce::Result loadFromXml(const juce::String& xmlString);

    // Helper to create effect by type name
    static std::unique_ptr<AudioEffect> createEffect(const std::string& type);

private:
    // Simple double buffering approach for thread safety without locking audio thread
    using EffectList = std::vector<std::unique_ptr<AudioEffect>>;

    std::shared_ptr<EffectList> activeEffects;

    juce::dsp::ProcessSpec currentSpec { 44100.0, 512, 2 };

    // Lock only for updating the config (write side), not for reading in process
    juce::CriticalSection updateLock;
};
