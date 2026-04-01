#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout ProvisionAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    juce::StringArray rootKeys = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    layout.add (std::make_unique<juce::AudioParameterChoice> ("ROOT", "Root Key", rootKeys, 0));
    layout.add (std::make_unique<juce::AudioParameterBool> ("MINOR", "Minor Scale", false));
    layout.add (std::make_unique<juce::AudioParameterInt> ("SPLIT_LOW", "Split Low", 0, 127, 48)); // Default C3
    layout.add (std::make_unique<juce::AudioParameterInt> ("SPLIT_HIGH", "Split High", 0, 127, 72)); // Default C5

    return layout;
}

// ==============================================================================
ProvisionAudioProcessor::ProvisionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    // Cache pointers for O(1) read access in audio thread
    rootParam      = apvts.getRawParameterValue ("ROOT");
    minorParam     = apvts.getRawParameterValue ("MINOR");
    splitLowParam  = apvts.getRawParameterValue ("SPLIT_LOW");
    splitHighParam = apvts.getRawParameterValue ("SPLIT_HIGH");
}

ProvisionAudioProcessor::~ProvisionAudioProcessor() {}

const juce::String ProvisionAudioProcessor::getName() const { return "Provision"; }
bool ProvisionAudioProcessor::acceptsMidi() const { return true; }
bool ProvisionAudioProcessor::producesMidi() const { return true; }
bool ProvisionAudioProcessor::isMidiEffect() const { return true; }
double ProvisionAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int ProvisionAudioProcessor::getNumPrograms() { return 1; }
int ProvisionAudioProcessor::getCurrentProgram() { return 0; }
void ProvisionAudioProcessor::setCurrentProgram (int index) {}
const juce::String ProvisionAudioProcessor::getProgramName (int index) { return {}; }
void ProvisionAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void ProvisionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    std::fill(std::begin(activeChords), std::end(activeChords), false);
}
void ProvisionAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ProvisionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const { return true; }
#endif

// ==============================================================================
// 2. The Diatonic Math Helper Function (Absolute Pitch)
// ==============================================================================

int getDiatonicPitch (int scaleRootPitch, bool isMinor, int targetDegree)
{
    int majorScale[] = {0, 2, 4, 5, 7, 9, 11};
    int minorScale[] = {0, 2, 3, 5, 7, 8, 10};
    int* scale = isMinor ? minorScale : majorScale;

    int octaves = targetDegree / 7;
    int remainder = targetDegree % 7;
    if (remainder < 0) { remainder += 7; octaves -= 1; }

    int rawNote = scaleRootPitch + (octaves * 12) + scale[remainder];
    return std::clamp(rawNote, 0, 127); // Boundary Validation
}

// ==============================================================================
// 3. The MIDI Logic Block
// ==============================================================================
void ProvisionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    int rootKey  = static_cast<int>(rootParam->load());
    bool isMinor = minorParam->load() > 0.5f;
    int splitL   = static_cast<int>(splitLowParam->load());
    int splitH   = static_cast<int>(splitHighParam->load());

    juce::MidiBuffer processedBuffer;

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        int noteNumber = message.getNoteNumber();
        
        if (message.isNoteOn() || message.isNoteOff())
        {
            // 1. Calculate the Relative Pitch Class against the Root Key
            int pc = noteNumber % 12;
            int relPc = (pc - rootKey + 12) % 12; 
            int degree = -1;
            
            // 2. Map the Relative Pitch to the Diatonic Scale Degree
            if (!isMinor) {
                switch (relPc) {
                    case 0: degree = 0; break; case 2: degree = 1; break;
                    case 4: degree = 2; break; case 5: degree = 3; break;
                    case 7: degree = 4; break; case 9: degree = 5; break;
                    case 11: degree = 6; break;
                }
            } else {
                switch (relPc) {
                    case 0: degree = 0; break; case 2: degree = 1; break;
                    case 3: degree = 2; break; case 5: degree = 3; break;
                    case 7: degree = 4; break; case 8: degree = 5; break;
                    case 10: degree = 6; break;
                }
            }

            bool triggerChord = false;

            // 3. The Gatekeeper
            if (message.isNoteOn())
            {
                // Only trigger if the note belongs to the scale AND is within the Split Zone
                if (degree != -1 && noteNumber >= splitL && noteNumber <= splitH) {
                    activeChords[noteNumber] = true;
                    triggerChord = true;
                }
            }
            else if (message.isNoteOff())
            {
                if (activeChords[noteNumber]) {
                    activeChords[noteNumber] = false;
                    triggerChord = true;
                }
            }

            if (triggerChord)
            {
                // 4. Anchor the chord to the precise octave of the pressed key
                int scaleRootPitch = noteNumber - relPc;

                int n1 = getDiatonicPitch(scaleRootPitch, isMinor, degree);     // Root
                int n2 = getDiatonicPitch(scaleRootPitch, isMinor, degree + 4); // Fifth
                int n3 = getDiatonicPitch(scaleRootPitch, isMinor, degree + 7); // Octave
                int n4 = getDiatonicPitch(scaleRootPitch, isMinor, degree + 9); // Tenth

                juce::uint8 vel = message.getVelocity();

                if (message.isNoteOn()) {
                    processedBuffer.addEvent (juce::MidiMessage::noteOn (message.getChannel(), n1, vel), metadata.samplePosition);
                    processedBuffer.addEvent (juce::MidiMessage::noteOn (message.getChannel(), n2, vel), metadata.samplePosition);
                    processedBuffer.addEvent (juce::MidiMessage::noteOn (message.getChannel(), n3, vel), metadata.samplePosition);
                    processedBuffer.addEvent (juce::MidiMessage::noteOn (message.getChannel(), n4, vel), metadata.samplePosition);
                } else {
                    processedBuffer.addEvent (juce::MidiMessage::noteOff (message.getChannel(), n1, vel), metadata.samplePosition);
                    processedBuffer.addEvent (juce::MidiMessage::noteOff (message.getChannel(), n2, vel), metadata.samplePosition);
                    processedBuffer.addEvent (juce::MidiMessage::noteOff (message.getChannel(), n3, vel), metadata.samplePosition);
                    processedBuffer.addEvent (juce::MidiMessage::noteOff (message.getChannel(), n4, vel), metadata.samplePosition);
                }
                continue; // Skip the pass-through logic below
            }
        }
        
        // Pass-through Gatekeeper (CC, Pitch Bend, Non-diatonic keys, or Out-of-bounds notes)
        processedBuffer.addEvent (message, metadata.samplePosition);
    }

    midiMessages.swapWith (processedBuffer);
}

// ==============================================================================
juce::AudioProcessorEditor* ProvisionAudioProcessor::createEditor()
{
    return new ProvisionEditor (*this);
}

bool ProvisionAudioProcessor::hasEditor() const { return true; }

// We will implement these in V1.1.5 for DAW state saving
void ProvisionAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {}
void ProvisionAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProvisionAudioProcessor();
}