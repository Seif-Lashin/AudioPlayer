#pragma once
#include <JuceHeader.h>
#include <vector>

class PlayerAudio : public juce::AudioSource{
private:
    juce::String currentTrackName;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::PropertiesFile>history; // playeraudio owns its own settings file
    juce::AudioTransportSource transportSource;
    bool islooping = false;
    bool ismuted = false;
    bool isPlaying = true;
    bool isFun = false;
	bool isSegment = false;
    float lastVolume = 0.5;
    float start = 0.0;
    float end = 0.0;
    std::vector<double> trackMarkers;

    // responsible for background color adaptivness with the soundfile
    std::atomic<float> currentRMS = 0.0f;

    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;
    double currentSampleRate = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void setStart(float Start);
    void setEnd(float End);
    float getStart();
    float getEnd();
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
    void funToggle(bool shouldfun);
    bool getFunState() const;
    void mute(bool shouldMute);
	void SegmentToggle(bool shouldSegment);
	void segmentPlayCheck();
	void setSegment(double Start, double End);
    juce::String getCurrentTrackName() const { return currentTrackName; }
    //when a new file is loaded
    void savecurrentfilepath(const juce::File& file);
    // when last session is pressed
    juce::File retrievelastfile();

    // Markers
    // add current position to marker list
    void addPositionAsMarker();
    // Referencing the list of markers to show
    const std::vector<double>& getMarkers() const;
    //clears current markers when file is no longer live
    void clearMarkers();
    void FindPlayback(juce::ComboBox* newcombobox, juce::ComboBox & markerList);
    void UpdateMarkerList(juce::ComboBox& markerList);
    int markerChecker();


    float getRMS() const { return currentRMS.load(); };
    bool IsPlaying()const;


    void setSpeed(double ratio);


};
