#include "PlayerAudio.h"
#include <cmath>
#include <tag.h>
#include <tstring.h>
#include <fileref.h>
#include <audioproperties.h>
// edited this part to take properties file


PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();

	resamplingSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false);

    // preparing the system file
    juce::PropertiesFile::Options options;
    options.applicationName = "Simple Audio Player";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    options.storageFormat = juce::PropertiesFile::storeAsXML;
    options.millisecondsBeforeSaving = 0;

    // getter for file path
    juce::File settingsfile = options.getDefaultFile();

    //creating the PropertiesFile object and store it in our pointer
    history = std::make_unique<juce::PropertiesFile>(settingsfile, options); 
	currentTrackName = "No Track Loaded";
	retrievelastfile(); // trying to retrieve last session file
}

PlayerAudio::~PlayerAudio() {
    // Saving Position if user exists
    if (getLength() > 0.0 && history != nullptr) {
        history->setValue(key_lastPosition, getPosition());   // saving the current position
        history->saveIfNeeded();                              // forcesave
    }

    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
	currentSampleRate = sampleRate;
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
	resamplingSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    resamplingSource->getNextAudioBlock(bufferToFill);

    float rms = 0.0f;
    int cntChannels = bufferToFill.buffer->getNumChannels();
    for (int channel = 0; channel < cntChannels; ++channel) {
        rms += bufferToFill.buffer->getRMSLevel(channel, bufferToFill.startSample, bufferToFill.numSamples); //getRMSLevel gets the energy of the block
    }

    if (cntChannels > 0) {
        rms /= (float)cntChannels;
    }

    float oldRMS = currentRMS.load(), decayRMS = oldRMS * 0.95f;
    currentRMS.store(std::max(rms, decayRMS));
}

void PlayerAudio::releaseResources() {
    transportSource.releaseResources();
    resamplingSource->releaseResources();
}

void PlayerAudio::loadFile(const juce::File& file) {
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            savecurrentfilepath(file);
            clearMarkers();
            
            //metadata extraction using taglib
            getMetadata(file);

            // clearing out the history
            history->setValue(key_lastPosition, 0.0);
            history->saveIfNeeded();


            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            //repeat
            readerSource->setLooping(islooping); //keep track of state when loading new track


            start = 0;
            end = getLength();
            //mute
            
            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);


            transportSource.setPosition(0.0); // ensuring it starts at 0.
            transportSource.start();
        }
    }
}

void PlayerAudio::play() {
    isPlaying = true;
    transportSource.start();
}

void PlayerAudio::stop() {
    isPlaying = false;
    transportSource.stop();
}

void PlayerAudio::setGain(float gain) {
    transportSource.setGain(gain);
}

void PlayerAudio::setGainMute(float gain) {
    if (ismuted) {
        lastVolume = gain;
    }
    else setGain(gain);
}

void PlayerAudio::setPosition(double pos) {
    transportSource.setPosition(pos);
}

void PlayerAudio::Jumptostart() {
    setPosition(start);
}

void PlayerAudio::Jumptoend() {
    setPosition(end);
}

void PlayerAudio::plus10(double pos) {
    double Length = end; // is the total length of the size file.
    double Next = pos + 10.0;
    if (Length > 0.0) {
        Next = std::min(Next, Length);
    }
    setPosition(Next);
}

void PlayerAudio::minus10(double pos) {
    double Length = end; // is the total length of the size file.
    double Next = pos - 10.0;
    if (Length > 0.0) {
        Next = std::max(Next, (double)start);
    }
   setPosition(Next);
   if (isPlaying)play();
}

