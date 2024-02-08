
#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"
#include <string>

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player, AudioFormatManager & 	formatManagerToUse, AudioThumbnailCache & 	cacheToUse) 
        : player(_player), waveformDisplay(formatManagerToUse, cacheToUse)
{

    addAndMakeVisible(playButton); // Add play button
    addAndMakeVisible(stopButton); // Add stop button
    addAndMakeVisible(loadButton); // Add load button
       
    addAndMakeVisible(volSlider);   // Add volume slider
    addAndMakeVisible(speedSlider); // Add speed slider
    addAndMakeVisible(posSlider);   // Add position slider

    addAndMakeVisible(volLabel);    // Add "volume"
    addAndMakeVisible(speedLabel);  // Add "Speed"
    addAndMakeVisible(posLabel);    // Add "Position"

    addAndMakeVisible(waveformDisplay); // Add WaveForms

    playButton.addListener(this); // Playbutton listener
    stopButton.addListener(this); // stopButton listener
    loadButton.addListener(this); // loadButton listener

    volSlider.addListener(this);    // volSlider listener
    speedSlider.addListener(this);  // speedSlider listener 
    posSlider.addListener(this);    // posSlider listener

    volSlider.setRange(0.0, 2.0);                                                               // volSlider range 0 - 2
    volSlider.setNumDecimalPlacesToDisplay(2);                                                  // volSlider only show up to 2 decmial places
    volSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 40, volSlider.getTextBoxHeight());    // Parameter for volSlider number text

    speedSlider.setRange(0.0000001, 10.0);                                                      // speedSlider range 0.0000001 - 10 (breakpoint at 0)
    speedSlider.setNumDecimalPlacesToDisplay(2);                                                // speedSlider only show up to 2 decmial places
    speedSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 40, speedSlider.getTextBoxHeight());// Parameter for speedSlider number text

    posSlider.setRange(0.0, 1.0);                                                               // speedSlider range 0 - 1
    posSlider.setNumDecimalPlacesToDisplay(2);                                                  // posSlider only show up to 2 decmial places
    posSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 40, posSlider.getTextBoxHeight());    // Parameter for posSlider number text

    volSlider.setValue(1.0);    // set volSlider default value
    speedSlider.setValue(1.0);  // set speedSlider default value
    posSlider.setValue(0.0);    // set posSlider default value

    volLabel.setText("Volume", dontSendNotification);   // set label "Volume"
    volLabel.attachToComponent(&volSlider, true);       // attach label to volSlider

    speedLabel.setText("Speed", dontSendNotification);  // set label "Speed"
    speedLabel.attachToComponent(&speedSlider, true);   // attach label to speedSlider

    posLabel.setText("Position", dontSendNotification); // set label "Position"
    posLabel.attachToComponent(&posSlider, true);       // attach label to posSlider

    startTimer(500); // Timer procs every 0.5 secs
}

DeckGUI::~DeckGUI()
{
    stopTimer(); // When program close, stop the timer
}

void DeckGUI::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colours::lightslategrey);   // background color

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("DeckGUI", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 8; 
    double rowW = getWidth() / 8;
    playButton.setBounds(0, 0, getWidth() / 2, rowH);               // Play button parameter
    stopButton.setBounds(getWidth() / 2, 0, getWidth() / 2, rowH);  // Stop button parameter
    loadButton.setBounds(0, rowH, getWidth(), rowH);                // Load button parameter
    volSlider.setBounds(rowW, rowH * 2, getWidth() - rowW, rowH);   // vol slider parameter
    speedSlider.setBounds(rowW, rowH * 3, getWidth() - rowW, rowH); // speed slider parameter
    posSlider.setBounds(rowW, rowH * 4, getWidth() - rowW, rowH);   // position slider parameter
    waveformDisplay.setBounds(0, rowH * 5, getWidth(), rowH * 3);   // waveform parameter
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton) // If play button clicked
    {
        DBG("DeckGUI::Play button was clicked ");
        player->start(); // Call start function
    } 
     if (button == &stopButton) // If stop button clicked
    {
        DBG("DeckGUI::Stop button was clicked ");
        player->stop(); // Call stop function

    }
    if (button == &loadButton) // If load button clicked
    {
        FileChooser chooser {"Select a file..."}; // default placeholder
        if (chooser.browseForFileToOpen()) // If a file is selected
        {
            player->loadURL(URL{chooser.getResult()});          // Call loadURL to read the file
            double length = player->getlength();                // Call getlength to get the duration of the file
            waveformDisplay.loadURL(URL{chooser.getResult()});  // Call loadURL into the waveformdisplay

            File Musicfile(chooser.getResult());                // get file name
            String stringfile = Musicfile.getFileName();        // turn file name into string
            DBG("The name of the file is " << stringfile);
            DBG("The length of the file is " << length);
        }
    }
}

void DeckGUI::sliderValueChanged (Slider *slider)
{
    if (slider == &volSlider) // If volSlider change value
    {
        player->setGain(slider->getValue()); // Change gain to the new value
    }

    if (slider == &speedSlider) // If speedSlider change value
    {
        player->setSpeed(slider->getValue()); // Change speed to the new value
    }
    
    if (slider == &posSlider) // If posSlider change value
    {
        player->setPositionRelative(slider->getValue()); // Change position to the new value
    }
    
}

bool DeckGUI::isInterestedInFileDrag (const StringArray &files)
{
    DBG("DeckGUI::isInterestedInFileDrag");
    return true;                            // If file is dragged into, return true
}

void DeckGUI::filesDropped (const StringArray &files, int x, int y)
{
  DBG("DeckGUI::filesDropped");
  if (files.size() == 1) // If a file is dropped
  {
    player->loadURL(URL{File{files[0]}}); // call loadURL onto the dropped file
  }
}

void DeckGUI::timerCallback()
{
    //DBG("DeckGUI::timerCallback");
    waveformDisplay.setPositionRelative(player->getPositionRelative()); // Whenever timer reset, get position 
}

void DeckGUI::playlistplayer(URL URL) // function for playlistComponent
{
    DBG("DeckGUI::playlistplayer used");
    player->loadURL(URL);           // Call LoadURL to a targeted file/URL to read
    waveformDisplay.loadURL(URL);   // Call LoadURL to a targeted file/URL to form waveform
}
    

