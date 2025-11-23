#include "EffectChain.h"
#include "Effects/GainEffect.h"
#include "Effects/FilterEffect.h"
#include "Effects/CompressorEffect.h"
#include "Effects/LimiterEffect.h"
#include <yaml-cpp/yaml.h>

EffectChain::EffectChain()
{
    activeEffects = std::make_shared<EffectList>();
}

EffectChain::~EffectChain()
{
}

void EffectChain::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::ScopedLock sl(updateLock);
    currentSpec = spec;

    // We can iterate the active list safely here because prepare is usually called on the main thread
    // or before processing starts. However, strictly speaking, prepare might be called by the host
    // when processing is suspended.

    // For safety, we access the current list
    auto currentList = std::atomic_load(&activeEffects);
    if (currentList)
    {
        for (auto& effect : *currentList)
        {
            effect->prepare(spec);
        }
    }
}

void EffectChain::process(juce::AudioBuffer<float>& buffer)
{
    // Thread-safe access to the shared pointer
    auto currentList = std::atomic_load(&activeEffects);

    if (currentList)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        for (auto& effect : *currentList)
        {
            effect->process(context);
        }
    }
}

void EffectChain::reset()
{
    juce::ScopedLock sl(updateLock);
    auto currentList = std::atomic_load(&activeEffects);
    if (currentList)
    {
        for (auto& effect : *currentList)
        {
            effect->reset();
        }
    }
}

std::unique_ptr<AudioEffect> EffectChain::createEffect(const std::string& type)
{
    if (type == "Gain") return std::make_unique<GainEffect>();
    if (type == "Filter" || type == "EQ") return std::make_unique<FilterEffect>();
    if (type == "Compressor") return std::make_unique<CompressorEffect>();
    if (type == "Limiter") return std::make_unique<LimiterEffect>();
    return nullptr;
}

juce::Result EffectChain::loadFromYaml(const juce::String& yamlString)
{
    juce::ScopedLock sl(updateLock);

    auto newEffects = std::make_shared<EffectList>();

    try {
        YAML::Node root = YAML::Load(yamlString.toStdString());

        if (!root.IsSequence())
            return juce::Result::fail("YAML root must be a sequence/list.");

        for (const auto& node : root)
        {
            if (node["type"])
            {
                std::string type = node["type"].as<std::string>();
                auto effect = createEffect(type);
                if (effect)
                {
                    effect->configure(node);
                    effect->prepare(currentSpec);
                    newEffects->push_back(std::move(effect));
                }
                else
                {
                     return juce::Result::fail("Unknown effect type: " + type);
                }
            }
        }

        // Atomically swap
        std::atomic_store(&activeEffects, newEffects);
        return juce::Result::ok();
    }
    catch (const YAML::Exception& e)
    {
        return juce::Result::fail(juce::String("YAML Error: ") + e.what());
    }
    catch (...)
    {
        return juce::Result::fail("Unknown error parsing YAML.");
    }
}

juce::Result EffectChain::loadFromXml(const juce::String& xmlString)
{
    juce::ScopedLock sl(updateLock);

    auto newEffects = std::make_shared<EffectList>();

    auto xml = juce::XmlDocument::parse(xmlString);
    if (xml)
    {
        auto tree = juce::ValueTree::fromXml(*xml);

        if (!tree.isValid())
             return juce::Result::fail("Invalid XML.");

        for (const auto& child : tree)
        {
             if (child.hasProperty("type"))
             {
                 std::string type = child.getProperty("type").toString().toStdString();
                 auto effect = createEffect(type);
                 if (effect)
                 {
                     effect->configure(child);
                     effect->prepare(currentSpec);
                     newEffects->push_back(std::move(effect));
                 }
                 else
                 {
                     return juce::Result::fail("Unknown effect type: " + type);
                 }
             }
        }

        // Atomically swap
        std::atomic_store(&activeEffects, newEffects);
        return juce::Result::ok();
    }

    return juce::Result::fail("Failed to parse XML.");
}
