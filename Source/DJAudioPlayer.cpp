
#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager) 
: formatManager(_formatManager)
{

}
DJAudioPlayer::~DJAudioPlayer()
{
    
}

void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void DJAudioPlayer::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    resampleSource.getNextAudioBlock(bufferToFill);

}
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(URL audioURL) // Global file reader
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {       
        std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true)); 
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);             
        readerSource.reset (newSource.release());          
    }
    else
    {
        DBG("Bad file");
    }
}
void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0 || gain > 10.0)
    {
        DBG("DJAudioPlayer::setGain gain should be between 0 and 1");
    }
    else 
    {
        transportSource.setGain(gain); // Call setGain on new value
    }
   
}
void DJAudioPlayer::setSpeed(double ratio)
{
  if (ratio < 0 || ratio > 10.0)
    {
      DBG("DJAudioPlayer::setSpeed ratio should be between 0 and 100");
    }
    else 
    {
        resampleSource.setResamplingRatio(ratio); // Call setResamplingRatio on new value
    }
}
void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
     if (pos < 0 || pos > 1.0)
    {
         DBG("DJAudioPlayer::setPositionRelative pos should be between 0 and 1");
    }
    else 
    {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}


void DJAudioPlayer::start()
{
    transportSource.start(); // Start playing
}
void DJAudioPlayer::stop()
{
    transportSource.stop(); // Stop playing
}

double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds(); // return file relative position by dividing current position with file duration in seconds
}

double DJAudioPlayer::getlength()
{
    return transportSource.getLengthInSeconds(); // return file duration in seconds
}