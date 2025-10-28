#include "PlayerAudio.h"
#include <cmath>
PlayerAudio::PlayerAudio() {
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio() {
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
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
            // 🔑 Disconnect old source first
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
            transportSource.start();
        }
    }
}


void PlayerAudio::play() {
    transportSource.start();
}

void PlayerAudio::stop() {
    transportSource.stop();
}

void PlayerAudio::setGain(float gain) {
    transportSource.setGain(gain);
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
        
        if (!islooping) { //fixing bug (when closing repeat mid track the track closes)
            auto cPos = getPosition();
            auto length = getLength();

            if (length > 0) {
                setPosition(fmod(cPos, length));
            }
        }

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