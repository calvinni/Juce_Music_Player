
#include "Songs.h"
#include <filesystem>

Song::Song(File _file) : MFile(_file), Title(_file.getFileNameWithoutExtension()), URL(juce::URL{ _file })
{
    DBG("Created new Song with title: " << Title);
}

bool Song::operator == (const String& other) const
{
    return Title == other;
}