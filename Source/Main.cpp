#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

// Our application class
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Simple Audio Player"; }
    const juce::String getApplicationVersion() override { return "1.0"; }

    void initialise(const juce::String&) override
    {
        // Create and show the main window
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr; // Clean up
    }



private:
    // The main window of the app
    class MainWindow : public juce::DocumentWindow
    {
    public:
       // Changed the standard OS window to our custom window
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::darkgrey,
                juce::DocumentWindow::closeButton | juce::DocumentWindow::minimiseButton | juce::DocumentWindow::maximiseButton
               
                )
        {
            setUsingNativeTitleBar(false);// not using operating system's title bar
            setOpaque(false);             // to make round corners , ##not done yet##

            setContentOwned(new PlayerGUI(), true);
            centreWithSize(1000, 500);
            setVisible(true);
        }
                
        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    std::unique_ptr<MainWindow> mainWindow;
};

// This macro starts the app
START_JUCE_APPLICATION(SimpleAudioPlayer)