#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
private:
    PlayerAudio playerAudio;
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton startButton{ "Start" };
    juce::TextButton endButton{ "End" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
	juce::ToggleButton repeatButton{ "Repeat" };
    juce::ToggleButton muteButton{ "Mute" };
    juce::TextButton jumpForward{ "+10s" };
    juce::TextButton jumpBackward{ "-10s" };
    juce::Slider volumeSlider;
    juce::Slider trackSlider;
    std::unique_ptr<juce::FileChooser> fileChooser;
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

    void timerCallback() override;
};