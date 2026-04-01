#pragma once

#include <JuceHeader.h>

class ProvisionAudioProcessor  : public juce::AudioProcessor
{
public:
    ProvisionAudioProcessor();
    ~ProvisionAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    // V1.3: XML State Serialization functions
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // O(1) Cached Parameter Pointers
    std::atomic<float>* rootParam = nullptr;
    std::atomic<float>* minorParam = nullptr;
    std::atomic<float>* splitLowParam = nullptr;
    std::atomic<float>* splitHighParam = nullptr;

    // State Management to prevent hanging notes
    bool activeChords[128] = { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProvisionAudioProcessor)
};