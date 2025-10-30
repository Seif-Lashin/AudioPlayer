#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"


class PlayerGUI : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ComboBox::Listener,

    public juce::TableListBoxModel,
    public juce::ChangeListener

{
private:
    PlayerAudio playerAudio;
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Start" };
    juce::TextButton endButton{ "End" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
	juce::ToggleButton repeatButton{ "Repeat" };
    juce::ToggleButton muteButton{ "Mute" };
    juce::TextButton jumpForward{ "+10s" };
    juce::TextButton jumpBackward{ "-10s" };
    juce::TextButton lastSession{ "Last Session" };
    juce::TextButton addMarker{ "Add Marker" };
    juce::ComboBox markerList;
    juce::Slider volumeSlider;
    juce::Slider trackSlider;
    juce::Label trackLabel;
    std::unique_ptr<juce::FileChooser> fileChooser;
    // Playlist members
    juce::Array<juce::File> playlist;
    juce::TableListBox playlistTable;
    int currentTrackIndex = -1;
    
    void updateMarkerList();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)


        // New: TableListBoxModel overrides
        int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;

    // New: ChangeListener override
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // New: Private helper to load a track from the playlist
    void loadTrack(int index);

public:
    PlayerGUI();
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void comboBoxChanged(juce::ComboBox* newComboBox) override;

    void timerCallback() override;
};