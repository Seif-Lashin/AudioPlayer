#include "PlayerAudio.h"
#include <cmath>

// edited this part to take properties file

const juce::String key = "Last Played";
const juce::String key_lastPosition = "Last Position";
PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();

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
}

PlayerAudio::~PlayerAudio() {
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();

    // Saving Position if user exists

    if (transportSource.getLengthInSeconds() > 0.0 && history != nullptr) {
        history->setValue(key_lastPosition, getPosition());   // saving the current position
        history->saveIfNeeded();                              // forcesave
    }
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources() {
    transportSource.releaseResources();
}

void PlayerAudio::loadFile(const juce::File& file) {
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            juce::Logger::writeToLog("SUCCESS: Created reader for file: " + file.getFileName());
            savecurrentfilepath(file);
            clearMarkers();
            
            juce::String title = reader->metadataValues["title"];
            juce::String artist = reader->metadataValues["artist"];

            if (title.isNotEmpty() && artist.isNotEmpty()) {
                currentTrackName = title + " - " + artist;
            }
            else if (title.isNotEmpty()) {
                currentTrackName = title;
            }
            else {
                currentTrackName = file.getFileName(); 
            }
             currentTrackName = file.getFileName();

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

            //mute
            
            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);


            transportSource.setPosition(0.0); // ensuring it starts at 0.
            transportSource.start();
        }
        else
        {
            // Failed to create reader!
            juce::Logger::writeToLog("ERROR: Failed to create reader for file: " + file.getFileName());
        }
    }
    else
    {
        juce::Logger::writeToLog("ERROR: File does not exist: " + file.getFileName());
    
    }
}


void PlayerAudio::play() {
    isPlaying = true;
    transportSource.start();
}

void PlayerAudio::stop() {
    isPlaying = false;
    transportSource.stop();
    if (transportSource.getLengthInSeconds() > 0 && history != nullptr) {
        history->setValue(key_lastPosition, getPosition());        // putting the current position in history if program stopped or closed
        history->saveIfNeeded();                                   // force save
    }
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
   setPosition(0);
}

void PlayerAudio::Jumptoend() {
   setPosition(getLength());
}

void PlayerAudio::plus10(double pos) {
    double Length = getLength(); // is the total length of the size file.
    double Next = pos + 10.0;
    if (Length > 0.0) {
        Next = std::min(Next, Length);
    }
    setPosition(Next);
}

void PlayerAudio::minus10(double pos) {
    double Length = getLength(); // is the total length of the size file.
    double Next = pos - 10.0;
    if (Length > 0.0) {
        Next = std::max(Next, 0.0);
    }
   setPosition(Next);
   if (isPlaying)play();
}

double PlayerAudio::getPosition() const {
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const {
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


void PlayerAudio::savecurrentfilepath(const juce::File& file) {
    juce::String filepath = file.getFullPathName();  // saving the current file path

    history->setValue(key, filepath);     // putting the filepath in our history file.
    history->saveIfNeeded();             // forcing the save.
}

juce::File PlayerAudio::retrievelastfile() {
    juce::String lastfilepath = history->getValue(key, juce::String()); // getting the filepath from our history

    if (lastfilepath.isNotEmpty()) {       // if the path isn't empty
        juce::File lastFile(lastfilepath);// we get the file in it

        if (lastFile.existsAsFile()) {       // if the file exists
            if (auto* reader = formatManager.createReaderFor(lastFile)) {   // attempts to create an audio-reader for the file

                transportSource.stop();
                transportSource.setSource(nullptr);
                readerSource.reset();

                juce::String title = reader->metadataValues["title"];
                juce::String artist = reader->metadataValues["artist"];

                if (title.isNotEmpty() && artist.isNotEmpty()) {
                    currentTrackName = title + " - " + artist;
                }
                else if (title.isNotEmpty()) {
                    currentTrackName = title;
                }
                else {
                    currentTrackName = lastFile.getFileName(); // Fallback to filename
                }

                // Create new reader source
                readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

               
                transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);


                //getting the saved position, if none its 0.0
                double lastPos = history->getDoubleValue(key_lastPosition, 0.0);   
                transportSource.setPosition(lastPos);
                transportSource.start();


                return lastFile;   // return it
            }

                                                      
        }
    }
    currentTrackName = "No File Loaded Ya 7esba";

    return juce::File(); // returning a dummy file
}


void PlayerAudio::addPositionAsMarker() {
    double current = getPosition();
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