double PlayerAudio::getPosition() const {
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const {
    if (readerSource == nullptr) return 1;
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::repeatToggle(bool shouldRepeat) {
    islooping = shouldRepeat; //changing the state of islooping depending on button
    if (readerSource != nullptr) {
        auto cPos = getPosition();
        auto length = getLength();
        if (!islooping) { //fixing bug (when closing repeat mid track the track closes)
            if (length > 0) {
                setPosition(fmod(cPos, length));
            }
        }
        if (isPlaying)play();
        readerSource->setLooping(islooping);
    }
}

void PlayerAudio::funToggle(bool shouldfun) {
    isFun = shouldfun;
}

bool PlayerAudio::getFunState() const {
    return isFun;
}

void PlayerAudio::mute(bool shouldMute) {
    if (ismuted) {
        setGain(lastVolume);
        ismuted = shouldMute;
    }
    else {
        lastVolume = transportSource.getGain();
        setGain(0.0);
        ismuted = shouldMute;
    }
}

juce::File PlayerAudio::retrievelastfile() {
    juce::String lastfilepath = history->getValue(key, juce::String()); // getting the filepath from our history
    if (lastfilepath.isNotEmpty()) {       // if the path isn't empty
        juce::File lastFile(lastfilepath);// we get the file in it
        double lastPos = history->getDoubleValue(key_lastPosition, 0.0);
        loadFile(lastFile);              // loading the file
        setPosition(lastPos);            // setting the last position
        return lastFile;
    }
    else {
        return juce::File();
    }
}

void PlayerAudio::savecurrentfilepath(const juce::File& file) {
    juce::String filepath = file.getFullPathName();  // saving the current file path

    history->setValue(key, filepath);     // putting the filepath in our history file.
    history->saveIfNeeded();             // forcing the save.
}

void PlayerAudio::addPositionAsMarker() {
    double current = getPosition();
    for (double marker : trackMarkers) {
        if (std::abs(marker - current) <= 0.01) {
            return;
        }
    }
    trackMarkers.push_back(current);
    sort(trackMarkers.begin(), trackMarkers.end());
}

const std::vector<double>& PlayerAudio::getMarkers() const {
    return trackMarkers;
}

void PlayerAudio::clearMarkers() {
    trackMarkers.clear();
}

void PlayerAudio::FindPlayback(juce::ComboBox* newComboBox, juce::ComboBox& markerList) {
    if (newComboBox == &markerList) {
        const auto& markers = getMarkers();
        int selectedidx = markerList.getSelectedItemIndex();

        if (selectedidx >= 0 && selectedidx < markers.size()) {
            double playbacktime = markers[selectedidx];
            setPosition(playbacktime);
        }
    }
}

void PlayerAudio::UpdateMarkerList(juce::ComboBox& markerList) {
    markerList.clear(juce::dontSendNotification);

    const auto& markers = getMarkers();
    int markerNumber = 1;
    for (double timestamp : markers) {

        int totalSeconds = (int)timestamp;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        juce::String secondsStr = juce::String(seconds).paddedLeft('0', 2);
        juce::String timeString = juce::String(minutes) + ":" + secondsStr;


        juce::String markerLabel = "Marker " + juce::String(markerNumber) + " (" + timeString + ")";

        //add to combobox
        //first param is text, second is item id
        markerList.addItem(markerLabel, markerNumber);
        markerNumber++;
    }
}

// returns current marker that can be selected
int PlayerAudio::markerChecker() {
    double currentPos = getPosition();
    auto& markers = getMarkers();
    int selectedMarker = 0;
    double approx = 0.75;
    for (int i = 0; i < markers.size(); ++i) {
        double current = markers[i];

        if (std::abs(currentPos - current) <= approx) {
            selectedMarker = i + 1;
            break;
        }
    }
    return selectedMarker;
}

void PlayerAudio::setStart(float Start) {
    start = Start;
}

void PlayerAudio::setEnd(float End) {
    end = End;
}

float PlayerAudio::getStart() {
    return start;
}

float PlayerAudio::getEnd() {
    return end;
}

void PlayerAudio::SegmentToggle(bool shouldSegment) {
    isSegment = shouldSegment;
    if (!isSegment) {
		start = 0.0f;
		end = (float)getLength();
    }
}

void PlayerAudio::segmentPlayCheck() {
    double segStart = start;
    double segEnd = end;
    if (segEnd > segStart)
        if (getPosition() < segStart || getPosition() >= segEnd) {
            setPosition(segStart);
        }
}

void PlayerAudio::setSegment(double Start, double End) {
    start = Start;
    end = End;
}

void PlayerAudio::getMetadata(const juce::File& file) {
    TagLib::FileRef f(file.getFullPathName().toRawUTF8());
    if (!f.isNull() && f.tag()) {
        TagLib::Tag* tag = f.tag();
        title = juce::String(tag->title().toCString(true));
        artist = juce::String(tag->artist().toCString(true));
        if (title.isNotEmpty() || artist.isNotEmpty()) {
            if(title.isEmpty()) title = "UNKNOWN";
            else if(artist.isEmpty()) artist = "UNKNOWN";
            currentTrackName = title + " - " + artist;
        }
        else currentTrackName = file.getFileNameWithoutExtension();
    }
    else currentTrackName = file.getFileNameWithoutExtension();
}

juce::String PlayerAudio::getCurrentTrackName() {
    return currentTrackName;
}

bool PlayerAudio::IsPlaying() const
{
    return transportSource.isPlaying();
}

void PlayerAudio::setSpeed(double ratio) {
    if (resamplingSource != nullptr) {
        resamplingSource->setResamplingRatio(ratio);
    }
}