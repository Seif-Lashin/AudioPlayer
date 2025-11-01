#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"


class PlayerGUI : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ComboBox::Listener
{
private:
    void updateGUI();
 
    PlayerAudio playerAudio;
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Start" };
    juce::TextButton endButton{ "End" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
	juce::ToggleButton repeatButton{ "Repeat" };
    juce::ToggleButton muteButton{ "Mute" };
    juce::ToggleButton funButton{ "Fun" };
    juce::TextButton jumpForward{ "+10s" };
    juce::TextButton jumpBackward{ "-10s" };
    juce::TextButton lastSession{ "Last Session" };
    juce::TextButton addMarker{ "Add Marker" };
	juce::ToggleButton segmentButton{ "Segment" };
    juce::ComboBox markerList;
    juce::Slider segmentSlider;
    juce::Slider volumeSlider;
    juce::Slider trackSlider;
    juce::Slider speedSlider;
    juce::Label speedLabel;
    juce::Label trackLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    
    void updateMarkerList();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

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