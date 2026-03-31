Provision
Professional MIDI FX for Live Worship Environments
Manufacturer: MannaAudio
Version: 1.1 — The Performance Split Update

🎹 Overview

Provision is a high-efficiency MIDI FX plugin built in C++ using the JUCE framework. It is designed to transform simple one-finger input into massive, perfectly-voiced 1-5-8-10 "worship pads" while maintaining the flexibility needed for live performance.

Originally conceived as a simple macro tool (formerly "WorshipChords"), Version 1.1 introduces the Performance Split Engine, allowing musicians to drive a powerful chord engine with their left hand while performing lead melodies or counter-harmonies with their right—all on a single MIDI controller.

✨ Key Features & Architecture (V1.1)

1. The Performance Split Engine
Active Zone Gatekeeper: Define a specific MIDI range via Split Low and Split High parameters.

Melodic Pass-Through: 
Any notes played outside the Active Zone bypass the chord engine entirely, passing the original raw MIDI data to the DAW untouched.

Accidental Filtering: 
Black keys (accidentals) inside the active zone currently pass through untouched, ensuring chromatic passing notes don't trigger unwanted diatonic chord clusters.

2. DSP & Thread Optimization
3. 
O(1) Parameter Access: Parameter values are cached as std::atomic<float>* pointers in the processor constructor. This eliminates computationally expensive string-hash lookups (getRawParameterValue) in the high-priority audio thread, ensuring crackle-free performance.

Modulo Math Logic: 
The chord engine uses pitch-class modulo math (noteNumber % 12). It dynamically maps to the played octave, allowing you to shift your Active Zone anywhere on the keyboard and maintain perfect 1-5-8-10 voicing relative to the pressed key.

State-Safe MIDI Routing (Ghost Note Prevention):
Implements an activeChords[128] boolean array. If the user holds a chord and adjusts the split boundaries or scale type mid-performance, the plugin safely intercepts the subsequent NoteOff message and kills the generated voicings, preventing stuck or hanging MIDI notes.

3. UI/UX Refactor
Modern Interface: Dark mode aesthetic (RGB: 30, 32, 34) with clean juce::Rectangle layout slicing.

Binary Scale Toggle: 
Replaced dropdown menus with a streamlined juce::ToggleButton for instant Major/Minor scale switching.

APVTS Integration: 
Front-end UI components are securely bound to backend DSP parameters using juce::AudioProcessorValueTreeState::Attachment classes.

🛠 Tech Stack
Framework: JUCE (C++17/20)

Plugin Format: MIDI FX (AU / VST3)

Platform: macOS (Apple Silicon native) / Logic Pro

🚀 Future Scope / Roadmap
V1.1.5 — DAW State Serialization (Immediate Next Step)

Implement getStateInformation and setStateInformation via XML to ensure APVTS parameter states (Root Key, Split Zones) save and recall perfectly with DAW session files.

V1.2 — The "Accidental" Logic

Expanding the core math to dynamically handle non-diatonic notes.

Intelligent defaulting for chromatic movements to ensure jam sessions stay harmonically "safe."

V1.3 — Standalone Installers

Packaging the plugin into a one-click macOS .pkg installer.

Moving away from source-code compilation for easier distribution to the collective.

V2.0 — The Sequencer & Custom Dictionary

Syncing with DAW BPM via juce::AudioPlayHead.

Expanding the 1-5-8-10 formula to include custom voicings like Sus2, Add9, and custom user-defined dictionaries.

V3.0 — Song Structures

Building a "Section Switcher" to automate transitions between Verse, Chorus, and Bridge voicings in real-time.

📄 License
Internal use for MannaAudio and the Manna Collective.
