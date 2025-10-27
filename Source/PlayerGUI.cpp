#include "PlayerGUI.h"


PlayerGUI::PlayerGUI()
{
    // Add buttons

    for (auto* btn : { &loadButton, &restartButton , &stopButton, &jumpForward, &jumpBackward,
         &endButton, &playButton, &lastSession })//text buttons
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    for (auto* btn : { &repeatButton, &muteButton }) //toggle buttons
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
    setLookAndFeel(nullptr);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    juce::Colour startColour = juce::Colours::black;
    juce::Colour endColour = juce::Colours::crimson;

    juce::ColourGradient gradient(
        startColour,
        (float)getLocalBounds().getX(),
        (float)getLocalBounds().getY(),
        endColour,
        (float)getLocalBounds().getRight(),
        (float)getLocalBounds().getY(),
        false
    );
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());
}
// Buttons layout 
void PlayerGUI::resized()
{
    auto bounds = getLocalBounds();
    int windowWidth = bounds.getWidth();
    int windowHeight = bounds.getHeight();

    

    trackSlider.setBounds(600, windowHeight - 125, windowWidth- 1200, 20);
    volumeSlider.setBounds(30, windowHeight -  50, windowWidth - 1300, 20);


    std::vector<juce::Button*> orderedButtons = {
        &loadButton, 
        &restartButton,     // restart
        &jumpBackward,      // Jump -10s
        &playButton,        // Play/Pause
        &jumpForward,       // Jump +10s
        &endButton,         // Jump to End
        &stopButton,
    };


    int buttonWidth = 100;
    int buttonHeight = 40;
    int buttonSpacing = 20;
    int totalButtons = (int)orderedButtons.size();


    int totalWidth = (totalButtons * buttonWidth) + ((totalButtons - 1) * buttonSpacing);
    int startx = (windowWidth - totalWidth) / 2;
    int button_y = windowHeight - buttonHeight - 20;
    int currentX = startx;

    for (auto* btn : orderedButtons)
    {
        btn->setBounds(currentX, button_y, buttonWidth, buttonHeight);
        currentX += buttonWidth + buttonSpacing;
    }

    muteButton.setBounds(1340, button_y, 80, 40);
    repeatButton.setBounds(1440, button_y, 80, 40);
    lastSession.setBounds(1340, 100, 150, 40);

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
        playerAudio.Jumptostart();
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

    if (button == &lastSession) {
        juce::File loadedfile = playerAudio.retrievelastfile(); // getting the file
        if (loadedfile.existsAsFile()) { // if it exists
            trackSlider.setRange(0.0, playerAudio.getLength()); // syncing the trackslider with the file 
        }
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