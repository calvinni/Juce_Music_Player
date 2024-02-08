
#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "Songs.h"
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

//==============================================================================
/*
*/
class PlaylistComponent : public Component, public TableListBoxModel, public Button::Listener, public TextEditor::Listener
{
public:
    PlaylistComponent(DJAudioPlayer* _player, AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse, DeckGUI* _Deck1, DeckGUI* _Deck2);
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int getNumRows () override;

    void paintRowBackground(Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    void buttonClicked(Button* button) override;

private:

    TextEditor Searchbar;

    TextButton AddButton{ "ADD TO PLAYLIST" };

    TableListBox tableComponent;

    std::vector<Song> Songs;
    
    DJAudioPlayer* player;
    DeckGUI* Deck1;
    DeckGUI* Deck2;
    
    double length;

    String Getlength(URL audioURL);

    void AddToLibrary();
    void SearchLibrary(String Searchbar);
    void saveLibrary();
    void loadLibrary();
    void loadPlayer(DeckGUI* deckGUI);
    void DeleteFromLibrary(int id);

    bool InsideSong(String fileNameWithoutExtension);
    int SongSearch(String Searchbar);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
