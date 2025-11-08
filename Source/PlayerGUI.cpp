#include "PlayerGUI.h"
#include <vector> // Make sure this is included for std::vector

PlayerGUI::PlayerGUI(PlayerAudio& audioToControl) :playerAudio(audioToControl), 
waveform(playerAudio.getFormatManager(), playerAudio.getThumbnailCache()) // to not duplicate data, formatmanager reads files, thumbnailcache is a shared storage
{
        // playPauseButton
        playIcon = juce::Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize);
        pauseIcon = juce::Drawable::createFromImageData(BinaryData::pause_svg, BinaryData::pause_svgSize);
        playPauseButton = std::make_unique<juce::DrawableButton>("Play/Pause", juce::DrawableButton::ImageFitted);
        playPauseButton->setClickingTogglesState(true);
        playPauseButton->setImages(playIcon.get(), nullptr, nullptr, nullptr, pauseIcon.get());
        playPauseButton->setColour(juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        playPauseButton->setColour(juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);
        playPauseButton->addListener(this);
        addAndMakeVisible(playPauseButton.get());

        // repeatButton
        repeatOffIcon = juce::Drawable::createFromImageData(BinaryData::repeat_svg, BinaryData::repeat_svgSize);
        repeatOnIcon = juce::Drawable::createFromImageData(BinaryData::repeatOn_svg, BinaryData::repeatOn_svgSize);
        repeatButton = std::make_unique<juce::DrawableButton>("Repeat", juce::DrawableButton::ImageFitted);
        repeatButton->setClickingTogglesState(true);
        repeatButton->setImages(repeatOffIcon.get(), nullptr, nullptr, nullptr, repeatOnIcon.get());
        repeatButton->setColour(juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        repeatButton->setColour(juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);
        repeatButton->addListener(this);
        addAndMakeVisible(repeatButton.get());

        // nextButton
        nextIcon = juce::Drawable::createFromImageData(BinaryData::next_svg, BinaryData::next_svgSize);
        nextButton = std::make_unique<juce::DrawableButton>("Next", juce::DrawableButton::ImageFitted);
        nextButton->setImages(nextIcon.get());
        nextButton->setColour(juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        nextButton->addListener(this);
        addAndMakeVisible(nextButton.get());

        // prevButton
        prevIcon = juce::Drawable::createFromImageData(BinaryData::prev_svg, BinaryData::prev_svgSize);
        prevButton = std::make_unique<juce::DrawableButton>("Previous", juce::DrawableButton::ImageFitted);
        prevButton->setImages(prevIcon.get());
        prevButton->setColour(juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        prevButton->addListener(this);
        addAndMakeVisible(prevButton.get());

        // forwardButton
        forwardIcon = juce::Drawable::createFromImageData(BinaryData::forward_svg, BinaryData::forward_svgSize);
        forwardButton = std::make_unique<juce::DrawableButton>("Forward", juce::DrawableButton::ImageFitted);
        forwardButton->setImages(forwardIcon.get());
        forwardButton->setColour(juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        forwardButton->addListener(this);
        addAndMakeVisible(forwardButton.get());

        // replayButton
        replayIcon = juce::Drawable::createFromImageData(BinaryData::replay_svg, BinaryData::replay_svgSize);
        replayButton = std::make_unique<juce::DrawableButton>("Replay", juce::DrawableButton::ImageFitted);
        replayButton->setImages(replayIcon.get());
        replayButton->setColour(juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        replayButton->addListener(this);
        addAndMakeVisible(replayButton.get());

        // muteButton
        muteOffIcon = juce::Drawable::createFromImageData(BinaryData::volumeOff_svg, BinaryData::volumeOff_svgSize);
        muteOnIcon = juce::Drawable::createFromImageData(BinaryData::volumeUp_svg, BinaryData::volumeUp_svgSize);
        muteButton = std::make_unique<juce::DrawableButton>("Mute", juce::DrawableButton::ImageFitted);
        muteButton->setClickingTogglesState(true);
        muteButton->setImages(muteOnIcon.get(), nullptr, nullptr, nullptr, muteOffIcon.get());
        muteButton->setColour(juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
        muteButton->setColour(juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);
        muteButton->addListener(this);
        addAndMakeVisible(muteButton.get());

        // loadButton
        loadButton.addListener(this);
        loadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.3f));
        loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

        addAndMakeVisible(loadButton);

        // addMarker
        addMarker.addListener(this);
        addMarker.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.3f));
        addMarker.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

        addAndMakeVisible(addMarker);

        // funButton
        funButton.addListener(this);
        funButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::hotpink.withAlpha(0.5f));
        funButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.3f));
        funButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        funButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);

        addAndMakeVisible(funButton);

        // segmentButton
        segmentButton.addListener(this);
        segmentButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::hotpink.withAlpha(0.5f));
        segmentButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.3f));
        segmentButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        segmentButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);

        addAndMakeVisible(segmentButton);

        // addTrackButton
        addTrackButton.addListener(this);
        addTrackButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.3f));
        addTrackButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

        addAndMakeVisible(addTrackButton);

        // removeTrackButton
        removeTrackButton.addListener(this);
        removeTrackButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.3f));
        removeTrackButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

        addAndMakeVisible(removeTrackButton);

        // playlist ComboBox
        playlist.addListener(this);
        playlist.setTextWhenNoChoicesAvailable("No Tracks in Playlist<3");
        playlist.setTextWhenNothingSelected("Select a Track");

        playlist.setColour(juce::ComboBox::backgroundColourId, juce::Colours::black.withAlpha(0.3f));
        playlist.setColour(juce::ComboBox::textColourId, juce::Colours::white);
        playlist.setColour(juce::ComboBox::outlineColourId, juce::Colours::white.withAlpha(0.5f));
        playlist.setColour(juce::ComboBox::arrowColourId, juce::Colours::hotpink);
        playlist.setColour(juce::ComboBox::buttonColourId, juce::Colours::transparentBlack);

        addAndMakeVisible(playlist);
        // trackLabel
        trackLabel.setText(playerAudio.getCurrentTrackName(), juce::dontSendNotification);
        trackLabel.setJustificationType(juce::Justification::centred);
        trackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        trackLabel.setFont(juce::Font(30.0f, juce::Font::bold));

        addAndMakeVisible(trackLabel);
        // speedLabel
        speedLabel.setText("Speed:", juce::dontSendNotification);
        speedLabel.setJustificationType(juce::Justification::centredRight);

        speedLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.8f));

        addAndMakeVisible(speedLabel);
        // markerList ComboBox
        markerList.addListener(this);
        markerList.setTextWhenNoChoicesAvailable("No Markers Available<3");
        markerList.setTextWhenNothingSelected("Select a Marker");

        markerList.setColour(juce::ComboBox::backgroundColourId, juce::Colours::black.withAlpha(0.3f));
        markerList.setColour(juce::ComboBox::textColourId, juce::Colours::white);
        markerList.setColour(juce::ComboBox::outlineColourId, juce::Colours::white.withAlpha(0.5f));
        markerList.setColour(juce::ComboBox::arrowColourId, juce::Colours::hotpink);
        markerList.setColour(juce::ComboBox::buttonColourId, juce::Colours::transparentBlack);
        
        addAndMakeVisible(markerList);
        //Waveform
        addAndMakeVisible(waveform);


        // volumeSlider
        volumeSlider.textFromValueFunction = [](double value) { // changing value to percentage
            double percent = value * 100;
            return juce::String((int)percent) + "%";
            };
        volumeSlider.valueFromTextFunction = [](const juce::String& text) { // changing percentage to value
            int percentagePos = text.indexOf("%");
            if (percentagePos != -1) {
                juce::String percent = text.substring(0, percentagePos);
                return percent.getDoubleValue() / 100.0;
            }
            return text.getDoubleValue() / 100.0;
            };
        volumeSlider.setRange(0.0, 1.0, 0.01);
        volumeSlider.setValue(0.5);
        volumeSlider.addListener(this);
		volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
        
        volumeSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::black.withAlpha(0.3f));
        volumeSlider.setColour(juce::Slider::trackColourId, juce::Colours::hotpink);
        volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        volumeSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        volumeSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black.withAlpha(0.3f));
        volumeSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

        addAndMakeVisible(volumeSlider);

        // trackSlider
        trackSlider.textFromValueFunction = [](double value) { // changing the value to a M:SS
            int totalSeconds = (int)value;
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            juce::String secondsStr = juce::String(seconds).paddedLeft('0', 2);
            return juce::String(minutes) + ":" + secondsStr;
            };
        trackSlider.valueFromTextFunction = [](const juce::String& text) { // changing the M:SS to the value
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
        trackSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 80, 20);
        trackSlider.addListener(this);
        
        trackSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::black.withAlpha(0.3f));
        trackSlider.setColour(juce::Slider::trackColourId, juce::Colours::hotpink);
        trackSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        trackSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        trackSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        trackSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        
        addAndMakeVisible(trackSlider);

        // speedSlider
        speedSlider.setRange(0.5, 2.0, 0.01); // From half-speed to double-speed
        speedSlider.setValue(1.0);
        speedSlider.setSkewFactorFromMidPoint(1.0); // Makes 1.0x the center of the slider
        speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
        speedSlider.textFromValueFunction = [](double value) { return juce::String(value, 2) + "x"; };
        speedSlider.valueFromTextFunction = [](const juce::String& text) { return text.removeCharacters("x").getDoubleValue(); };
        speedSlider.addListener(this);
        
        speedSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::black.withAlpha(0.3f));
        speedSlider.setColour(juce::Slider::trackColourId, juce::Colours::hotpink);
        speedSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        speedSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        speedSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black.withAlpha(0.3f));
        speedSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

        addAndMakeVisible(speedSlider);

        // segmentSlider
        segmentSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
        segmentSlider.setRange(0.0, playerAudio.getLength());
        segmentSlider.setMinValue(0.0);
        segmentSlider.setMaxValue(playerAudio.getLength());
        segmentSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        playerAudio.setStart(0.0f);
        playerAudio.setEnd(playerAudio.getEnd());
        segmentSlider.addListener(this);
        
        segmentSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::black.withAlpha(0.3f));
        segmentSlider.setColour(juce::Slider::trackColourId, juce::Colours::hotpink.withAlpha(0.5f));
        segmentSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        
        addAndMakeVisible(segmentSlider);
        segmentSlider.setVisible(segmentButton.getToggleState());

        setSize(1000, 400);
        startTimer(60);

        juce::File lastFile = playerAudio.retrievelastfile();
        if (lastFile.existsAsFile()) {
            playlistFiles.add(lastFile);
            updatePlaylistComboBox();

            double lastPos = playerAudio.getLastPlayedPosition();
            playTrackAtIndex(0);
            playerAudio.setPosition(lastPos);
            waveform.setCurrentPosition(lastPos);
            playerAudio.stop();
            playPauseButton->setToggleState(false, juce::dontSendNotification);
        }
}

