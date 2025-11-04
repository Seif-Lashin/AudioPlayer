#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "WaveForm.h"


class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ComboBox::Listener
{
private:
    PlayerAudio& playerAudio;
    WaveForm waveform;
    juce::TextButton loadButton{ "Load File" };
    /*juce::TextButton restartButton{ "Start" };
    juce::TextButton endButton{ "End" };*/
    juce::ToggleButton funButton{ "Fun" };
    juce::TextButton lastSession{ "Last Session" };
    juce::TextButton addMarker{ "Add Marker" };
	juce::ToggleButton segmentButton{ "Segment" };
    juce::ComboBox markerList;
	juce::Slider speedSlider;
    juce::Slider segmentSlider;
    juce::Slider volumeSlider;
    juce::Slider trackSlider;
    juce::Label trackLabel;
	juce::Label speedLabel;
    std::unique_ptr<juce::FileChooser> fileChooser;
    //playlist
    juce::ComboBox playlist;
	juce::TextButton addTrackButton{ "Add to Playlist" };
	juce::TextButton removeTrackButton{ "Remove from Playlist" };

    juce::Array<juce::File> playlistFiles;
	int currentTrackIndex = -1;
	bool isPlaying = false;

    //playPauseButton
	std::unique_ptr<juce::DrawableButton> playPauseButton;
	std::unique_ptr<juce::Drawable> playIcon;
	std::unique_ptr<juce::Drawable> pauseIcon;
    //repeatButton
    std::unique_ptr<juce::DrawableButton> repeatButton;
    std::unique_ptr<juce::Drawable> repeatOffIcon;
    std::unique_ptr<juce::Drawable> repeatOnIcon;
    //nextButton
    std::unique_ptr<juce::DrawableButton> nextButton;
    std::unique_ptr<juce::Drawable> nextIcon;
    //prevButton
    std::unique_ptr<juce::DrawableButton> prevButton;
    std::unique_ptr<juce::Drawable> prevIcon;
	//forwardButton
    std::unique_ptr<juce::DrawableButton> forwardButton;
    std::unique_ptr<juce::Drawable> forwardIcon;
    //replayButton
    std::unique_ptr<juce::DrawableButton> replayButton;
    std::unique_ptr<juce::Drawable> replayIcon;
	//muteButton
    std::unique_ptr<juce::DrawableButton> muteButton;
    std::unique_ptr<juce::Drawable> muteOffIcon;
    std::unique_ptr<juce::Drawable> muteOnIcon;

    void updateMarkerList();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

public:
    PlayerGUI(PlayerAudio& audioToControl);
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void comboBoxChanged(juce::ComboBox* newComboBox) override;

    void timerCallback() override;
private:
	//playlist functions
    void openFiles(); //add tracks
	void removeSelectedTrack(); //remove tracks
	void playTrackAtIndex(int index); //play specific track
	void playNextTrack(); //play next track
	void playPreviousTrack(); //play previous track 
	void updatePlaylistComboBox();
};