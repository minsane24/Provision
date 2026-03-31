#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ProvisionEditor  : public juce::AudioProcessorEditor
{
public:
    ProvisionEditor (ProvisionAudioProcessor&);
    ~ProvisionEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ProvisionAudioProcessor& audioProcessor;

    juce::Label rootKeyLabel {"", "Root Key:"};
    juce::ComboBox rootKeyBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rootKeyAttachment;

    juce::ToggleButton minorToggle {"Minor Scale"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> minorAttachment;

    juce::Label splitLowLabel {"", "Split Low:"};
    juce::Slider splitLowSlider { juce::Slider::LinearHorizontal, juce::Slider::TextBoxLeft };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> splitLowAttachment;

    juce::Label splitHighLabel {"", "Split High:"};
    juce::Slider splitHighSlider { juce::Slider::LinearHorizontal, juce::Slider::TextBoxLeft };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> splitHighAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProvisionEditor)
};