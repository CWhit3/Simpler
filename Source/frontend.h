/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.3.2

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "Tag.h"
#include "AudioThumbnailTutorial_04.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
 An auto-generated component, created by the Projucer.

 Describe your class and how it works here!
                                                                    //[/Comments]
*/
class FrontEnd  : public Component,
                  public Button::Listener
{
public:
    //==============================================================================
    FrontEnd ();
    ~FrontEnd();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void reorganizeTags();
    void deleteTag(Tag* t);
    void renameTag(Tag* t);
    void resizeTags();
    void updateAutogenLabel();

    Tag* tagToDelete;
    Tag* tagToRename;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent& e) override;

    // Binary resources:
    static const char* plus_button_up_png;
    static const int plus_button_up_pngSize;
    static const char* plus_button_down_png;
    static const int plus_button_down_pngSize;
    static const char* minus_button_up_png;
    static const int minus_button_up_pngSize;
    static const char* minus_button_down_png;
    static const int minus_button_down_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    double fittedTagWidth;
    MainContentComponent* waveformEditor;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<ImageButton> addTagButton;
    std::unique_ptr<Label> label;
    std::unique_ptr<Label> saveSectionLabel;
    std::unique_ptr<Label> waveformEditorLabel;
    std::unique_ptr<ImageButton> addMarkerButton;
    std::unique_ptr<TextButton> saveAllSlicesButton;
    std::unique_ptr<TextButton> manualSaveButton;
    std::unique_ptr<Label> currentSliceAutogenTitle;
    std::unique_ptr<TextEditor> tagRenameEditor;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrontEnd)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
