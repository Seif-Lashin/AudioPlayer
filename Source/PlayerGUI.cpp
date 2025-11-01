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

    for (auto* btn : { &repeatButton, &muteButton }) //toggle buttons
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

    // New: Set up the playlist table
    addAndMakeVisible(playlistTable);
    playlistTable.setModel(this);
    playlistTable.getHeader().addColumn("Track Title", 1, 300); // Column ID 1

    // New: Register as a listener to the PlayerAudio's transport source
    playerAudio.getTransportSource().addChangeListener(this);

    // Set a reasonable default size (adjusted for the playlist table)
    setSize(800, 450);
    setAudioChannels(0, 2);
    startTimer(60);

    // Add new playlist button
    addPlaylistButton.addListener(this);
    addAndMakeVisible(addPlaylistButton);

    // Initialize playlist management with a default playlist
    allPlaylists.add(Playlist("Default Playlist"));

    // Set up the playlist selector ComboBox
    addAndMakeVisible(playlistSelector);
    playlistSelector.addListener(this);
    playlistSelector.addItem("Default Playlist", 1);
    playlistSelector.setSelectedId(1, juce::dontSendNotification); // Select the first playlist



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
    // New: Unregister as a listener
    playerAudio.getTransportSource().removeChangeListener(this);

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

    // --- TOP RIGHT CONTROLS (Moved down to make room for the table) ---
    int topClusterY = margin;

    lastSession.setBounds(rightX, currentY, rightClusterWidth, buttonHeight);
    currentY += buttonHeight + spacing;
    addMarker.setBounds(rightX, currentY, rightClusterWidth, buttonHeight);
    currentY += buttonHeight + spacing;
    markerList.setBounds(rightX, currentY, rightClusterWidth, comboBoxHeight);


    // --- MAIN PLAYBACK CONTROLS AREA ---
    int mainAreaWidth = rightX - spacing;

    // Playlist Table takes up the top-left area (approx 1/3 of the height)
    int playlistTableHeight = windowHeight / 3;
    playlistTable.setBounds(margin, margin, mainAreaWidth - margin, playlistTableHeight);

    // Track Label moved below the table
    /*const int trackLabelHeight = 30;
    trackLabel.setBounds(margin, margin + playlistTableHeight + spacing, mainAreaWidth - margin, trackLabelHeight);*/

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




// New Private Helper Method
void PlayerGUI::loadTrack(int index)
{

    juce::Array<juce::File>& currentPlaylist = getActivePlaylist();

    if (index >= 0 && index < currentPlaylist.size())
    {
        currentTrackIndex = index;
        juce::File trackToLoad = currentPlaylist[currentTrackIndex];

        playerAudio.loadFile(currentPlaylist[currentTrackIndex]);
        playerAudio.play();

        auto& playlist = allPlaylists[currentTrackIndex];
        if (!playlist.tracks.isEmpty())
        {
            playerAudio.loadFile(playlist.tracks[0]); // Load the first track in the selected playlist
        }
        else
        {
            // Optionally handle empty playlist case, e.g. show "No tracks in playlist"
            trackLabel.setText("No tracks in playlist", juce::dontSendNotification);
            return;
        }

        // Update GUI elements
        trackLabel.setText(playerAudio.getCurrentTrackName(), juce::dontSendNotification);
        trackSlider.setRange(0.0, playerAudio.getLength());
        updateMarkerList();

        // Highlight the playing track in the table
        playlistTable.selectRow(currentTrackIndex);

        updateTrackInfoDisplay();

        // Save the currently playing file as the last session file
        if (!allPlaylists[currentTrackIndex].tracks.isEmpty())
            playerAudio.savecurrentfilepath(allPlaylists[currentTrackIndex].tracks[0]);
    }
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

        // Set flags to allow selecting multiple files and directories
        auto fileChooserFlags = juce::FileBrowserComponent::openMode
            | juce::FileBrowserComponent::canSelectFiles
            | juce::FileBrowserComponent::canSelectMultipleItems;

        // New: FileChooser to select multiple files
        fileChooser.reset(new juce::FileChooser("Select audio files to add to playlist...",
            juce::File::getSpecialLocation(juce::File::userMusicDirectory),
            "*.mp3;*.wav"));

        // Use launchAsync with canSelectMultipleItems flag
        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();

                if (results.size() > 0)
                {
                    juce::Array<juce::File>& activeTracks = getActivePlaylist();

                    for (const auto& file : results)
                    {
                        activeTracks.add(file);
                    }

                    // Notify the TableListBox to refresh its view
                    playlistTable.updateContent();

                    // Clear old playlist and add new results
                    /*allPlaylists.clear();
                    allPlaylists.addArray(results);*/

                    // Automatically load the first track
                    loadTrack(0);

                    
                }
                //updateTrackInfoDisplay();
                fileChooser.reset(); fileChooser.reset();

            });
    }
    else if (button == &addPlaylistButton)
    {
        // Logic to add a new playlist
        juce::String newName = "New Playlist " + juce::String(allPlaylists.size() + 1);
        allPlaylists.add(Playlist(newName));

        // Add the new playlist to the selector and switch to it
        int newId = allPlaylists.size();
        playlistSelector.addItem(newName, newId);
        playlistSelector.setSelectedId(newId, juce::sendNotification); // sending a notification triggers comboBoxChanged
    }
       
    if(button == &restartButton)
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
        if(loadedfile.existsAsFile()) { // if it exists
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

// NEW: TableListBoxModel Implementation

int PlayerGUI::getNumRows()
{
    return getActivePlaylist().size();
}

void PlayerGUI::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    juce::Colour colour = (rowIsSelected || rowNumber == currentTrackIndex)
        ? juce::Colours::darkgrey.withAlpha(0.7f) // Highlight selected/playing track
        : juce::Colours::grey.withAlpha(0.2f);

    g.fillAll(colour);
}

