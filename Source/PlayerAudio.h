#pragma once
#include <JuceHeader.h>
#include <vector>

class PlayerAudio : public juce::AudioSource{
private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    bool islooping = false; //bool to keep track of repeat button
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
public:

    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadFile(const juce::File& file);
    void play();
    void stop();
    void setGain(float gain);
    void setPosition(double pos);
    void nxt10(double pos);
    void bck10(double pos);
    void Jumptostart();
    void Jumptoend();
    double getPosition() const;
    double getLength() const;

	void repeatToggle(bool shouldRepeat);

};
