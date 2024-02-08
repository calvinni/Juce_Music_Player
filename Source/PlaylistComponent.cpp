
#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(DJAudioPlayer* _player, AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse, DeckGUI* _deck1, DeckGUI* _deck2)
                                    : player(_player) , Deck1(_deck1) , Deck2(_deck2)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
      
    addAndMakeVisible(AddButton); // Add AddButton
    AddButton.addListener(this);  // Add AddButton Listener

    addAndMakeVisible(Searchbar);                                                                           // Add Searchbar
    Searchbar.setTextToShowWhenEmpty("Search Songs (enter to submit) (CAPS SENSITIVE)", Colours::orange);   // Add Searchbar placeholder
    Searchbar.onReturnKey = [this] { SearchLibrary(Searchbar.getText()); };                                 // On clicking enter, call SearchLibrary on written text
    
    addAndMakeVisible(tableComponent);                              // Add tableComponent
    tableComponent.getHeader().addColumn("Track title", 1, 440);    // Set Header "Track title"
    tableComponent.getHeader().addColumn("Duration", 2, 100);       // Set Header "Duration"
    tableComponent.getHeader().addColumn("Load player1", 3, 100);   // Set Header "Load player1"
    tableComponent.getHeader().addColumn("Load player2", 4, 100);   // Set Header "Load player2"
    tableComponent.getHeader().addColumn("Del", 5, 50);             // Set Header "Del"
    tableComponent.setModel(this);

    loadLibrary();
}

PlaylistComponent::~PlaylistComponent()
{
    saveLibrary();
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId)); // clear background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("You should not be seeing this", getLocalBounds(), Justification::centred, true);   // draw some placeholder text
    Searchbar.setColour(1, Colours::grey);
}

void PlaylistComponent::resized()
{
    AddButton.setBounds(0, 0, getWidth(), 50);                      // Bounds for Addbutton
    Searchbar.setBounds(0, 50, getWidth(), 30);                     // Bounds for searchbar
    tableComponent.setBounds(0, 80, getWidth(), getHeight() - 80);  // Bounds for tableComponent
}

int PlaylistComponent::getNumRows()
{
    return Songs.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(Colours::lightyellow);
    }
    else
    {
        g.fillAll(Colours::grey);
    }
}

void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (rowNumber < getNumRows())
    {
        if (columnId == 1)
        {
            g.drawText(Songs[rowNumber].Title, 2, 0, width - 4, height, Justification::centredLeft, true); // load Song.Title in column 1
        }

        if (columnId == 2)
        {
            g.drawText(Songs[rowNumber].Length, 2, 0, width - 4, height, Justification::centredLeft, true); // load Song.Length in column 2
        }
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (columnId == 3) //player1 button
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton { "Load into player1" };   // Add textbutton "Load into player1"
            String uniqueid { "A" + std::to_string(rowNumber)};         // String A for delete
            btn->setComponentID(uniqueid);                              // Set ID component string A

            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }

    if (columnId == 4) //player2 button
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton { "Load into player2" };   // Add textbutton "Load into player2"
            String uniqueid { "B" + std::to_string(rowNumber) };        // String B for delete
            btn->setComponentID(uniqueid);                              // Set ID component string B

            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }

    if (columnId == 5) //Delete button
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ " X " };                  // Add textbutton " X "
            String uniqueid { "C" + std::to_string(rowNumber) };        // String C for delete
            btn->setComponentID(uniqueid);                              // Set ID component string C

            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }

    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(Button* button)
{
    if (button == &AddButton)
    {
        DBG("Add button clicked");
        AddToLibrary();     // Call AddToLibrary
    }
    else
    {
        std::string uniqueID = button->getComponentID().toStdString();
        //DBG("This button id is " << uniqueID);                        // For debugging

        if (uniqueID.find("A") != std::string::npos) // Player1 button clicked
        {
            int RowNumber = std::stoi(uniqueID.erase(0, 1)); // Erase the A from the string, turn into int
            
            tableComponent.selectRow(RowNumber);    // Select Appropriate row
            loadPlayer(Deck1);                      // load into Deck1

            //DBG("PlaylistComponent::buttonClicked Player1");          // For debugging
        }

        if (uniqueID.find("B") != std::string::npos) // Player2 button clicked
        {
            int RowNumber = std::stoi(uniqueID.erase(0, 1)); // Erase the B from the string, turn into int
            
            tableComponent.selectRow(RowNumber);    // Select Appropriate row
            loadPlayer(Deck2);                      // load into Deck2

            //DBG("PlaylistComponent::buttonClicked Player2");          // For debugging
        }

        if (uniqueID.find("C") != std::string::npos) // Delete button clicked
        {
            int RowNumber = std::stoi(uniqueID.erase(0, 1)); // Erase the C from the string, turn into int
            
            DeleteFromLibrary(RowNumber);           // Call delete on Appropriate row

            //DBG(Songs[RowNumber].Title + " removed from Library");    // For debugging
            //DBG("PlaylistComponent::buttonClicked Delete");           // For debugging
        }
    }

    tableComponent.updateContent();
}

