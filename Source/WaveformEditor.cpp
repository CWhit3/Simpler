//
//  AudioThumbnailTutorial_04.cpp
//  Simpler
//
//  Created by Christian White on 11/26/18.
//
//

#include "WaveformEditor.hpp"
#include "../JuceLibraryCode/JuceHeader.h"
#include "frontend.h"



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //this class is responsible for drawing the waveform of the sample that has been
    //imported
    SimpleThumbnailComponent::SimpleThumbnailComponent (int sourceSamplesPerThumbnailSample,
                              AudioFormatManager& formatManager,
                              AudioThumbnailCache& cache)
    : thumbnail (sourceSamplesPerThumbnailSample, formatManager, cache)
    {
        thumbnail.addChangeListener (this);
    }
    
    //sets the file to be read from for drawing the waveform
    void SimpleThumbnailComponent::setFile (const File& file)
        {
            thumbnail.setSource (new FileInputSource (file));
        }
    
    //paint function that draws the waveform region depending on if a file has been
    //loaded
    void SimpleThumbnailComponent::paint (Graphics& g)
    {
        if (thumbnail.getNumChannels() == 0)
            paintIfNoFileLoaded (g);
        else
            paintIfFileLoaded (g);
    }
    
    //paint function called when a file has not yet been loaded into the plugin
    void SimpleThumbnailComponent::paintIfNoFileLoaded (Graphics& g)
        {
            g.fillAll (Colours::white);
            g.setColour (Colour(0xffcdcdcd));
            g.drawFittedText ("No File Loaded", getLocalBounds(), Justification::centred, 1.0f);
        }
    
    //paint function called when a file has been loaded into the plugin
    //region is filled with white, then a bounding rectangle is drawn,
    //then the waveform is drawn, separating the stereo audio into left and right
    //channels if both exist and the sample is not mono
    void SimpleThumbnailComponent::paintIfFileLoaded (Graphics& g)
        {
            g.fillAll(Colours::white);
            g.setColour(Colour(0xffcdcdcd));
            g.drawRect(0, 0, getWidth(), getHeight());
            g.setColour (Colour(0xffb4a7d6));
            thumbnail.drawChannels (g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
        }
    
    void SimpleThumbnailComponent::changeListenerCallback (ChangeBroadcaster* source)
        {
            if (source == &thumbnail)
                thumbnailChanged();
        }

    void SimpleThumbnailComponent::thumbnailChanged()
        {
            repaint();
        }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SimplePositionOverlay::SimplePositionOverlay (AudioTransportSource& transportSourceToUse, DummyTagSwitcher& tagSwitcherToUse)
    : transportSource (transportSourceToUse), tagSwitcher(tagSwitcherToUse)
    {
        //initializes waveform markers with one at the very beginning of the sample
        //and another at the end, which aids in graphically drawing the slice selected so that it
        //is still within the bounds of the component
        waveformMarkers = {0.0, transportSource.getLengthInSeconds()};
        indexOfLeftMarkerOfSlice = -1;
        startTimer (40);
    }
    
    
    void SimplePositionOverlay::paint (Graphics& g)
    {
        auto duration = transportSource.getLengthInSeconds();
        
        //division of paint function dedicated to filling a rectangle of low opacity over the selected
        //slice of the sample by the user
        if(indexOfLeftMarkerOfSlice != -1)
        {
            //the locations, in pixels from the left edge of the parent component, of the markers subdividing
            //the sample into the slice selected by the user
            auto leftEdgeOfSlice = (waveformMarkers[indexOfLeftMarkerOfSlice] / duration) * getWidth();
            auto rightEdgeOfSlice = (waveformMarkers[indexOfLeftMarkerOfSlice + 1] / duration) * getWidth();
            
            g.setColour(Colours::orange);
            g.setOpacity(0.3f);
            g.fillRect(leftEdgeOfSlice, 0.0f, rightEdgeOfSlice - leftEdgeOfSlice, (float) getHeight());
        }
        
        //division of paint function dedicated to drawing a vertical line for each marker added by the user
        //by iterating over the set waveformMarkers
        for(int i = 0; i < waveformMarkers.size(); i++)
        {
            auto markerPosition = (waveformMarkers[i] / duration) * getWidth();
            g.setColour(Colours::black);
            g.drawLine (markerPosition, 0.0f, markerPosition, (float) getHeight(), 1.0f);
        }
        
        //division of the paint function responsible for drawing a vertical line at the current position
        //that the sample is being played at
        if (duration > 0.0)
        {
            auto audioPosition = transportSource.getCurrentPosition();
            auto drawPosition = (audioPosition / duration) * getWidth();
            
            g.setColour (Colour(0xffcdcdcd));
            g.drawLine (drawPosition, 0.0f, drawPosition, (float) getHeight(), 2.0f);
        }
    }
    
    //this function is responsible for tracking mouse movement and updating the position of the
    //"playhead" of the waveform editor graphically and logically
    void SimplePositionOverlay::mouseDrag (const MouseEvent& e)
    {
        auto duration = transportSource.getLengthInSeconds();
        
        if (duration > 0.0)
        {
            auto clickPosition = e.position.x;
            auto audioPosition = (clickPosition / getWidth()) * duration;
            
            transportSource.setPosition (audioPosition);
        }
        
        Component* possibleMarkerHandle = getComponentAt(e.position.x, e.position.y);
        if(possibleMarkerHandle->getComponentID() == "MarkerHandle"){
            possibleMarkerHandle->setTopLeftPosition(e.position.x, e.position.y);
        }
    }
    
    //function called within the frontend component which checks if a marker already exists at
    //the current location of the "playhead" and, if a marker does not exist there already,
    //adds a new one to the waveformMarkers set
    void SimplePositionOverlay::addMarkerAtCurrentPosition()
    {
        for(int i = 0; i < waveformMarkers.size(); i++){
            if(waveformMarkers[i] == transportSource.getCurrentPosition())
                return;
        }
        waveformMarkers.push_back(transportSource.getCurrentPosition());
    }
    
    //a sorting algorithm for the double values of the markers in the marker set, which
    //which is done so that the waveformMarkers set's indices reflect the number marker
    //that that is when counting from the left. Logically, it is sorting the double values
    //of each marker from least to greatest
    //NOTE: the sorting algorithm used here is bubble sort, but hopefully this comment will
    //not exist and will be changed to say that it's a different sorting algorithm because
    //bubble sort sucks ass. As it stands now, this is a placeholder sorting algorithm for debugging
    //purposes
    void SimplePositionOverlay::sortMarkers(){
        for(int i = 0; i < waveformMarkers.size() - 1; i++){
            for(int j = 0; j < waveformMarkers.size() - i - 1; j++){
                if(waveformMarkers[j] > waveformMarkers[j+1]){
                    double temp = waveformMarkers[j];
                    waveformMarkers[j] = waveformMarkers[j+1];
                    waveformMarkers[j+1] = temp;
                }
            }
        }
    }
    
    //an overridden double-click function which calculate which marker is closest to the click
    //location of the mouse and then updates the earlier indexOfLeftMarkerOfSlice variable used
    //for graphically drawing the slice of the sample that has been currently selected. Because
    //the sorting algorithm is called following each addition of a marker, the next marker which
    //is the right boundary of the slice is always assumed to be indexOfLeftMarkerOfSlice + 1
    void SimplePositionOverlay::mouseDoubleClick(const MouseEvent& e)
    {
        int minDistToLeftMarker = 100000;
        for(int i = 0; i < waveformMarkers.size(); i++){
            auto currDistToLeftMarker = e.position.x - (waveformMarkers[i] / transportSource.getLengthInSeconds()) * getWidth();
            if(currDistToLeftMarker < minDistToLeftMarker && currDistToLeftMarker > 0){
                indexOfLeftMarkerOfSlice = i;
            }
        }
        transportSource.setPosition(waveformMarkers[indexOfLeftMarkerOfSlice]);
    }
    
    void SimplePositionOverlay::setTransportSourceLengthForFinalMarker(String fileName, double lengthOfSampleInSeconds){
        
        //spoofed transient detection for presentation
        if(fileName == "DL Broken.wav"){
            waveformMarkers = {0.0f, 0.47839, 0.977778, 1.49129, 1.9934, 2.49063,
                2.96946, 3.25227, 3.48746, 3.75753, 3.99007, 4.48109,
                4.98503, 5.48957, 5.97855, 6.26068, 6.4781, 6.749, 6.98658, 7.4929, 8.0f};
        }
        
        else
            waveformMarkers = {0.0f, lengthOfSampleInSeconds};
    }
    
    //getter method for the index of the slice that was last selected by the user by double clicking within
    //its bounds
    int SimplePositionOverlay::getSelectedSliceIndex(){
        return indexOfLeftMarkerOfSlice;
    }
    
    //getter method for the position in seconds of the left marker indicating the beginning of the
    //slice the user last selected
    double SimplePositionOverlay::getPositionFromCurrentSliceIndex(){
        return waveformMarkers[indexOfLeftMarkerOfSlice];
    }
    
    void SimplePositionOverlay::timerCallback()
    {
        repaint();
    }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    MainContentComponent::MainContentComponent()
    : state (Stopped),
    thumbnailCache (5),
    thumbnailComp (512, formatManager, thumbnailCache),
    positionOverlay (transportSource, tagSwitcher)
    {
        //adds the button for opening an audio file to be displayed
        addAndMakeVisible (&openButton);
        openButton.setButtonText ("Open");
        openButton.onClick = [this] { openButtonClicked(); };
        openButton.setColour (TextButton::buttonColourId, Colour(0xffcdcdcd));
        
        //adds the button for playing the audio file from the current position
        addAndMakeVisible (&playButton);
        playButton.setButtonText ("Play");
        playButton.onClick = [this] { playButtonClicked(); };
        playButton.setColour (TextButton::buttonColourId, Colour(0xffcdcdcd));
        playButton.setEnabled (false);
        
        //adds the button for stopping the audio file from playing
        addAndMakeVisible (&stopButton);
        stopButton.setButtonText ("Stop");
        stopButton.onClick = [this] { stopButtonClicked(); };
        stopButton.setColour (TextButton::buttonColourId, Colour(0xffcdcdcd));
        stopButton.setEnabled (false);
        
        //adds the waveform visualization
        addAndMakeVisible (&thumbnailComp);
        addAndMakeVisible (&positionOverlay);
        
        setSize (584, 176);
        
        formatManager.registerBasicFormats();
        transportSource.addChangeListener (this);
        
        setAudioChannels (2, 2);
    }
    
    MainContentComponent::~MainContentComponent()
    {
        this->deleteAllChildren();
        shutdownAudio();
    }
    
    void MainContentComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
        {
            transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
        }
    
    void MainContentComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
    {
        if (readerSource.get() == nullptr)
            bufferToFill.clearActiveBufferRegion();
        else
            transportSource.getNextAudioBlock (bufferToFill);
    }
    
    void MainContentComponent::releaseResources()
    {
        transportSource.releaseResources();
    }
    
    void MainContentComponent::resized()
    {
        //sets the locations of each of the buttons for audio control
        openButton.setBounds (10, 10, 35, 15);
        playButton.setBounds (50, 10, 35, 15);
        stopButton.setBounds (90, 10, 35, 15);
        
        //sets the bounds of the waveform visualization as well as the overlay for position
        //of playhead, markers, slices, etc
        Rectangle<int> thumbnailBounds (10, 30, getWidth() - 20, getHeight() - 60);
        thumbnailComp.setBounds (thumbnailBounds);
        positionOverlay.setBounds (thumbnailBounds);
    }
    
    void MainContentComponent::changeListenerCallback (ChangeBroadcaster* source)
    {
        if (source == &transportSource)
            transportSourceChanged();
    }
    
    //linking method between FrontEnd object and positionOverlay object
    //that passes along the call the add a new marker at the position of the
    //playhead currently. The markers are then sorted so that if the markers
    //are not necessarily clicked from left to right, they will still be in
    //the correct order for graphical rendering later
    void MainContentComponent::addMarkerAtCurrentPosition(){
        positionOverlay.addMarkerAtCurrentPosition();
        positionOverlay.sortMarkers();
    }
    
    //linking method for obtaning the current slice being played in order to swap which
    //tag set is being used for that slice of the sample. E.g. if the first slice has been
    //selected by the user, this method is called in frontend to change the set of tags being displayed
    //in the tagging section as the vector of tag pointers at index 0 of the 2D array tagSetFromGivenSlice
    int MainContentComponent::getSelectedSliceIndex(){
        return positionOverlay.getSelectedSliceIndex();
    }
    
    void MainContentComponent::changeState (TransportState newState)
    {
        if (state != newState)
        {
            state = newState;
            
            switch (state)
            {
                case Stopped:
                    stopButton.setEnabled (false);
                    playButton.setEnabled (true);
                    break;
                    
                case Starting:
                    playButton.setEnabled (false);
                    transportSource.start();
                    break;
                    
                case Playing:
                    stopButton.setEnabled (true);
                    break;
                    
                case Stopping:
                    transportSource.stop();
                    if(positionOverlay.getSelectedSliceIndex() != -1){
                        transportSource.setPosition(positionOverlay.getPositionFromCurrentSliceIndex());
                        positionOverlay.repaint();
                    }
                    break;
                    
                default:
                    jassertfalse;
                    break;
            }
        }
    }
    
    void MainContentComponent::transportSourceChanged()
    {
        if (transportSource.isPlaying())
            changeState (Playing);
        else
            changeState (Stopped);
    }
    
    // Creates a filechooser to selected a file to play, adds a label displaying the filename of
    // the file selected, creates the dummy tag switcher for the demo to handle tagSet swapping,
    // and handles all audio-related logic (this was written by JUCE themselves so I dont totally
    // understand it all enough to explain it)
    void MainContentComponent::openButtonClicked()
    {
        FileChooser chooser ("Select an audio file to sample...",
                             File::nonexistent,
                             "*.wav;*.mp3");
        
        if (chooser.browseForFileToOpen())
        {
            File file = chooser.getResult();
            
            if (auto* reader = formatManager.createReaderFor (file))
            {
                addAndMakeVisible(songName);
                songName.setBounds(130, 10, 385, 15);
                songName.setText(file.getFileName(), juce::dontSendNotification);
                songName.setFont(Font ("Avenir Next", 17.30f, Font::plain).withTypefaceStyle ("Regular"));
                songName.setColour(Label::textColourId, Colour(0xff878787));
                
                //DEMO CODE TO ADD THE TAGSWITCHER IF THE PRESELECTED SAMPLE IS CHOSEN
                if(file.getFileName() == "DL Broken.wav")
                    tagSwitcher.setBounds(10, 30, 564, 95);
                    addAndMakeVisible(tagSwitcher);
                
                std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
                transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                positionOverlay.setTransportSourceLengthForFinalMarker(file.getFileName(), transportSource.getLengthInSeconds());
                playButton.setEnabled (true);
                thumbnailComp.setFile (file);
                readerSource.reset (newSource.release());
                
            }
        }
    }
    
    void MainContentComponent::playButtonClicked()
    {
        changeState (Starting);
    }
    
    void MainContentComponent::stopButtonClicked()
    {
        changeState (Stopping);
    }
    
    //linking method so that when the dummy tag switcher tells its parent (this class)
    //to handle the double click after it has swapped tag sets, this method passes the
    //event to the private variable positionOverlay so that the selected slice can be highlighted,
    //the playhead snaps to the beginning of the selected slice upon stopping, etc.
    void MainContentComponent::mouseDoubleClick(const MouseEvent& e){
        positionOverlay.mouseDoubleClick(e);
    }
    
    void MainContentComponent::mouseDrag(const MouseEvent& e){
        positionOverlay.mouseDrag(e);
    }
    
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
//DUMMY TAG SWITCHER CLASS:
//  This is the class that is simulating features not yet implemented that needed to be present for the live demo.
//  An instance of this class is added on top of the waveformEditor component of the FrontEnd and, if the sample imported has
//  the same filename as the one that will be demoed, all of the marker points and tag names are automatically created
//  by this class. It is also responsible for first response to all double-clicks and simulates changing the tag set of the
//  FrontEnd. Once it has done this, it passes the mouse event to its the positionOverlay of the waveformEditor via a linking method
//  to avoid accessing the overlay itself.
    DummyTagSwitcher::DummyTagSwitcher()
    {   
        lastAccessedSetIndex = -1;
        setSize(584, 176);
    
        //pretermined transients in sample "DL Broken.wav" being used for demo
        spoofedTagSwapSpots = {0.0f, 0.47839, 0.977778, 1.49129, 1.9934,
                               2.49063, 2.96946, 3.25227, 3.48746, 3.75753,
                               3.99007, 4.48109, 4.98503, 5.48957, 5.97855, 
                               6.26068, 6.4781, 6.749, 6.98658, 7.4929, 8.0f};
        
        //descriptions of each slice of the sample hand-written as a placeholder until ML tag generation works
        spoofedSliceTagSets = {{new Tag("Clean"), new Tag("Dry"),                   new Tag("Kick")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Closed Hat")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Snare")},
                               {new Tag("Clean"), new Tag("Dry"), new Tag("Muted"), new Tag("Kick")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Open Hat")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Snare")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Closed Hat")},
                               {new Tag("Clean"), new Tag("Dry"), new Tag("Muted"), new Tag("Snare")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Closed Hat")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Snare")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Kick")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Kick")},
                               {new Tag("Clean"), new Tag("Reverb"),                new Tag("Snare")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Kick")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Open Hat")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Snare")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Closed Hat")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Snare")},
                               {new Tag("Clean"), new Tag("Dry"),                   new Tag("Closed Hat")},
                               {new Tag("Clean"), new Tag("Reverb"),                new Tag("Snare")}};
    }

    DummyTagSwitcher::~DummyTagSwitcher()
    {
        spoofedTagSwapSpots.clear();
        spoofedSliceTagSets.clear();
    }

    void DummyTagSwitcher::paint (Graphics& g){}
    void DummyTagSwitcher::resized(){}
    void DummyTagSwitcher::mouseDrag(const MouseEvent& e)
    {
        //get reference to MainComponentOverlay parent and pass the mouseEvent
        MainContentComponent* innerParent = (MainContentComponent*)getParentComponent();
        innerParent->mouseDrag(e);   
    }
  
    //Iterate through predetermined spots where the tag set changes and handles this via its own fields
    //as well as telling the FrontEnd to delete all of its tags graphically to avoid them being drawn
    //over each other. After this, it sets the tagSet of the FrontEnd to whichever tagSet within this class's
    //spoofed tag sets corresponds to the place the user clicked. The new tag set is then drawn by the FrontEnd
    //via the reorganizeTags method
    void DummyTagSwitcher::mouseDoubleClick(const MouseEvent& e)
    {
    
        for(int i = 0; i < spoofedTagSwapSpots.size() - 1; i++){
            //click was right of the  ith      member's position in the component &
            //click was left  of the (i + 1)th member's position in the component
            if(e.getMouseDownX() > spoofedTagSwapSpots[i]    /8.0f * getWidth() && 
               e.getMouseDownX() < spoofedTagSwapSpots[i + 1]/8.0f * getWidth())
        {
            //get reference to MainComponentOverlay parent and pass the mouseEvent
            MainContentComponent* innerParent = (MainContentComponent*)getParentComponent();
            innerParent->mouseDoubleClick(e);
       
            //tell the FrontEnd to get rid of old tags graphically, store any changes to the
            //previous tagSet (i.e. adding and deleting), update its tagSet, and redraw the new tagSet
            FrontEnd*             outerParent = (FrontEnd*)innerParent->getParentComponent();
            outerParent->deleteAllCurrentTags();
            if(lastAccessedSetIndex > -1)
                spoofedSliceTagSets[lastAccessedSetIndex] = outerParent->tagSet;
            outerParent->tagSet = spoofedSliceTagSets[i];
            outerParent->reorganizeTags();
            lastAccessedSetIndex = i;
            return;
        }
    }

}
