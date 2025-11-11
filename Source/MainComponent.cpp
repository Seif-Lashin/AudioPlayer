#include "MainComponent.h"


std::unique_ptr<juce::PropertiesFile> MainComponent::historySetup(juce::String name)
{
    juce::PropertiesFile::Options options;
    options.applicationName = "Simple Audio Player";
    options.filenameSuffix = name;
    options.osxLibrarySubFolder = "Application Support";
    options.storageFormat = juce::PropertiesFile::storeAsXML;
    options.millisecondsBeforeSaving = 0;
    return std::make_unique<juce::PropertiesFile>(options.getDefaultFile(), options);
}

MainComponent::MainComponent() :
    historyA(historySetup("DeckA settings")),
    historyB(historySetup("DeckB settings")),
    playerA(*historyA), playerB(*historyB), guiA(playerA), guiB(playerB)
{
    addAndMakeVisible(guiA);
    addAndMakeVisible(guiB);
    guiB.setVisible(false); // <-- Hide guiB by default

    mixerSlider.setRange(0.0, 1.0, 0.01);
    mixerSlider.setValue(0.5);
    mixerSlider.addListener(this);
    mixerSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixerSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::black.withAlpha(0.3f));
    mixerSlider.setColour(juce::Slider::trackColourId, juce::Colours::hotpink);
    mixerSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    mixerSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    mixerSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black.withAlpha(0.3f));
    mixerSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(mixerSlider);
    mixerSlider.setVisible(false); // <-- Hide mixerSlider by default

    labelA.setText("Deck A", juce::dontSendNotification);
    labelB.setText("Deck B", juce::dontSendNotification);
    labelA.setJustificationType(juce::Justification::centredRight);
    labelB.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(labelA);
    addAndMakeVisible(labelB);
    labelA.setVisible(false); // <-- Hide labelA by default
    labelB.setVisible(false); // <-- Hide labelB by default

    // Setup the toggle button
    modeToggleButton.setButtonText("Switch to Mixer Mode");
    modeToggleButton.addListener(this);
    addAndMakeVisible(modeToggleButton);
    modeToggleButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.3f));
    modeToggleButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);


    mixerSource.addInputSource(&playerA, false);
    mixerSource.addInputSource(&playerB, false);

    playerA.setMixerGain(0.5f);
    playerB.setMixerGain(0.5f);

    setSize(1200, 850);
    setAudioChannels(0, 2); // no inputs, two outputs
}

MainComponent::~MainComponent()
{
    shutdownAudio();
    mixerSource.removeAllInputs();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    mixerSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    // Only draw the mixer background if not in single player mode
    if (!isSinglePlayerMode)
    {
        auto mixerBounds = getLocalBounds().removeFromBottom(40);
        g.setColour(juce::Colours::black);
        g.fillRect(mixerBounds);
    }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    // Always show the toggle button at the top
    auto topBar = bounds.removeFromTop(40);
    modeToggleButton.setBounds(topBar.reduced(10)); // Give it some margin


    if (isSinglePlayerMode)
    {
        // Single Player Mode: guiA takes up all remaining space
        guiA.setBounds(bounds);

        // Make sure others are not visible and have no space
        guiB.setBounds({});
        mixerSlider.setBounds({});
        labelA.setBounds({});
        labelB.setBounds({});
    }
    else
    {
        // Mixer Mode: Use your original layout
        auto mixerBounds = bounds.removeFromBottom(40);

        guiA.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
        guiB.setBounds(bounds);
        labelA.setBounds(mixerBounds.removeFromLeft(mixerBounds.getWidth() / 2 - 100));
        mixerSlider.setBounds(mixerBounds.removeFromLeft(200));
        labelB.setBounds(mixerBounds);
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &mixerSlider)
    {
        float gainA = (float)(1.0 - mixerSlider.getValue());
        float gainB = (float)(mixerSlider.getValue());
        playerA.setMixerGain(gainA);
        playerB.setMixerGain(gainB);
    }
}

// Implement the button click handler
void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &modeToggleButton)
    {
        // Flip the mode
        isSinglePlayerMode = !isSinglePlayerMode;

        if (isSinglePlayerMode)
        {
            // Now in Single Player Mode
            modeToggleButton.setButtonText("Switch to Mixer Mode");
            guiB.setVisible(false);
            mixerSlider.setVisible(false);
            labelA.setVisible(false);
            labelB.setVisible(false);
        }
        else
        {
            // Now in Mixer Mode
            modeToggleButton.setButtonText("Switch to Single Player");
            guiB.setVisible(true);
            mixerSlider.setVisible(true);
            labelA.setVisible(true);
            labelB.setVisible(true);
        }

        // Trigger a layout update
        resized();
        // Repaint to fix the mixer background
        repaint();
    }
}