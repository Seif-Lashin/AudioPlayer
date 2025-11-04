#include "WaveForm.h"


WaveForm::WaveForm(juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& thumbnailcache) :
	thumbnail(100, formatManager, thumbnailcache)
{
	thumbnail.addChangeListener(this); // 
}

WaveForm::~WaveForm() {

}

void WaveForm::loadFile(const juce::File& file) {
    currentPosition = 0.0;
	thumbnail.setSource(new juce::FileInputSource(file)); // this makes it start generating the waveform
}

void WaveForm::setCurrentPosition(double newPosition)
{
	if (newPosition != currentPosition)
	{
		currentPosition = newPosition;
		repaint();
	}
}

void WaveForm::changeListenerCallback(juce::ChangeBroadcaster* source) // signal to redraw itself
{   
    repaint();
}


void WaveForm::paint(juce::Graphics& g) {
	auto waveformBounds = getLocalBounds();
	g.fillAll(juce::Colours::black);

	if (thumbnail.isFullyLoaded()) {
		double totalLength = thumbnail.getTotalLength();

		double startTime = currentPosition - (visibleWave / 2.0); // to center the window on the playhead
		startTime = juce::jlimit(0.0, std::max(0.0, totalLength - visibleWave), startTime); // clamps startTime

		double endTime = startTime + visibleWave; 
		endTime = std::min(endTime, totalLength);

		g.setColour(juce::Colours::hotpink);
		thumbnail.drawChannel(g, waveformBounds, startTime, endTime,1, 1.0f); // draws only between startTime and endTime


		// Drawing the vertical playhead
		float playheadX = (float)juce::jmap(currentPosition, startTime, endTime, (double)waveformBounds.getX(), (double)waveformBounds.getRight()); // maps currentPosition to distance range
		g.setColour(juce::Colours::red.withAlpha(0.8f));          // draws the playhead
		g.drawVerticalLine(juce::roundToInt(playheadX), (float)waveformBounds.getY(), (float)waveformBounds.getBottom());
	}
	else {
		g.setColour(juce::Colours::white);
		g.drawFittedText("Loading...", waveformBounds, juce::Justification::centred, 1);
	}
}

void WaveForm::resized() {
   
}