void PlaylistComponent::AddToLibrary()
{
    FileChooser chooser{ "Select files" };
    if (chooser.browseForFileToOpen())
    {
        for (File& file : chooser.getResults())
        {
            String FileNameNoExt = file.getFileNameWithoutExtension(); // return file name

            if (!InsideSong(FileNameNoExt)) // if not already in the library
            {
                Song AddSong { file };
                URL audioURL { file };
                AddSong.Length = Getlength(audioURL);
                Songs.push_back(AddSong);
                DBG( "loaded file: " << AddSong.Title );
            }
            else // display info message
            {
                DBG( "AddToLibrary:: " << FileNameNoExt << " already loaded" );
            }
        }
    }
}

bool PlaylistComponent::InsideSong(String FileNameNoExt)
{
    return (std::find(Songs.begin(), Songs.end(), FileNameNoExt) != Songs.end()); // if inside, return yes, if not, no
}

String PlaylistComponent::Getlength(URL audioURL) // Get the duration of the file
{
    player->loadURL(audioURL);
    double sec = player->getlength();
    int roundedsec = (std::round(sec)); //round up/down the seconds

    String stringmin = std::to_string(roundedsec / 60); // convert secs into mins
    String stringsec = std::to_string(roundedsec % 60); // find the remanding secs

    if (stringsec.length() < 2) // if seconds are 1 digit or less
    {
        stringsec = stringsec.paddedLeft('0', 2); //add '0' to seconds until seconds = 2
    }

    String Duration{ stringmin + ":" + stringsec }; // Set duration string

    return Duration;
}

void PlaylistComponent::SearchLibrary(String Searchbar)
{
    DBG("Searching library for: " << Searchbar);
    if (Searchbar != "") // If searchbar has something
    {
        int rowNumber = SongSearch(Searchbar); // Find rowNumber based on SongSearch result
        tableComponent.selectRow(rowNumber);   // Select row on rowNumber
    }
    else
    {
        tableComponent.deselectAllRows();     // If no result, deselect all rows
    }
}

int PlaylistComponent::SongSearch(String Searchbar)
{
    // Search Song on searchbar beginning letter and ending letter, returning an int
    auto find = find_if(Songs.begin(), Songs.end(), [&Searchbar](const Song& obj) {return obj.Title.contains(Searchbar); });
    int i = -1;                                 // It must be -1 one so if any search no match, it wont highlight anything

    if (find != Songs.end())
    {
        i = std::distance(Songs.begin(), find); // Find the rowNumber 
    }

    return i;
}

void PlaylistComponent::saveLibrary()
{
    File TrackFile = File::getCurrentWorkingDirectory().getChildFile("Library.csv");    // File in the same working directory
    if (!TrackFile.existsAsFile())                                                      // If file dont exist, create it
    {
        DBG("File doesn't exist, creating now");
        TrackFile.create();
    }

    std::ofstream myLibrary("Library.csv"); // input stream

    for (Song& Song : Songs)
    {
        myLibrary << Song.MFile.getFullPathName() << "," << Song.Length << "\n"; // Insert into CSV, fileFillpath and duration
    }
}

void PlaylistComponent::loadLibrary() // Load everytime it boots up
{
    std::ifstream MyLibrary("Library.csv");
    std::string filePath;
    std::string length;

    if (MyLibrary.is_open())
    {
        while (getline(MyLibrary, filePath, ',')) // While reading the file strings
        {
            File file = filePath; // Convert the std::string into juce::File
            Song newTrack = file; // Have the file be put into Song class

            getline(MyLibrary, length); // Read file string, put into length
            newTrack.Length = length;   // Add found length into class Length
            Songs.push_back(newTrack);  // Add the class into the class vector Songs
        }
    }
    MyLibrary.close();
}

void PlaylistComponent::loadPlayer(DeckGUI* deckGUI)
{
    int selectedrow{ tableComponent.getSelectedRow() }; // Selected row is used to determined which row is being loaded
    
    DBG("PlaylistComponent::loadPlayer selectedrow is " << selectedrow);
    if (selectedrow >= 0) // If there is selected row
    {
        DBG("Adding: " << Songs[selectedrow].Title << " to Player");
        deckGUI->playlistplayer(Songs[selectedrow].URL); // Call playlistplayer on the song URL in the selected row
    }
    else
    {
        DBG("loadPlayer:: please select a valid file");
    }
}

void PlaylistComponent::DeleteFromLibrary(int id)
{
    //DBG("Deleting: " << id); // For debugging
    Songs.erase(Songs.begin() + id); // Delete songs beginning with the selected row ID
}