/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AudioThumbnailTutorial
 version:          4.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Plays audio files and displays a thumbnail.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2017, linux_make

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

// example implementation of the Exercise 3
//==============================================================================




//PLEASE NOTE THAT PORTIONS OF THIS CODE HAVE COME FROM AN EXAMPLE PROJECT PROVIDED
//IN THE JUCE DOWNLOAD. AS STATED ABOVE IN THE HEADER WRITTEN BY JUCE, WE ARE ALLOWED
//TO USE/MODIFY THIS CODE AS LONG AS THE ABOVE HEADER REMAINS
//                                                          -Simpler devs



//this class is responsible for drawing the waveform of the sample that has been
//imported
class SimpleThumbnailComponent : public Component,
                                 private ChangeListener
{
public:
    SimpleThumbnailComponent (int sourceSamplesPerThumbnailSample,
                              AudioFormatManager& formatManager,
                              AudioThumbnailCache& cache)
       : thumbnail (sourceSamplesPerThumbnailSample, formatManager, cache)
    {
        thumbnail.addChangeListener (this);
    }
    
    //sets the file to be read from for drawing the waveform
    void setFile (const File& file)
    {
        thumbnail.setSource (new FileInputSource (file));
    }

    //paint function that draws the waveform region depending on if a file has been
    //loaded
    void paint (Graphics& g) override
    {
        if (thumbnail.getNumChannels() == 0)
            paintIfNoFileLoaded (g);
        else
            paintIfFileLoaded (g);
    }

    //paint function called when a file has not yet been loaded into the plugin
    void paintIfNoFileLoaded (Graphics& g)
    {
        g.fillAll (Colours::white);
        g.setColour (Colour(0xffcdcdcd));
        g.drawFittedText ("No File Loaded", getLocalBounds(), Justification::centred, 1.0f);
    }
    
	//paint function called when a file has been loaded into the plugin
	//region is filled with white, then a bounding rectangle is drawn,
	//then the waveform is drawn, separating the stereo audio into left and right
	//channels if both exist and the sample is not mono
    void paintIfFileLoaded (Graphics& g)
    {
        g.fillAll(Colours::white);
        g.setColour(Colour(0xffcdcdcd));
        g.drawRect(0, 0, getWidth(), getHeight());
        g.setColour (Colour(0xffb4a7d6));
        thumbnail.drawChannels (g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            thumbnailChanged();
    }

private:
    void thumbnailChanged()
    {
        repaint();
    }

    AudioThumbnail thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleThumbnailComponent)
};

//------------------------------------------------------------------------------

