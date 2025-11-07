Here is a formal GitHub README based on the provided project files.

---

# Resonate Audio Player

**Resonate** is a robust, dual-deck audio player built from the ground up using the C++ JUCE framework. It serves as a comprehensive project demonstrating modern C++ design principles, separation of concerns, and complex audio processing.

This project was developed as a final assignment for the **CS213 - Object Oriented Programming** course at the **Faculty of Computing and Artificial Intelligence, Cairo University**, under the supervision of **Dr. [cite_start]Mohamed El-Ramly**[cite: 5, 6, 8].

[cite_start] [cite: 217]

---

## 🚀 Key Features

[cite_start]Resonate functions as a feature-rich media player capable of loading and managing `.wav` and `.mp3` audio files[cite: 12].

### Playback & Navigation
* [cite_start]**Dual-Deck Mixing:** Load and play two separate audio tracks simultaneously, with a central crossfade slider to mix between "Deck A" and "Deck B"[cite: 90, 95, 100].
* [cite_start]**Transport Controls:** Standard Play, Stop, Jump to Start, and Jump to End functionalities[cite: 16].
* [cite_start]**Track Navigation:** Skip forward or backward in 10-second intervals[cite: 16].
* [cite_start]**Volume & Mute:** An adjustable volume slider [cite: 17] [cite_start]and a one-click mute toggle that preserves the current volume level[cite: 22].
* [cite_start]**Playback Speed:** Adjustable speed control from 0.5x to 2.0x[cite: 18].
* [cite_start]**Looping:** Toggle to repeat the current track indefinitely[cite: 21].

### Playlist & Session
* [cite_start]**Playlist Management:** A full-featured playlist for each deck, allowing users to add multiple tracks, remove tracks, and navigate with "Next" and "Previous" buttons[cite: 15, 26].
* [cite_start]**Session Persistence:** The application automatically saves the last played track and its exact position for each deck, allowing you to resume your session on startup[cite: 27, 74].

### Advanced Audio Tools
* **Time-Based Markers:** Add markers to the track at any position. [cite_start]A dropdown list allows you to instantly jump to any saved marker[cite: 23].
* [cite_start]**Audio Segmentation:** A toggleable "Segment Mode" with a two-value slider to define a specific "A-B" section of the track and loop it[cite: 24, 79].
* [cite_start]**Metadata Display:** Automatically reads and displays track metadata (Title and Artist) using the TagLib library[cite: 30, 114].

### Visual Interface
* [cite_start]**Scrolling Waveform:** A custom-built waveform visualizer component that displays a 20-second scrolling window of the track's audio[cite: 146, 149].
* [cite_start]**Dynamic Background:** The UI background dynamically changes color based on the audio's real-time Root Mean Square (RMS) level[cite: 28, 86].
* [cite_start]**"Fun Mode":** A toggleable mode that introduces random background colors for visual flair[cite: 28].
* [cite_start]**Real-Time Sync:** A `juce::Timer` keeps the track progression slider and waveform playhead perfectly synchronized with the audio engine[cite: 85, 132].

---

## 🏗️ Technical Architecture

[cite_start]The project's architecture is strictly designed to **decouple the audio engine from the graphical user interface (GUI)**[cite: 69]. This "Separation of Concerns" model makes the code clean, maintainable, and testable.

The application is centered around four primary classes:

* ### `MainComponent`
    [cite_start]This is the core of the application, inheriting from `juce::AudioAppComponent`[cite: 93, 198]. It acts as the "host" that:
    * [cite_start]Owns and manages the two independent decks (A and B)[cite: 90].
    * [cite_start]Creates two `PlayerAudio` (engine) and two `PlayerGUI` (interface) instances[cite: 91, 94, 95].
    * [cite_start]Uses a `juce::MixerAudioSource` to combine the audio from both decks[cite: 95].
    * [cite_start]Manages the central `mixerSlider` and applies the crossfade gain to `playerA` and `playerB`[cite: 100, 101].
    * [cite_start]Handles session persistence by creating and passing `juce::PropertiesFile` instances to each audio engine[cite: 96, 98].

