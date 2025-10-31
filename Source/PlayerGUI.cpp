#include "PlayerGUI.h"
#include <vector> // Make sure this is included for std::vector

PlayerGUI::PlayerGUI()
{
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton, &jumpForward, &jumpBackward,
         &endButton, &playButton, &lastSession, &addMarker })//text buttons
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    for (auto* btn : { &repeatButton, &muteButton, &funButton }) //toggle buttons
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }


    //title Logic
    trackLabel.setText("No File Loaded", juce::dontSendNotification);
    trackLabel.setJustificationType(juce::Justification::centred);
    trackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    trackLabel.setFont(juce::Font(30.0f, juce::Font::bold));
    addAndMakeVisible(trackLabel);

    // Marker logic
    addAndMakeVisible(markerList);
    markerList.addListener(this);
    markerList.setTextWhenNoChoicesAvailable("No Markers Available<3");
    markerList.setTextWhenNothingSelected("Select a Marker");

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

    // Track slider
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

    // Set a reasonable default size
    setSize(800, 300);
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

    float rms = playerAudio.getRMS();
    juce::Colour startColour = juce::Colours::purple;
    juce::Colour endColour = juce::Colours::hotpink;
   juce::Colour highlighter = juce::Colours::white;
    juce::Colour NewstartColour = startColour;
    juce::Colour NEWendColour = endColour;

    if (playerAudio.getFunState())
    {
        if (playerAudio.IsPlaying()) {
        auto& random = juce::Random::getSystemRandom();

        startColour = juce::Colour::fromHSV(random.nextFloat(), 1.0f, 1.0f, 1.0f); // hue, sat, brightness respectively
        endColour = juce::Colour::fromHSV(random.nextFloat(), 1.0f, 1.0f, 1.0f);
        highlighter = juce::Colour::fromHSV(random.nextFloat(), 1.0f, 1.0f, 1.0f);
       }
        
      
        float normalRMS = juce::jlimit(0.0f, 1.0f, rms * 5.0f);

        NEWendColour = endColour.interpolatedWith(highlighter, normalRMS);
        NewstartColour = startColour.interpolatedWith(highlighter, normalRMS);
    }

    
    juce::ColourGradient gradient(
        NewstartColour,
        (float)getLocalBounds().getX(),
        (float)getLocalBounds().getY(),
        NEWendColour,
        (float)getLocalBounds().getRight(),
        (float)getLocalBounds().getY(),
        false
    );
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());
}