// NEW: ChangeListener Implementation (For Track Transition)

void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // Check if the change is from the AudioTransportSource
    if (source == &playerAudio.getTransportSource())
    {
        // Check if the track has finished playing AND we have a valid playlist
        if (!playerAudio.getTransportSource().isPlaying() && playerAudio.getTransportSource().getCurrentPosition() == 0.0 && allPlaylists.size() > 0 && currentTrackIndex != -1)
        {
            // If repeat is toggled, restart the current track.
            if (repeatButton.getToggleState())
            {
                loadTrack(currentTrackIndex); // Reloads the current track, restarting it from 0
                playerAudio.play();
            }
            else
            {
                // CORRECTED LOGIC: Advance to the next track using simple increment
                int nextIndex = currentTrackIndex + 1;
                juce::Array<juce::File>& currentPlaylist = getActivePlaylist(); // Get the active track list
                if (nextIndex < currentPlaylist.size())
                {
                    loadTrack(nextIndex); // Load next track and start playing
                    playerAudio.play();
                }
                else
                {
                    // If we reached the end of the entire playlist, stop.
                    playerAudio.stop();
                    currentTrackIndex = -1; // Reset to no track selected
                    playlistTable.deselectAllRows();
                    trackLabel.setText("Playlist Finished", juce::dontSendNotification);
                }
            }
        }
    }
}


void PlayerGUI::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    juce::Array<juce::File>& currentPlaylist = getActivePlaylist();

    if(rowNumber < currentPlaylist.size()) // ColumnId 1 is the "Track Title" column
    {
        juce::Array<juce::File>& currentPlaylist = getActivePlaylist();
        juce::String filename = allPlaylists[rowNumber].getFileNameWithoutExtension();

        g.setColour(juce::Colours::white);
        g.setFont(height * 0.7f);
        g.drawText(filename,
            2, 0, width - 4, height,
            juce::Justification::centredLeft,
            true);
    }
}

void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{
    // JUCE TableListBox does not have getSelectedNumRows().
    // Instead, use getNumSelectedRows() to check if any rows are selected.
    if(playlistTable.getNumSelectedRows() > 0)
    {
        int selectedRow = playlistTable.getSelectedRow(0);
        loadTrack(selectedRow);
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider) {
        playerAudio.setGain((float)slider->getValue());
    }
    if (slider == &trackSlider)
    {
        // Only update position if the user is dragging the slider
        if(slider->isMouseButtonDown())
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
}

void PlayerGUI::updateTrackInfoDisplay()
{
    // 1. Update the main track label with the title
    trackLabel.setText(playerAudio.getCurrentTrackName(), juce::dontSendNotification);

    // 2. Retrieve the full metadata
    const juce::StringPairArray& metadata = playerAudio.getMetadata();

    // 3. Construct a display string for other metadata (e.g., Artist, Album, Year)
    juce::String artist = metadata.getValue("Artist", "Unknown Artist");
    juce::String album = metadata.getValue("Album", "Unknown Album");
    juce::String year = metadata.getValue("Year", "N/A");

    // Try to find an Artist tag using common keys, falling back to a generic name
    if (artist == "Unknown Artist")
        artist = metadata.getValue("artist", "Unknown Artist");


    juce::String metadataString;
    metadataString << "Artist: " << artist << "\n"
        << "Album: " << album << "\n"
        << "Year: " << year;

    // Update a separate label (you'll need to add this label in PlayerGUI.h/cpp)
    // For this example, let's just use DBG, but you would normally use another juce::Label
    // metadataLabel.setText(metadataString, juce::dontSendNotification); // if you added metadataLabel

    // For now, let's just use the trackLabel for the title and the playlist for everything else.
    // If you want to display the full artist/album, you should add a new juce::Label member 
    // to PlayerGUI.h (e.g., `juce::Label metadataLabel;`) and use the commented line above.
}


//void PlayerGUI::comboBoxChanged(juce::ComboBox* newComboBox) {
//    playerAudio.FindPlayback(newComboBox, markerList);
//}

void PlayerGUI::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &markerList)
    {
        // ... (existing marker list logic)
        playerAudio.FindPlayback(comboBoxThatHasChanged, markerList);
    }
    else if (comboBoxThatHasChanged == &playlistSelector)
    {
        // The item ID starts at 1, so the index is ID - 1
        int newIndex = playlistSelector.getSelectedId() - 1;

        if (newIndex != activePlaylistIndex && newIndex >= 0 && newIndex < allPlaylists.size())
        {
            activePlaylistIndex = newIndex;

            // Stop playback and reset current track index for the new playlist
            playerAudio.stop();
            currentTrackIndex = -1;

            // Refresh the GUI
            playlistTable.updateContent();
            updateTrackInfoDisplay(); // Update display (e.g., to "No File Loaded")
        }
    }
}

void PlayerGUI::updateMarkerList() {
    playerAudio.UpdateMarkerList(markerList);
}