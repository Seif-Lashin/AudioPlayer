#include "PlayerGUI.h"
#include <vector> // Make sure this is included for std::vector

PlayerGUI::PlayerGUI()
{
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton, &jumpForward, &jumpBackward,
         &endButton, &playButton, &addMarker })//text buttons
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    for (auto* btn : { &repeatButton, &muteButton, &funButton, &segmentButton }) //toggle buttons
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

	//playlist
    for(auto* btn : { &addTrackButton, &removeTrackButton, &nextTrackButton, &previousTrackButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
	}
	addAndMakeVisible(playlist);
	playlist.addListener(this);
    playlist.setTextWhenNoChoicesAvailable("No Tracks in Playlist<3");
	playlist.setTextWhenNothingSelected("Select a Track");

    //title Logic
    trackLabel.setText(playerAudio.getCurrentTrackName(), juce::dontSendNotification);
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
    trackSlider.setRange(0.0, playerAudio.getLength());
    trackSlider.setValue(0.0);

    for (auto* sli : { &volumeSlider, &trackSlider, &segmentSlider, &speedSlider}) //sliders
    {
        sli->addListener(this);
        addAndMakeVisible(sli);
    }

    addAndMakeVisible(speedLabel);
    speedLabel.setText("Speed:", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredRight);

    speedSlider.setRange(0.5, 2.0, 0.01); // From half-speed to double-speed
    speedSlider.setValue(1.0);
    speedSlider.setSkewFactorFromMidPoint(1.0); // Makes 1.0x the center of the slider
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    speedSlider.textFromValueFunction = [](double value) { return juce::String(value, 2) + "x"; };
    speedSlider.valueFromTextFunction = [](const juce::String& text) { return text.removeCharacters("x").getDoubleValue(); };


	segmentSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    segmentSlider.setRange(0.0, playerAudio.getLength());
	segmentSlider.setMinValue(0.0);
    segmentSlider.setMaxValue(playerAudio.getLength());
	segmentSlider.setVisible(segmentButton.getToggleState());
	playerAudio.setStart(0.0f);
	playerAudio.setEnd(playerAudio.getEnd());

    // Set a reasonable default size
    setSize(1000, 400);
    setAudioChannels(0, 2);
    startTimer(60);
    juce::File lastFile = playerAudio.retrievelastfile();
    if (lastFile.existsAsFile()) {
        playlistFiles.add(lastFile);
        updatePlaylistComboBox();

	    playTrackAtIndex(0);

		double lastPos = playerAudio.getLastPlayedPosition();
        playerAudio.setPosition(lastPos);
   }
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

    juce::Colour NewstartColour = startColour;
    juce::Colour NEWendColour = endColour;

    if (playerAudio.getFunState())
    {
        juce::Colour highlighter = juce::Colours::black;
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
    const int rightClusterWidth = 220; 


    int currentY = margin;
    int rightX = windowWidth - margin - rightClusterWidth;

    currentY += buttonHeight + spacing;
    addMarker.setBounds(rightX, currentY, rightClusterWidth, buttonHeight);
    currentY += buttonHeight + spacing;
    markerList.setBounds(rightX, currentY, rightClusterWidth, comboBoxHeight);


	currentY += comboBoxHeight + spacing * 2;
	playlist.setBounds(rightX, currentY, rightClusterWidth, comboBoxHeight);
	currentY += comboBoxHeight + spacing;

	int halfWidth = (rightClusterWidth - spacing) / 2;
	addTrackButton.setBounds(rightX, currentY, halfWidth, buttonHeight);
	removeTrackButton.setBounds(rightX + halfWidth + spacing, currentY, halfWidth, buttonHeight);
	currentY += buttonHeight + spacing;

	previousTrackButton.setBounds(rightX, currentY, halfWidth, buttonHeight);
	nextTrackButton.setBounds(rightX + halfWidth + spacing, currentY, halfWidth, buttonHeight);

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

	currentX -= (smallButtonWidth + spacing);
	segmentButton.setBounds(currentX, bottomRowY, smallButtonWidth, buttonHeight);


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
    int speedSliderY = sliderY - sliderHeight - spacing - margin * 2;
	int speedLabelWidth = 60;
    speedLabel.setBounds(margin, speedSliderY, speedLabelWidth, sliderHeight);
    speedSlider.setBounds(margin + speedLabelWidth + spacing, speedSliderY, sliderWidth - speedLabelWidth - spacing, sliderHeight);

    trackSlider.setBounds(margin, sliderY, sliderWidth, sliderHeight);

	int sliderY2 = sliderY - spacing - sliderHeight;
    segmentSlider.setBounds(margin, sliderY2, sliderWidth, sliderHeight);
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

                //playlist
                playlistFiles.clear();
				playlistFiles.add(file);
				currentTrackIndex = 0;
                isPlaying = true;
				updatePlaylistComboBox();
				playTrackAtIndex(0);

                updateMarkerList();
            });
    }

    if (button == &restartButton)
    {
        playerAudio.Jumptostart();
        playerAudio.play();
		isPlaying = true;
    }

    if (button == &stopButton)
    {
        playerAudio.stop(); 
		isPlaying = false;
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
		isPlaying = true;
    }

    if (button == &muteButton) {
        playerAudio.mute(muteButton.getToggleState());
    }

    if (button == &addMarker) {
        playerAudio.addPositionAsMarker();
        updateMarkerList();
    }

    if (button == &segmentButton) {
        playerAudio.SegmentToggle(segmentButton.getToggleState());
        if (segmentButton.getToggleState() == false) {
			segmentSlider.setMinValue(playerAudio.getStart());
			segmentSlider.setMaxValue(playerAudio.getEnd());
        }
		segmentSlider.setVisible(segmentButton.getToggleState());
    }

    if (button == &addTrackButton) {
		openFiles();
    }

    if (button == &removeTrackButton) {
		removeSelectedTrack();
    }
    
    if (button == &nextTrackButton) {
		playNextTrack();
    }

    if (button == &previousTrackButton) {
		playPreviousTrack();
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

    if (slider == &segmentSlider) {
		playerAudio.setSegment(segmentSlider.getMinValue(), segmentSlider.getMaxValue());
    }

    if(slider == &speedSlider) {
        playerAudio.setSpeed(slider->getValue());
	}
}

