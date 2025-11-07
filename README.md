# Resonate Audio Player

**Resonate** is a robust, dual-deck audio player built from the ground up using the C++ JUCE framework. It serves as a comprehensive project demonstrating modern C++ design principles, separation of concerns, and complex audio processing.

This project was developed as a final assignment for the **CS213 - Object Oriented Programming** course at the **Faculty of Computing and Artificial Intelligence, Cairo University**, under the supervision of **Dr. Mohamed El-Ramly**. 
---

## 🚀 Key Features

Resonate functions as a feature-rich media player capable of loading and managing `.wav` and `.mp3` audio files.

### Playback & Navigation
* **Dual-Deck Mixing:** Load and play two separate audio tracks simultaneously, with a central crossfade slider to mix between "Deck A" and "Deck B".
* **Transport Controls:** Standard Play, Stop, Jump to Start, and Jump to End functionalities.
* **Track Navigation:** Skip forward or backward in 10-second intervals.
* **Volume & Mute:** An adjustable volume slider and a one-click mute toggle that preserves the current volume level.
* **Playback Speed:** Adjustable speed control from 0.5x to 2.0x.
* **Looping:** Toggle to repeat the current track indefinitely.

### Playlist & Session
* **Playlist Management:** A full-featured playlist for each deck, allowing users to add multiple tracks, remove tracks, and navigate with "Next" and "Previous" buttons.
* **Session Persistence:** The application automatically saves the last played track and its exact position for each deck, allowing you to resume your session on startup.

### Advanced Audio Tools
* **Time-Based Markers:** Add markers to the track at any position. A dropdown list allows you to instantly jump to any saved marker.
* **Audio Segmentation:** A toggleable "Segment Mode" with a two-value slider to define a specific "A-B" section of the track and loop it.
* **Metadata Display:** Automatically reads and displays track metadata (Title and Artist) using the TagLib library.

### Visual Interface
* **Scrolling Waveform:** A custom-built waveform visualizer component that displays a 20-second scrolling window of the track's audio.
* **Dynamic Background:** The UI background dynamically changes color based on the audio's real-time Root Mean Square (RMS) level.
* **"Fun Mode":** A toggleable mode that introduces random background colors for visual flair.
* **Real-Time Sync:** A `juce::Timer` keeps the track progression slider and waveform playhead perfectly synchronized with the audio engine.

---

## 🏗️ Technical Architecture

The project's architecture is strictly designed to **decouple the audio engine from the graphical user interface (GUI)**. This "Separation of Concerns" model makes the code clean, maintainable, and testable.

The application is centered around four primary classes:

* ### `MainComponent`
    This is the core of the application, inheriting from `juce::AudioAppComponent`. It acts as the "host" that:
    * Owns and manages the two independent decks (A and B).
    * Creates two `PlayerAudio` (engine) and two `PlayerGUI` (interface) instances.
    * Uses a `juce::MixerAudioSource` to combine the audio from both decks.
    * Manages the central `mixerSlider` and applies the crossfade gain to `playerA` and `playerB`.
    * Handles session persistence by creating and passing `juce::PropertiesFile` instances to each audio engine.

* ### `PlayerAudio`
    This is the **audio engine** for a single deck. It is completely "headless" and knows nothing about the GUI.
    * It inherits from `juce::AudioSource` so it can be "plugged into" the `MainComponent`'s mixer.
    * It manages the core audio pipeline: `AudioFormatReaderSource` (file reading) -> `AudioTransportSource` (play/stop/gain/position) -> `ResamplingAudioSource` (speed control).
    * It handles all playback logic and state (looping, muting, segment times, and track markers).
    * It uses **TagLib** to extract metadata on `loadFile()`.
    * It calculates the audio's RMS level in the `getNextAudioBlock` callback and stores it in an `std::atomic<float>` for the GUI to read safely.

* ### `PlayerGUI`
    This is the **user interface** and **controller** for a single deck.
    * It inherits from `juce::Component` and multiple listener classes (`Button::Listener`, `Slider::Listener`, `Timer`, etc.).
    * It holds a reference to its corresponding `PlayerAudio` instance (`PlayerAudio& playerAudio`).
    * It owns all UI elements (buttons, sliders, combo boxes).
    * It translates user actions into commands for the engine (e.g., `playPauseButton` calls `playerAudio.play()`).
    * Its `timerCallback` runs every 60ms to query the engine for the current position and update the `trackSlider` and `waveform` components.

* ### `WaveForm`
    A specialized, custom `juce::Component` built to visualize the audio.
    * It uses a `juce::AudioThumbnail` to store and draw the low-resolution waveform data.
    * Its `paint()` method is optimized to draw only a 20-second "scrolling window" centered around the playhead, rather than the entire file.

---

## 💡 Core OOP Principles

This project was built to demonstrate a deep understanding of Object-Oriented Programming principles.

1.  **Abstraction**
    Complexity is hidden inside "black boxes". The `PlayerGUI` doesn't know *how* audio is played; it just calls simple, high-level methods like `playerAudio.play()` and `playerAudio.setSpeed()`. Similarly, the `WaveForm` class abstracts all the complex rendering logic behind a simple `setCurrentPosition()` method.

2.  **Inheritance**
    The project heavily reuses functionality from the JUCE framework. `PlayerGUI` **is-a** `juce::Component`, `PlayerAudio` **is-a** `juce::AudioSource`, and `MainComponent` **is-a** `juce::AudioAppComponent`. This allows them to inherit vast amounts of functionality (like being drawn, processing audio, or managing child components) for free.

3.  **Polymorphism**
    Polymorphism is used to handle events and process audio. `PlayerGUI` inherits from multiple abstract listener classes and provides its own unique behavior by overriding their virtual functions (e.g., `buttonClicked()`, `sliderValueChanged()`, `timerCallback()`). Likewise, `PlayerAudio` overrides `getNextAudioBlock()` to provide its custom audio data to the mixer.

4.  **Encapsulation**
    All data is kept private and protected. Critical state variables (like `islooping`, `trackGain`, `mixerGain`, and `trackMarkers` in `PlayerAudio`) are `private`. The GUI cannot corrupt this data directly; it must use public methods like `playerAudio.setTrackGain()`, ensuring the engine is always in control of its own state.

---

## 🛠️ Dependencies

* [**JUCE Framework**](https://juce.com/)
* [**TagLib**](https://taglib.org/) (for audio file metadata)

---

## 👨‍💻 Authors

This project was a collaborative effort by:
* **Seif Mohamed Lashin**
* **Youssef Mohamed Hassib**
* **Anas Ibrahem Hesham**
* **Ahmed Mohamed Kamal**
