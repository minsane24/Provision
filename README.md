# Provision
**Professional MIDI FX for Live Worship Environments**  
**Manufacturer:** MannaAudio  
**Version:** 1.2 — *The Intelligent Harmonizer Update*  

## 🎹 Overview
Provision is a high-efficiency MIDI FX plugin built in C++ using the JUCE framework. It is designed to transform simple one-finger input into massive, perfectly-voiced 1-5-8-10 "worship pads" while maintaining the flexibility needed for live performance.

With Version 1.2, Provision evolves from a static macro-trigger into an **Intelligent Harmonizer**. It dynamically calculates relative pitch classes (`relPc`) to generate perfect diatonic voicings rooted precisely on the physical key you press, while safely passing non-diatonic notes through untouched.

---

## ✨ Key Features & Architecture (V1.2)

### 1. The Intelligent Harmonizer (New in V1.2)
* **Dynamic Root Generation:** The engine is no longer hardcoded to the white keys. Pressing 'C' in the key of C Major generates a C Major chord; pressing 'D' generates a D minor chord. The physical key pressed is the absolute root of the generated voicing.
* **Relative Pitch Class (`relPc`) Math:** Uses $O(1)$ modulo arithmetic to instantly identify the diatonic scale degree of the pressed key relative to the user's selected `Root Key` and `Scale`.
* **The Non-Diatonic Gatekeeper:** If a musician accidentally (or intentionally) plays a passing note outside of the selected key (e.g., F natural in D Major), the engine recognizes `degree == -1` and safely passes the single note through to the DAW untouched.

### 2. The Performance Split Engine
* **Active Zone:** Define a specific MIDI range via `Split Low` and `Split High` parameters. 
* **Melodic Pass-Through:** Any notes played outside the Active Zone bypass the chord engine entirely, allowing for untouched lead lines in higher octaves.

### 3. DSP & Thread Optimization
* **O(1) Parameter Access:** Parameter values are cached as `std::atomic<float>*` pointers in the processor constructor, eliminating computationally expensive string-hash lookups in the high-priority audio thread.
* **State-Safe MIDI Routing (Ghost Note Prevention):** Implements an `activeChords[128]` boolean array. If the user holds a chord and adjusts the split boundaries or scale type mid-performance, the plugin safely intercepts the subsequent `NoteOff` message to kill the generated voicings, completely preventing stuck MIDI notes.

---

## 🛠 Tech Stack
* **Framework:** JUCE (C++17/20)
* **Plugin Format:** MIDI FX (AU / VST3)
* **Platform:** macOS (Apple Silicon native) / Logic Pro

---

## 🚀 Future Scope / Roadmap

**V1.3 — DAW State Serialization (Immediate Next Step)**
* Implement `getStateInformation` and `setStateInformation` via XML to ensure APVTS parameter states (Root Key, Split Zones) save and recall perfectly with DAW session files.

**V2.0 — Custom Voicing Dictionaries**
* Expanding the 1-5-8-10 formula to include custom voicings like Sus2, Add9, and custom user-defined chord dictionaries.
* Syncing with DAW BPM via `juce::AudioPlayHead` for rhythmic pulsing.

---

## 📄 License
Internal use for **MannaAudio** and the **Manna Collective**.