PlayerGUI::~PlayerGUI()
{
    setLookAndFeel(nullptr);
}

void PlayerGUI::paint(juce::Graphics& g)
{

    float rms = playerAudio.getRMS();
    juce::Colour startColour = juce::Colours::black;
    juce::Colour endColour = juce::Colour(0xff2d0041);
    juce::Colour highlighter = juce::Colours::black;
    juce::Colour NewstartColour = startColour;
    juce::Colour NEWendColour = endColour;

    if (playerAudio.getFunState() && playerAudio.IsPlaying())
    {
        auto& random = juce::Random::getSystemRandom();

      
        float brightness = juce::jmap(rms, 0.0f, 0.7f, 0.4f, 1.0f);
       

        
        NewstartColour = juce::Colour::fromHSV(random.nextFloat(), 1.0f, brightness, 1.0f);
        NEWendColour = juce::Colour::fromHSV(random.nextFloat(), 1.0f, brightness, 1.0f);
        highlighter = juce::Colour::fromHSV(random.nextFloat(), 1.0f, brightness, 1.0f); //hue, sat, brightness
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
    const int margin = 15;
    const int spacing = 10;

	auto bounds = getLocalBounds().reduced(margin);

    int rowHeight = 30;
    int buttonWidth = 90;
    int sliderHeight = 20;

	//Header
	auto headerRow = bounds.removeFromTop(rowHeight);

    int markerAreaWidth = 100 + spacing + 150;
    auto markerArea = headerRow.removeFromRight(markerAreaWidth);

	headerRow.removeFromRight(spacing * 2);

    addMarker.setBounds(markerArea.removeFromLeft(buttonWidth));
	markerArea.removeFromLeft(spacing);
	markerList.setBounds(markerArea);

	loadButton.setBounds(headerRow.removeFromLeft(buttonWidth));
	headerRow.removeFromLeft(spacing);
	addTrackButton.setBounds(headerRow.removeFromLeft(buttonWidth));
	headerRow.removeFromLeft(spacing);
	removeTrackButton.setBounds(headerRow.removeFromLeft(buttonWidth));
	headerRow.removeFromLeft(spacing);
	playlist.setBounds(headerRow);

	bounds.removeFromTop(spacing * 2);
    
    //Body
	trackLabel.setBounds(bounds.removeFromTop(rowHeight));
	bounds.removeFromTop(spacing);
    
	int waveformHeight = 120;

	waveform.setBounds(bounds.removeFromTop(waveformHeight));
	bounds.removeFromTop(spacing * 2);

    int textBoxHeight = 20;
	trackSlider.setBounds(bounds.removeFromTop(sliderHeight + textBoxHeight));
	bounds.removeFromTop(spacing / 2);

	segmentSlider.setBounds(bounds.removeFromTop(sliderHeight));

	int sliderAreaCenterX = bounds.getCentreX();

    //Footer
	bounds.removeFromTop(spacing * 2);
	int footerRowHeight = 40;
	auto footerBounds = bounds.removeFromTop(footerRowHeight);

	// center buttons in the footer
    int playBtnWidth = 50;
    int transportBtnWidth = 40;

	int playBtnX = sliderAreaCenterX - (playBtnWidth / 2);
	playPauseButton->setBounds(playBtnX, footerBounds.getY(), playBtnWidth, footerRowHeight);

	int replayBtnX = playBtnX - transportBtnWidth - spacing;
	replayButton->setBounds(replayBtnX, footerBounds.getY(), transportBtnWidth, footerRowHeight);

	int prevBtnX = replayBtnX - transportBtnWidth - spacing;
	prevButton->setBounds(prevBtnX, footerBounds.getY(), transportBtnWidth, footerRowHeight);

	int forwardBtnX = playBtnX + transportBtnWidth + spacing;
	forwardButton->setBounds(forwardBtnX, footerBounds.getY(), transportBtnWidth, footerRowHeight);

	int nextBtnX = forwardBtnX + transportBtnWidth + spacing;
	nextButton->setBounds(nextBtnX, footerBounds.getY(), transportBtnWidth, footerRowHeight);

	int centerGroupStartX = prevBtnX;
	int centerGroupEndX = nextBtnX + transportBtnWidth;
    juce::FlexBox leftSettingsGroup;
	leftSettingsGroup.flexDirection = juce::FlexBox::Direction::row;
	leftSettingsGroup.alignItems = juce::FlexBox::AlignItems::center;

    leftSettingsGroup.items.add(juce::FlexItem(speedLabel)
        .withWidth(70)
        .withHeight(30)
        .withMargin(juce::FlexItem::Margin(0, 0, 0, spacing)));
	leftSettingsGroup.items.add(juce::FlexItem(speedSlider)
        .withFlex(1.0)
        .withMinWidth(80.0f)
		.withHeight(sliderHeight)
		.withMargin(juce::FlexItem::Margin(0, spacing, 0, 0)));
    leftSettingsGroup.items.add(juce::FlexItem(*muteButton)
        .withWidth(35)
        .withHeight(35)
		.withMargin(juce::FlexItem::Margin(0, spacing, 0, 0)));
    leftSettingsGroup.items.add(juce::FlexItem(volumeSlider)
        .withFlex(1.0)
		.withMinWidth(80.0f)
		.withHeight(sliderHeight)
		);
    int leftGroupX = footerBounds.getX();
	int leftGroupEndX = centerGroupStartX - spacing * 2;
	int leftGroupWidth = leftGroupEndX - leftGroupX;
    auto leftBound = footerBounds.withX(leftGroupX)
        .withWidth(leftGroupWidth);
    leftSettingsGroup.performLayout(leftBound.toFloat());

	juce::FlexBox rightSettingsGroup;
	rightSettingsGroup.flexDirection = juce::FlexBox::Direction::row;
	rightSettingsGroup.alignItems = juce::FlexBox::AlignItems::center;
	rightSettingsGroup.justifyContent = juce::FlexBox::JustifyContent::center;

    rightSettingsGroup.items.add(juce::FlexItem(*repeatButton)
        .withWidth(35)
        .withHeight(35)
        .withMargin(juce::FlexItem::Margin(0, spacing, 0, 0)));
    rightSettingsGroup.items.add(juce::FlexItem(segmentButton)
        .withWidth(80)
        .withHeight(30)
		.withMargin(juce::FlexItem::Margin(0, spacing, 0, 0)));
    rightSettingsGroup.items.add(juce::FlexItem(funButton)
        .withWidth(80)
        .withHeight(30));
	int rightGroupX = centerGroupEndX + spacing;
	int rightGroupWidth = footerBounds.getRight() - rightGroupX;
    auto rightBound = footerBounds.withX(rightGroupX)
        .withWidth(rightGroupWidth);
	rightSettingsGroup.performLayout(rightBound.toFloat());
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

                waveform.loadFile(file);

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

    if(button == playPauseButton.get())
    {
        if (playPauseButton->getToggleState()) {
            playerAudio.play();
            isPlaying = true;
        }
        else {
			playerAudio.stop();
            isPlaying = false;
        }
	}

    if (button == forwardButton.get()) {
        playerAudio.plus10(playerAudio.getPosition());
    }

    if (button == replayButton.get()) {
        playerAudio.minus10(playerAudio.getPosition());
    }

    if (button == repeatButton.get())
    {
        playerAudio.repeatToggle(repeatButton->getToggleState());
    }

    if (button == &funButton) {
        playerAudio.funToggle(funButton.getToggleState());
    }

    if (button == muteButton.get()) {
        playerAudio.mute(muteButton->getToggleState());
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
    
    if (button == nextButton.get()) {
		playNextTrack();
    }

    if (button == prevButton.get()) {
        if(playerAudio.getPosition() < 1.0) {
            playPreviousTrack();
        }
        else {
            playerAudio.Jumptostart();
            playerAudio.play();
            isPlaying = true;
        }
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGainMute((float)slider->getValue());

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
		playPauseButton->setToggleState(playerAudio.IsPlaying(), juce::dontSendNotification);
    }

    //waveform updater
     waveform.setCurrentPosition(playerAudio.getPosition());
    
    
    //playlist
    if (isPlaying && !repeatButton->getToggleState()) {
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

    waveform.loadFile(playlistFiles[index]);

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
