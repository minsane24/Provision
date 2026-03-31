#include "PluginProcessor.h"
#include "PluginEditor.h"

ProvisionEditor::ProvisionEditor (ProvisionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 240);

    // Root Key
    addAndMakeVisible (rootKeyLabel);
    rootKeyBox.addItemList({"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, 1);
    addAndMakeVisible (rootKeyBox);
    rootKeyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "ROOT", rootKeyBox);

    // Minor Toggle
    addAndMakeVisible (minorToggle);
    minorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "MINOR", minorToggle);

    // Split Low
    addAndMakeVisible (splitLowLabel);
    addAndMakeVisible (splitLowSlider);
    splitLowAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "SPLIT_LOW", splitLowSlider);

    // Split High
    addAndMakeVisible (splitHighLabel);
    addAndMakeVisible (splitHighSlider);
    splitHighAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "SPLIT_HIGH", splitHighSlider);
}

ProvisionEditor::~ProvisionEditor() {}

void ProvisionEditor::paint (juce::Graphics& g)
{
    // Dark background for a sleek, modern UI
    g.fillAll (juce::Colour::fromRGB(30, 32, 34));

    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (20.0f, juce::Font::bold)); 
    
    auto titleArea = getLocalBounds().removeFromTop(40);
    g.drawFittedText ("PROVISION V1.1", titleArea, juce::Justification::centred, 1);
}

void ProvisionEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop (40); // Offset for title
    area.reduce (20, 10);    // Margin

    int rowHeight = 30;
    int spacing = 10;

    // Row 1: Root Key & Toggle
    auto row1 = area.removeFromTop (rowHeight);
    rootKeyLabel.setBounds (row1.removeFromLeft (70));
    rootKeyBox.setBounds (row1.removeFromLeft (80));
    row1.removeFromLeft (20); // Spacer
    minorToggle.setBounds (row1);

    area.removeFromTop (spacing);

    // Row 2: Split Low
    auto row2 = area.removeFromTop (rowHeight);
    splitLowLabel.setBounds (row2.removeFromLeft (70));
    splitLowSlider.setBounds (row2);

    area.removeFromTop (spacing);

    // Row 3: Split High
    auto row3 = area.removeFromTop (rowHeight);
    splitHighLabel.setBounds (row3.removeFromLeft (70));
    splitHighSlider.setBounds (row3);
}