class SimplePositionOverlay : public Component,
                              public MouseListener,
                              private Timer
{
public:

    //set of all double values representing the time (in seconds)
    //at which a marker was created by the user
    std::vector<double> waveformMarkers;
    
    //index used for graphical drawing of the current selected slice of the sample
    //which is updated in the overriden mouseDoubleClick function
    int indexOfLeftMarkerOfSlice = -1;
    
    SimplePositionOverlay (AudioTransportSource& transportSourceToUse)
       : transportSource (transportSourceToUse)
    {
        //initializes waveform markers with one at the very beginning of the sample
        //and another at the end, which aids in graphically drawing the slice selected so that it
        //is still within the bounds of the component
        waveformMarkers = {0.0, transportSource.getLengthInSeconds()};
        startTimer (40);
    }
      

    void paint (Graphics& g) override
    {
        auto duration = transportSource.getLengthInSeconds();
        
        //division of paint function dedicated to filling a rectangle of low opacity over the selected
        //slice of the sample by the user
        if(indexOfLeftMarkerOfSlice > -1)
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
    void mouseDrag (const MouseEvent& event) override
    {
        auto duration = transportSource.getLengthInSeconds();

        if (duration > 0.0)
        {
            auto clickPosition = event.position.x;
            auto audioPosition = (clickPosition / getWidth()) * duration;

            transportSource.setPosition (audioPosition);
        }
    }
    
    //function called within the frontend component which checks if a marker already exists at
    //the current location of the "playhead" and, if a marker does not exist there already,
    //adds a new one to the waveformMarkers set
    void addMarkerAtCurrentPosition()
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
    void sortMarkers(){
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
     
    //an overridden double-click function which calculate which marker is closest the click
    //location of the mouse and then updates the earlier indexOfLeftMarkerOfSlice variable used
    //for graphically drawing the slice of the sample that has been currently selected. Because
    //the sorting algorithm is called following each addition of a marker, the next marker which 
    //is the right boundary of the slice is assumed to be indexOfLeftMarkerOfSlice + 1
    void mouseDoubleClick(const MouseEvent& e)override
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
    void setTransportSourceLengthForFinalMarker(double lengthOfSampleInSeconds){
        waveformMarkers = {0.0f, lengthOfSampleInSeconds};
    }
    
    int getSelectedSliceIndex(){
        return indexOfLeftMarkerOfSlice;
    }

private:
    void timerCallback() override
    {
        repaint();
    }

    AudioTransportSource& transportSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimplePositionOverlay)
};

//------------------------------------------------------------------------------

class MainContentComponent   : public AudioAppComponent,
                               public ChangeListener
{
public:
    
    MainContentComponent()
      : state (Stopped),
        thumbnailCache (5),
        thumbnailComp (512, formatManager, thumbnailCache),
        positionOverlay (transportSource)
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

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        if (readerSource.get() == nullptr)
            bufferToFill.clearActiveBufferRegion();
        else
            transportSource.getNextAudioBlock (bufferToFill);
    }

    void releaseResources() override
    {
        transportSource.releaseResources();
    }

    void resized() override
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

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &transportSource)
            transportSourceChanged();
    }
    
    //linking method between FrontEnd object and positionOverlay object
    //that passes along the call the add a new marker at the position of the
    //playhead currently. The markers are then sorted so that if the markers 
    //are not necessarily clicked from left to right, they will still be in 
    //the correct order for graphical rendering later
    void addMarkerAtCurrentPosition(){
        positionOverlay.addMarkerAtCurrentPosition();
        positionOverlay.sortMarkers();
    }
    
    int getSelectedSliceIndex(){
        return positionOverlay.getSelectedSliceIndex();
    }

private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    void changeState (TransportState newState)
    {
        if (state != newState)
        {
            state = newState;

            switch (state)
            {
                case Stopped:
                    stopButton.setEnabled (false);
                    playButton.setEnabled (true);
                    transportSource.setPosition (0.0);
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
                    break;

                default:
                    jassertfalse;
                    break;
            }
        }
    }

    void transportSourceChanged()
    {
        if (transportSource.isPlaying())
            changeState (Playing);
        else
            changeState (Stopped);
    }
    
	// openfile
    void openButtonClicked()
    {
        FileChooser chooser ("Select an audio file to sample...",
                             File::nonexistent,
                             "*.wav;*.mp3");

        if (chooser.browseForFileToOpen())
        {
            File file = chooser.getResult();

            if (auto* reader = formatManager.createReaderFor (file))
            {
                std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
                transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                positionOverlay.setTransportSourceLengthForFinalMarker(transportSource.getLengthInSeconds());
                playButton.setEnabled (true);
                thumbnailComp.setFile (file);
                readerSource.reset (newSource.release());
            }
        }
    }

    void playButtonClicked()
    {
        changeState (Starting);
    }

    void stopButtonClicked()
    {
        changeState (Stopping);
    }

	//void shouldDropFilesWhenDraggedExternally(File::nonexistent,"*.wav"){}

    //==========================================================================
    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;

    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState state;
    AudioThumbnailCache thumbnailCache;
    SimpleThumbnailComponent thumbnailComp;
    SimplePositionOverlay positionOverlay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
