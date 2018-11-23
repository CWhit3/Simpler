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

//[Headers] You can add your own extra header files here...
#include <list>
#include <string>
#include <iostream>
#include "frontend.h"
//[/Headers]

#include "Tag.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Tag::Tag ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    buttonText = "click to rename";
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (104, 24);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Tag::~Tag()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void Tag::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    
    //DRAW LILAC BUTTON BACKGROUND
    {
        float x = 0.0f, y = 0.0f, width = static_cast<float> (proportionOfWidth (1.0000f)), height = static_cast<float> (proportionOfHeight (1.0000f));
        Colour fillColour = Colour (0xffb4a7d6);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 4.500f);
    }
    
    //DRAW TEXT ON BUTTON IN PLUGIN FONT
    {
    int x = proportionOfWidth (0.0000f), y = proportionOfHeight (0.0000f), width = proportionOfWidth (1.0000f), height = proportionOfHeight (1.0000f);
        Colour fillColour = Colours::darkgrey;
    g.setColour (fillColour);
    g.setFont (Font ("Avenir Next", 20.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.drawText (buttonText, x, y, width, height,
                Justification::centred, true);
    }
    //[/UserPaint]
}

void Tag::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Tag::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    //[/UserCode_mouseEnter]
}

void Tag::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    //[/UserCode_mouseExit]
}

void Tag::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    FrontEnd* parent = (FrontEnd*)getParentComponent();
    parent->tagToRename = this;
    parent->reorganizeTags();
    //[/UserCode_mouseDown]
}

void Tag::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    //[/UserCode_mouseDrag]
}

void Tag::mouseDoubleClick (const MouseEvent& e)
{
    //[UserCode_mouseDoubleClick] -- Add your code here...
    FrontEnd* parent = (FrontEnd*)getParentComponent();
    parent->tagToDelete = this;
    parent->reorganizeTags();
    delete this;
    //[/UserCode_mouseDoubleClick]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//Custom method for setting text of button to be drawn over its background
//Note that this method is distinct from setButtonText(), which is not use due to
//the inability to set the font of the text drawn without overriding the method
//PARAMETER: the string to set this tag's text to
void Tag::setText (const String& newText){
    buttonText = newText;
}

String Tag::getText(){
    return buttonText;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Tag" componentName="" parentClasses="public TextButton"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="104"
                 initialHeight="24">
  <METHODS>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDoubleClick (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="323e44"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
