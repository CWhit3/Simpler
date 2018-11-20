/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "frontend.h"

//==============================================================================
SimplerAudioProcessorEditor::SimplerAudioProcessorEditor (SimplerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    FrontEnd* f = new FrontEnd();
    f->addMouseListener(this, true);
    addAndMakeVisible(f);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
}

SimplerAudioProcessorEditor::~SimplerAudioProcessorEditor()
{
    
}

//==============================================================================
void SimplerAudioProcessorEditor::paint (Graphics& g)
{

}

void SimplerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
