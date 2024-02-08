
#pragma once
#include <JuceHeader.h>

class Song
{
    public:
        Song(File _file);

        File MFile;

        URL URL;

        String Title;

        String Length;
    
        bool operator == (const String& other) const;
};