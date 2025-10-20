#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
private:
    PlayerAudio playerAudio;
    juce::TextButton loadButton{ "Load File" };
    juce::TextButton restartButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::Slider volumeSlider;
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

};