* ### `PlayerAudio`
    [cite_start]This is the **audio engine** for a single deck[cite: 103]. [cite_start]It is completely "headless" and knows nothing about the GUI[cite: 104].
    * [cite_start]It inherits from `juce::AudioSource` so it can be "plugged into" the `MainComponent`'s mixer[cite: 105, 193].
    * [cite_start]It manages the core audio pipeline: `AudioFormatReaderSource` (file reading) -> `AudioTransportSource` (play/stop/gain/position) -> `ResamplingAudioSource` (speed control)[cite: 106, 109, 110, 112].
    * [cite_start]It handles all playback logic and state (looping, muting, segment times, and track markers)[cite: 115].
    * [cite_start]It uses **TagLib** to extract metadata on `loadFile()`[cite: 114].
    * [cite_start]It calculates the audio's RMS level in the `getNextAudioBlock` callback and stores it in an `std::atomic<float>` for the GUI to read safely[cite: 117, 118].

* ### `PlayerGUI`
    [cite_start]This is the **user interface** and **controller** for a single deck[cite: 120].
    * [cite_start]It inherits from `juce::Component` and multiple listener classes (`Button::Listener`, `Slider::Listener`, `Timer`, etc.)[cite: 122, 123].
    * [cite_start]It holds a reference to its corresponding `PlayerAudio` instance (`PlayerAudio& playerAudio`)[cite: 124].
    * It owns all UI elements (buttons, sliders, combo boxes).
    * [cite_start]It translates user actions into commands for the engine (e.g., `playPauseButton` calls `playerAudio.play()`)[cite: 128].
    * [cite_start]Its `timerCallback` runs every 60ms to query the engine for the current position and update the `trackSlider` and `waveform` components[cite: 132, 133, 135].

* ### `WaveForm`
    [cite_start]A specialized, custom `juce::Component` built to visualize the audio[cite: 146].
    * [cite_start]It uses a `juce::AudioThumbnail` to store and draw the low-resolution waveform data[cite: 147].
    * [cite_start]Its `paint()` method is optimized to draw only a 20-second "scrolling window" centered around the playhead, rather than the entire file[cite: 149].

---

## 💡 Core OOP Principles

This project was built to demonstrate a deep understanding of Object-Oriented Programming principles.

1.  **Abstraction**
    Complexity is hidden inside "black boxes." [cite_start]The `PlayerGUI` doesn't know *how* audio is played; it just calls simple, high-level methods like `playerAudio.play()` and `playerAudio.setSpeed()`[cite: 160, 161]. [cite_start]Similarly, the `WaveForm` class abstracts all the complex rendering logic behind a simple `setCurrentPosition()` method[cite: 165, 167].

2.  **Inheritance**
    The project heavily reuses functionality from the JUCE framework. [cite_start]`PlayerGUI` **is-a** `juce::Component` [cite: 189][cite_start], `PlayerAudio` **is-a** `juce::AudioSource` [cite: 193][cite_start], and `MainComponent` **is-a** `juce::AudioAppComponent`[cite: 198]. [cite_start]This allows them to inherit vast amounts of functionality (like being drawn, processing audio, or managing child components) for free[cite: 192, 195].

3.  **Polymorphism**
    Polymorphism is used to handle events and process audio. [cite_start]`PlayerGUI` inherits from multiple abstract listener classes and provides its own unique behavior by overriding their virtual functions (e.g., `buttonClicked()`, `sliderValueChanged()`, `timerCallback()`) [cite: 172-178]. [cite_start]Likewise, `PlayerAudio` overrides `getNextAudioBlock()` to provide its custom audio data to the mixer[cite: 182].

4.  **Encapsulation**
    All data is kept private and protected. [cite_start]Critical state variables (like `islooping`, `trackGain`, `mixerGain`, and `trackMarkers` in `PlayerAudio`) are `private`[cite: 206]. [cite_start]The GUI cannot corrupt this data directly; it must use public methods like `playerAudio.setTrackGain()`[cite: 209, 210], ensuring the engine is always in control of its own state.

---

## 🛠️ Dependencies

* [**JUCE Framework**](https://juce.com/)
* [cite_start][**TagLib**](https://taglib.org/) (for audio file metadata) [cite: 114, 250]

---

## 👨‍💻 Authors

[cite_start]This project was a collaborative effort by[cite: 10]:
* **Seif Mohamed Lashin**
* **Youssef Mohamed Hassib**
* **Anas Ibrahem Hesham**
* **Ahmed Mohamed Kamal**
