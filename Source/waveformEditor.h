/*
  ==============================================================================

    waveformEditor.h
    Created: 22 Nov 2018 12:25:16pm
    Author:  Christian White

  ==============================================================================
*/

#pragma once
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

 name:             waveformEditor
 version:          1.0.0
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

 useLocalCopy:     0

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

//==============================================================================
class waveformEditor   : public AudioAppComponent,
                               private ChangeListener
{
public:
    waveformEditor()
       : state (Stopped),
         thumbnailCache (5),                            // [4]
         thumbnail (512, formatManager, thumbnailCache) // [5]
    {
        addAndMakeVisible (&openButton);
        openButton.setButtonText ("Open...");
        openButton.setBounds(0, 0, 20, 10);
        openButton.onClick = [this] { openButtonClicked(); };

        addAndMakeVisible (&playButton);
        playButton.setButtonText ("Play");
        playButton.setBounds(25, 0, 20, 10);
        playButton.onClick = [this] { playButtonClicked(); };
        playButton.setColour (TextButton::buttonColourId, Colours::green);
        playButton.setEnabled (false);

        addAndMakeVisible (&stopButton);
        stopButton.setButtonText ("Stop");
        stopButton.setBounds(50, 0, 20, 10);
        stopButton.onClick = [this] { stopButtonClicked(); };
        stopButton.setColour (TextButton::buttonColourId, Colours::red);
        stopButton.setEnabled (false);

        setSize (584, 80);

        formatManager.registerBasicFormats();
        transportSource.addChangeListener (this);
        thumbnail.addChangeListener (this);            // [6]

        setAudioChannels (2, 2);
    }

    ~waveformEditor()
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

    void paint (Graphics& g) override
    {
        Rectangle<int> thumbnailBounds (10, 15, getWidth() - 20, getHeight() - 120);

        if (thumbnail.getNumChannels() == 0)
            paintIfNoFileLoaded (g, thumbnailBounds);
        else
            paintIfFileLoaded (g, thumbnailBounds);
    }

    void resized() override
    {
        
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &transportSource) transportSourceChanged();
        if (source == &thumbnail)       thumbnailChanged();
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
        changeState (transportSource.isPlaying() ? Playing : Stopped);
    }

    void thumbnailChanged()
    {
        repaint();
    }

    void paintIfNoFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
    {
        g.setColour (Colours::white);
        g.fillRect (thumbnailBounds);
        g.setColour (Colour(0xffcdcdcd);
        g.drawFittedText ("No File Loaded", thumbnailBounds, Justification::centred, 1.0f);
    }

    void paintIfFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
    {
        g.setColour (Colours::white);
        g.fillRect (thumbnailBounds);

        g.setColour (Colour(0xffb4a7d6));                                     // [8]

        thumbnail.drawChannels (g,                                      // [9]
                                thumbnailBounds,
                                0.0,                                    // start time
                                thumbnail.getTotalLength(),             // end time
                                1.0f);                                  // vertical zoom
    }

    void openButtonClicked()
    {
        FileChooser chooser ("Select a file to play...",
                             {},
                             "*.mp3");

        if (chooser.browseForFileToOpen())
        {
            File file (chooser.getResult());
            auto* reader = formatManager.createReaderFor(file);

            if (reader != nullptr)
            {
                std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
                transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                playButton.setEnabled (true);
                thumbnail.setSource (new FileInputSource (file));          // [7]
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

    //==========================================================================
    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;

    AudioFormatManager formatManager;                    // [3]
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState state;
    AudioThumbnailCache thumbnailCache;                  // [1]
    AudioThumbnail thumbnail;                            // [2]

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