void PlayerGUI::resized()
{
    auto bounds = getLocalBounds();
    int windowWidth = bounds.getWidth();
    int windowHeight = bounds.getHeight();

    
    const int margin = 15;
    const int spacing = 10;
    const int buttonHeight = 40;
    const int smallButtonWidth = 80;
    const int sliderHeight = 20;
    const int comboBoxHeight = 30;
    const int rightClusterWidth = 150; 


    int currentY = margin;
    int rightX = windowWidth - margin - rightClusterWidth;

    lastSession.setBounds(rightX, currentY, rightClusterWidth, buttonHeight);
    currentY += buttonHeight + spacing;
    addMarker.setBounds(rightX, currentY, rightClusterWidth, buttonHeight);
    currentY += buttonHeight + spacing;
    markerList.setBounds(rightX, currentY, rightClusterWidth, comboBoxHeight);


    const int trackLabelHeight = 30;
    trackLabel.setBounds(margin, margin, rightX - margin - spacing, trackLabelHeight);

   
    int bottomRowY = windowHeight - margin - buttonHeight;

    
    const int volumeSliderWidth = 150;
    
    int volumeSliderY = bottomRowY + (buttonHeight - sliderHeight) / 2;
    volumeSlider.setBounds(margin, volumeSliderY, volumeSliderWidth, sliderHeight);

   
    int mainAreaRightEdge = rightX - spacing;

    int currentX = mainAreaRightEdge - smallButtonWidth;
    repeatButton.setBounds(currentX, bottomRowY, smallButtonWidth, buttonHeight);

    currentX -= (smallButtonWidth + spacing);
    muteButton.setBounds(currentX, bottomRowY, smallButtonWidth, buttonHeight);

    currentX -= (smallButtonWidth + spacing);;
    funButton.setBounds(currentX, bottomRowY, smallButtonWidth, buttonHeight);

    int transportAreaX = margin + volumeSliderWidth + spacing;
    int transportAreaWidth = currentX - spacing - transportAreaX;

    std::vector<juce::Button*> orderedButtons = {
        &loadButton,
        &restartButton,
        &jumpBackward,
        &playButton,
        &jumpForward,
        &endButton,
        &stopButton,
    };

    int numButtons = (int)orderedButtons.size();
    if (numButtons > 0)
    {
      
        int buttonWidth = (transportAreaWidth - (spacing * (numButtons - 1))) / numButtons;

        const int maxButtonWidth = 100;
        int transportX = transportAreaX;

        if (buttonWidth > maxButtonWidth)
        {
            buttonWidth = maxButtonWidth;
            int totalWidth = (numButtons * buttonWidth) + ((numButtons - 1) * spacing);
            transportX = transportAreaX + (transportAreaWidth - totalWidth) / 2;
        }

        for (auto* btn : orderedButtons)
        {
            btn->setBounds(transportX, bottomRowY, buttonWidth, buttonHeight);
            transportX += buttonWidth + spacing;
        }
    }

    
    int sliderY = bottomRowY - spacing - sliderHeight;
    int sliderWidth = mainAreaRightEdge - margin; 
    trackSlider.setBounds(margin, sliderY, sliderWidth, sliderHeight);
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

                trackLabel.setText(playerAudio.getCurrentTrackName(), juce::dontSendNotification);


                //only set the range when loading a new file
                trackSlider.setRange(0.0, playerAudio.getLength());

                // When loading a new file, update (clear) the marker list
                updateMarkerList();
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
      // markerList.setSelectedId(0, juce::dontSendNotification);
    }

    if (button == &jumpBackward) {
        playerAudio.minus10(playerAudio.getPosition());
       // markerList.setSelectedId(0, juce::dontSendNotification);
    }

    if (button == &repeatButton)
    {
        playerAudio.repeatToggle(repeatButton.getToggleState());
    }
    if (button == &funButton) {
        playerAudio.funToggle(funButton.getToggleState());
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


            // Also need to load the markers of the last session                         NOT DONE YET
            updateMarkerList();
      
        }
    
        trackLabel.setText(playerAudio.getCurrentTrackName(), juce::dontSendNotification);

    }


    if (button == &addMarker) {
        playerAudio.addPositionAsMarker();
        updateMarkerList();
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());

    if (slider == &trackSlider)
    {
        // Only update position if the user is dragging the slider
        if (slider->isMouseButtonDown())
        {
            playerAudio.setPosition((float)slider->getValue());
        }
    }
}

void PlayerGUI::timerCallback()
{
    // This timer callback automatically updates the slider's visual position.
    // We check !isMouseButtonDown() to prevent "fighting" with the user
    // if they are currently dragging the slider.
    if (!trackSlider.isMouseButtonDown())
    {
        trackSlider.setValue(playerAudio.getPosition(), juce::dontSendNotification);
    }

    int markerIdToSelect = playerAudio.markerChecker();
    int currentMarkerId = markerList.getSelectedId();
    if (currentMarkerId != markerIdToSelect) {
        markerList.setSelectedId(markerIdToSelect, juce::dontSendNotification);
    }
    repaint();
}


// Responsible for finding the playback time of the marker and jumping to it.
void PlayerGUI::comboBoxChanged(juce::ComboBox* newComboBox) {
    playerAudio.FindPlayback(newComboBox, markerList);
}


void PlayerGUI::updateMarkerList() {
    playerAudio.UpdateMarkerList(markerList);
}