void PlayerGUI::timerCallback()
{
    // This timer callback automatically updates the slider's visual position.
    // We check !isMouseButtonDown() to prevent "fighting" with the user
    // if they are currently dragging the slider.
    if (!trackSlider.isMouseButtonDown())
    {
        if (segmentButton.getToggleState()) {
            playerAudio.segmentPlayCheck();
        }
        trackSlider.setValue(playerAudio.getPosition(), juce::dontSendNotification);
    }
    //playlist
    if (isPlaying && !repeatButton.getToggleState()) {
		double length = playerAudio.getLength();
        if (length > 0 && playerAudio.getPosition() >= length) {
			playNextTrack();
        }
    }

    // Marker selection synchronization
    int markerIdToSelect = playerAudio.markerChecker();
    int currentMarkerId = markerList.getSelectedId();
    if (currentMarkerId != markerIdToSelect) {
        markerList.setSelectedId(markerIdToSelect, juce::dontSendNotification);
    }
    repaint();
}


// Responsible for finding the playback time of the marker and jumping to it.
void PlayerGUI::comboBoxChanged(juce::ComboBox* newComboBox) {
    if(newComboBox == &markerList)
        playerAudio.FindPlayback(newComboBox, markerList);
    if (newComboBox == &playlist) {
		int selectedTrackIndex = playlist.getSelectedId() - 1;

        if (selectedTrackIndex >= 0 && selectedTrackIndex < playlistFiles.size()) {
			playTrackAtIndex(selectedTrackIndex);
        }
    }
}


void PlayerGUI::updateMarkerList() {
    playerAudio.UpdateMarkerList(markerList);
}

void PlayerGUI::updatePlaylistComboBox() {
    playlist.clear(juce::dontSendNotification);

    for (int i = 0; i < playlistFiles.size(); ++i) {
        juce::String trackName = playlistFiles[i].getFileName();
        playlist.addItem(trackName, i + 1); // Item IDs start from 1
    }

    if (currentTrackIndex != -1) {
        playlist.setSelectedId(currentTrackIndex + 1, juce::dontSendNotification);
    }
}

void PlayerGUI::openFiles() {
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select audio files to add...",
        juce::File{},
        "*.wav;*.mp3;");
    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectFiles |
        juce::FileBrowserComponent::canSelectMultipleItems,
        [this](const juce::FileChooser& fc) {
            auto files = fc.getResults();
            if (files.isEmpty()) return;

            for (const auto& file : files) {
				if (!playlistFiles.contains(file))
                    playlistFiles.add(file);
            }
            updatePlaylistComboBox();

            // If no track is currently playing, start playing the first added track
            if (currentTrackIndex == -1 && !playlistFiles.isEmpty()) {
                playTrackAtIndex(0);
            }
        });
}

void PlayerGUI::removeSelectedTrack() {
	int selectedId = playlist.getSelectedId();

	if (selectedId == 0) return; // No selection

	int selectedIndex = selectedId - 1;
	playlistFiles.remove(selectedIndex);
    if (selectedIndex == currentTrackIndex) {
		playerAudio.stop();

        if (playlistFiles.isEmpty()) {
			currentTrackIndex = -1;
			isPlaying = false;
            trackLabel.setText("No Track Loaded", juce::dontSendNotification);
        }
        else {
			int nextIndex = juce::jlimit(0, playlistFiles.size() - 1, selectedIndex);
            playTrackAtIndex(nextIndex);
        }
    }
    else if (selectedIndex < currentTrackIndex) {
		currentTrackIndex--;
    }

	updatePlaylistComboBox();
}

void PlayerGUI::playTrackAtIndex(int index) {
    if (index < 0 || index >= playlistFiles.size()) {
        playerAudio.stop();
        isPlaying = false;
        currentTrackIndex = -1;
		trackLabel.setText("No Track Loaded", juce::dontSendNotification);
        return;
    }

	playerAudio.loadFile(playlistFiles[index]);

    isPlaying = true;
	currentTrackIndex = index;

    trackLabel.setText(playerAudio.getCurrentTrackName(), juce::dontSendNotification);
    trackSlider.setRange(0.0, playerAudio.getLength());
    segmentSlider.setRange(0.0, playerAudio.getLength());
    segmentSlider.setMinValue(0.0);
    segmentSlider.setMaxValue(playerAudio.getLength());
    playerAudio.setStart(0.0f);
    playerAudio.setEnd((float)playerAudio.getLength());
    updateMarkerList();

	playlist.setSelectedId(currentTrackIndex + 1, juce::dontSendNotification);
}

void PlayerGUI::playNextTrack() {
    if (playlistFiles.isEmpty()) return;
    int nextIndex = (currentTrackIndex + 1) % playlistFiles.size();
    playTrackAtIndex(nextIndex);
}

void PlayerGUI::playPreviousTrack() {
    if (playlistFiles.isEmpty()) return;
    int previousIndex = (currentTrackIndex - 1 + playlistFiles.size()) % playlistFiles.size();
    playTrackAtIndex(previousIndex);
}
