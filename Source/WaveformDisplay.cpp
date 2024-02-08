
#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager & 	formatManagerToUse, AudioThumbnailCache & 	cacheToUse) 
                                 : audioThumb(1000, formatManagerToUse, cacheToUse), fileLoaded(false), position(0)
                          
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

  audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::orange);

    if (fileLoaded)
    {
      audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f); // Drawing the lines
      g.setColour(Colours::lightgreen);
      g.drawRect(position * getWidth(), 0, 1, getHeight()); // Drawing the position box
    }
    else 
    {
      g.setFont (20.0f);
      g.drawText ("No Music?", getLocalBounds(), Justification::centred, true); // Placeholder text for empty box
    }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void WaveformDisplay::loadURL(URL audioURL) // check file to be an audio file, if not return a message
{
  audioThumb.clear();
  fileLoaded  = audioThumb.setSource(new URLInputSource(audioURL)); // checking file is an audio file
  if (fileLoaded)
  {
      DBG("wfd: loaded! ");
      FileName = audioURL.getFileName(); // get file name
      repaint();
  }
  else 
  {
      DBG("wfd: not loaded! ");
  }

}

void WaveformDisplay::changeListenerCallback (ChangeBroadcaster *source)
{
    DBG("wfd: change received! ");

    repaint(); // If any line change recieved, repaint

}

void WaveformDisplay::setPositionRelative(double pos)
{
  if (pos != position && pos > 0)
  {
    position = pos; // Set position to the new value
    repaint();
  }

  
}




