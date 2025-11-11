#pragma once

#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent,
    public juce::Slider::Listener,
    public juce::Button::Listener  // <-- Add Button::Listener
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
    void buttonClicked(juce::Button* button) override; // <-- Add buttonClicked declaration

private:
    std::unique_ptr<juce::PropertiesFile> historyA;
    std::unique_ptr<juce::PropertiesFile> historyB;

    PlayerAudio playerA;
    PlayerAudio playerB;

    PlayerGUI guiA;
    PlayerGUI guiB;

    juce::Slider mixerSlider;
    juce::Label labelA;
    juce::Label labelB;

    juce::TextButton modeToggleButton; // <-- Add toggle button
    bool isSinglePlayerMode = true;    // <-- Add state variable, default to true

    juce::MixerAudioSource mixerSource;

    std::unique_ptr<juce::PropertiesFile> historySetup(juce::String name);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};