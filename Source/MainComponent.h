#pragma once

#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent,
    public juce::Slider::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

private:
    PlayerAudio playerA;
	PlayerAudio playerB;

	PlayerGUI guiA;
	PlayerGUI guiB;

	juce::Slider mixerSlider;
    juce::Label labelA;
	juce::Label labelB;

	juce::MixerAudioSource mixerSource;

    std::unique_ptr<juce::PropertiesFile> historyA;
    std::unique_ptr<juce::PropertiesFile> historyB;
	std::unique_ptr<juce::PropertiesFile> historySetup(juce::String name);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};