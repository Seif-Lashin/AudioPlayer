#pragma once
#include <JuceHeader.h>
#include <vector>

class PlayerAudio : public juce::AudioSource{
private:
    juce::String currentTrackName;
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    juce::StringPairArray trackMetadata;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::PropertiesFile>history; // playeraudio owns its own settings file
    bool islooping = false;
    bool ismuted = false;
    bool isPlaying = true;
    float lastVolume = 0.5;
    std::vector<double> trackMarkers;
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
    void setGainMute(float gain);
    void setPosition(double pos);
    void plus10(double pos);
    void minus10(double pos);
    void Jumptostart();
    void Jumptoend();
    double getPosition() const;
    double getLength() const;
	void repeatToggle(bool shouldRepeat);
    void mute(bool shouldMute);
    const juce::StringPairArray& getMetadata() const { return trackMetadata; }

    
    juce::String getCurrentTrackName() const { return currentTrackName; }

    // NEW: Accessor for the transport source to allow the GUI to listen
    juce::AudioTransportSource& getTransportSource() { return transportSource; }

    //when a new file is loaded
    void savecurrentfilepath(const juce::File& file);
    // when last session is pressed
    juce::File retrievelastfile();


    // add current position to marker list
    void addPositionAsMarker();
    // Referencing the list of markers to show
    const std::vector<double>& getMarkers() const;
    //clears current markers when file is no longer live
    void clearMarkers();

    void FindPlayback(juce::ComboBox* newcombobox, juce::ComboBox & markerList);
    void UpdateMarkerList(juce::ComboBox& markerList);

   
};
