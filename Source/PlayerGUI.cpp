#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    // Add buttons
    
    for (auto* btn : { &loadButton,
        &restartButton, 
        &stopButton,
        &jumpForward, 
        &jumpBackward, 
        &startButton, 
        &endButton, 
        &playButton})//text buttons
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    for (auto* btn : { &repeatButton, &muteButton}) //toggle buttons
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider
    volumeSlider.textFromValueFunction = [](double value) {//cahnging value to percentage
        double percent = value * 100;

        return juce::String((int)percent) + "%";
        };
    volumeSlider.valueFromTextFunction = [](const juce::String& text) {//changing percentage to value
        int percentagePos = text.indexOf("%");

        if (percentagePos != -1) {
            juce::String percent = text.substring(0, percentagePos);
            return percent.getDoubleValue() / 100.0;
        }

        return text.getDoubleValue() / 100.0;
        };
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);

    trackSlider.textFromValueFunction = [](double value) {// changing the value to a M:SS
        int totalSeconds = (int)value;

        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        juce::String secondsStr = juce::String(seconds).paddedLeft('0', 2);

        return juce::String(minutes) + ":" + secondsStr;
        };
    trackSlider.valueFromTextFunction = [](const juce::String& text) { //changing the M:SS to the value
        int colonPos = text.indexOf(":");
        if (colonPos != -1) {
            juce::String minutesStr = text.substring(0, colonPos);
            juce::String secondsStr = text.substring(colonPos + 1);

            return minutesStr.getIntValue() * 60.0 + secondsStr.getIntValue();
        }

        return text.getDoubleValue();
        };

    trackSlider.setRange(0.0, 1.0);
    trackSlider.setValue(0.0);
    
    for (auto* sli : { &volumeSlider, &trackSlider }) //sliders
    {
        sli->addListener(this);
        addAndMakeVisible(sli);
    }

    setSize(500, 250);
    setAudioChannels(0, 2);
    startTimer(60); // this starts the timer, 60 updates per second
}

PlayerGUI::~PlayerGUI()
{
    shutdownAudio();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    playButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    repeatButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);
    startButton.setBounds(540, y, 80, 40);
    jumpBackward.setBounds(640, y, 80, 40);
    jumpForward.setBounds(740, y, 80, 40);
    endButton.setBounds(840, y, 80, 40);
    restartButton.setBounds(940, y, 80, 40);


    trackSlider.setBounds(20, 70, getWidth() - 40, 20);
    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                playerAudio.loadFile(file);

                //only set the range when loading a new file
                trackSlider.setRange(0.0, playerAudio.getLength());
            });
    }

    if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.play();
    }

    if (button == &stopButton)
    {
        playerAudio.stop();
    }

    if (button == &jumpForward) {
        playerAudio.plus10(playerAudio.getPosition());
    }

    if (button == &jumpBackward) {
        playerAudio.minus10(playerAudio.getPosition());
    }
  
    if (button == &repeatButton)
    {
		    playerAudio.repeatToggle(repeatButton.getToggleState());
    }
    if (button == &startButton)
    {
        playerAudio.Jumptostart();
    }
    if (button == &endButton)
    {
        playerAudio.Jumptoend();
    }
    if (button == &playButton)
    {
        playerAudio.play();
    }
    if (button == &muteButton) {
        playerAudio.mute(muteButton.getToggleState());
    }
}   

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
    if (slider == &trackSlider)
        // this is the manual update
        playerAudio.setPosition((float)slider->getValue());
}

void PlayerGUI::timerCallback() 
{
    if (!trackSlider.isMouseButtonDown())
        // this automatically updates the position as the track goes on, also prevents fighting with user
        trackSlider.setValue(playerAudio.getPosition(), juce::dontSendNotification); 
}