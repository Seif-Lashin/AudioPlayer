#pragma once
#include <JuceHeader.h>
class WaveForm :public juce::Component, public juce::ChangeListener
{
public:
	WaveForm(juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& thumbnailCache);
	~WaveForm() override;


	void paint(juce::Graphics& g)override;
	void resized()override;
	void loadFile(const juce::File& file);

	void setCurrentPosition(double currentPosition);
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
private:
	juce::AudioThumbnail thumbnail;  // object that stores and draws our waveform.
	
	double visibleWave = 20.0; // hardcoded to only show 20 second window
	double currentPosition = 0.0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveForm)

};

