//
//  AudioThumbnailTutorial_04.hpp
//  Simpler
//
//  Created by Christian White on 11/26/18.
//
//

#ifndef WaveformEditor_hpp
#define WaveformEditor_hpp

#pragma once


#include <stdio.h>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "Tag.h"

#endif /* AudioThumbnailTutorial_04_hpp */


//ONLY FOR USE DURING LIVE DEMO AND WILL BE DELETED AFTER. THIS IS A CLASS USED FOR HARDCODING TAG SWAPPING
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class DummyTagSwitcher    : public Component

{
public:
    DummyTagSwitcher();
   ~DummyTagSwitcher();
    void paint (Graphics&) override;
    void resized() override;
    void mouseDoubleClick(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;

private:
    int lastAccessedSetIndex;
    std::vector<double> spoofedTagSwapSpots;
    std::vector<std::vector<Tag*>> spoofedSliceTagSets;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyTagSwitcher)
};
//END HARDCODED CLASS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//draws the waveform from the audio file selected by the user or indicates no file has been loaded
//if one hasn't been selected yet
class SimpleThumbnailComponent: public Component,
                                public ChangeListener
{
public:
    SimpleThumbnailComponent (int sourceSamplesPerThumbnailSample,
                          AudioFormatManager& formatManager,
                            AudioThumbnailCache& cache);
                            
    //PUBLIC METHODS
    void setFile (const File& file);
    void paint (Graphics& g) override;
    void paintIfNoFileLoaded (Graphics& g);
    void paintIfFileLoaded (Graphics& g);
    void changeListenerCallback (ChangeBroadcaster* source) override;    
    
private:
    //PRIVATE METHODS
    void thumbnailChanged();
    
    //PRIVATE VARIABLES
    AudioThumbnail thumbnail;
    //PASSED REFERENCE TO FAKE TAG SWITCHER FOR LIVE DEMO, WILL BE DELETED FOR ACTUAL IMPLEMENTATION AFTER DEMO
    DummyTagSwitcher tagSwitcher;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//draws playhead, markers, and orange selected slice indicator overlay
class SimplePositionOverlay : public Component,
                              private Timer
{
public:
    SimplePositionOverlay (AudioTransportSource& transportSourceToUse, DummyTagSwitcher& tagSwitcherToUse);
    
    //PUBLIC METHODS
    void paint (Graphics& g) override;
    void mouseDrag (const MouseEvent& e) override;
    void addMarkerAtCurrentPosition();
    void sortMarkers();   
    void mouseDoubleClick(const MouseEvent& e)override ; 
    void setTransportSourceLengthForFinalMarker(String fileName, double lengthOfSampleInSeconds); 
    int getSelectedSliceIndex(); 
    double getPositionFromCurrentSliceIndex();
    
private:
    //PRIVATE METHODS
    void timerCallback() override;
    
    //PRIVATE VARIABLES
    
    AudioTransportSource& transportSource;
    //FAKE TAG SWITCHER FOR LIVE DEMO, WILL BE DELETED FOR ACTUAL IMPLEMENTATION AFTER DEMO
    DummyTagSwitcher& tagSwitcher; 
    //set of all double values representing the time (in seconds)
    //at which a marker was created by the user
    std::vector<double> waveformMarkers;
    //index used for graphical drawing of the current selected slice of the sample
    //which is updated in the overriden mouseDoubleClick function
    int indexOfLeftMarkerOfSlice;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class MainContentComponent   : public AudioAppComponent,
                               public ChangeListener
{
public:
    MainContentComponent();
   ~MainContentComponent();
    
    //PUBLIC METHODS
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void resized() override;
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void mouseDoubleClick(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void addMarkerAtCurrentPosition();
    int getSelectedSliceIndex();
    
private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };
    
    //PRIVATE METHODS
    void changeState (TransportState newState);
    void transportSourceChanged();
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    
    //PRIVATE VARIABLES
    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;
    Label      songName;
    
    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState state;
    AudioThumbnailCache thumbnailCache;
    SimpleThumbnailComponent thumbnailComp;
    SimplePositionOverlay positionOverlay;
    DummyTagSwitcher tagSwitcher;
};
