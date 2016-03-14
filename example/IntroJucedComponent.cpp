/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 4.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "IntroJucedComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
IntroJucedComponent::IntroJucedComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (textButton1 = new TextButton ("textButton1"));
    textButton1->setButtonText (TRANS("Do Something 1"));
    textButton1->addListener (this);

    addAndMakeVisible (textButton2 = new TextButton ("textButton2"));
    textButton2->setButtonText (TRANS("Do Something 2"));
    textButton2->addListener (this);

    addAndMakeVisible (textButton3 = new TextButton ("textButton3"));
    textButton3->setButtonText (TRANS("Do Something 3"));
    textButton3->addListener (this);

    addAndMakeVisible (textButton4 = new TextButton ("textButton4"));
    textButton4->setButtonText (TRANS("Do Something 4"));
    textButton4->addListener (this);

    addAndMakeVisible (textButton5 = new TextButton ("textButton5"));
    textButton5->setButtonText (TRANS("Do Something 5"));
    textButton5->addListener (this);

    addAndMakeVisible (slider1 = new Slider ("slider1"));
    slider1->setRange (0, 10, 0);
    slider1->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider1->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    slider1->addListener (this);

    addAndMakeVisible (slider2 = new Slider ("slider2"));
    slider2->setRange (0, 10, 0);
    slider2->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider2->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    slider2->addListener (this);

    addAndMakeVisible (slider3 = new Slider ("slider3"));
    slider3->setRange (0, 10, 0);
    slider3->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider3->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    slider3->addListener (this);


    //[UserPreSize]
    String xml = String::fromUTF8 (BinaryData::IntroJucedComponent_xml, BinaryData::IntroJucedComponent_xmlSize);
    layout = new Layout (xml, this);
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

IntroJucedComponent::~IntroJucedComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    textButton1 = nullptr;
    textButton2 = nullptr;
    textButton3 = nullptr;
    textButton4 = nullptr;
    textButton5 = nullptr;
    slider1 = nullptr;
    slider2 = nullptr;
    slider3 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void IntroJucedComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void IntroJucedComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    textButton1->setBounds (0, 24, 150, 24);
    textButton2->setBounds (0, 56, 150, 24);
    textButton3->setBounds (16, 88, 150, 24);
    textButton4->setBounds (16, 175, 150, 24);
    textButton5->setBounds (24, 216, 150, 24);
    slider1->setBounds (208, 24, 150, 128);
    slider2->setBounds (360, 24, 150, 128);
    slider3->setBounds (512, 24, 150, 128);
    //[UserResized] Add your own custom resize handling here..
    layout->updateGeometry();
    //[/UserResized]
}

void IntroJucedComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == textButton1)
    {
        //[UserButtonCode_textButton1] -- add your button handler code here..
        //[/UserButtonCode_textButton1]
    }
    else if (buttonThatWasClicked == textButton2)
    {
        //[UserButtonCode_textButton2] -- add your button handler code here..
        //[/UserButtonCode_textButton2]
    }
    else if (buttonThatWasClicked == textButton3)
    {
        //[UserButtonCode_textButton3] -- add your button handler code here..
        //[/UserButtonCode_textButton3]
    }
    else if (buttonThatWasClicked == textButton4)
    {
        //[UserButtonCode_textButton4] -- add your button handler code here..
        //[/UserButtonCode_textButton4]
    }
    else if (buttonThatWasClicked == textButton5)
    {
        //[UserButtonCode_textButton5] -- add your button handler code here..
        //[/UserButtonCode_textButton5]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void IntroJucedComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == slider1)
    {
        //[UserSliderCode_slider1] -- add your slider handling code here..
        //[/UserSliderCode_slider1]
    }
    else if (sliderThatWasMoved == slider2)
    {
        //[UserSliderCode_slider2] -- add your slider handling code here..
        //[/UserSliderCode_slider2]
    }
    else if (sliderThatWasMoved == slider3)
    {
        //[UserSliderCode_slider3] -- add your slider handling code here..
        //[/UserSliderCode_slider3]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="IntroJucedComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <TEXTBUTTON name="textButton1" id="e88308a3559be3f5" memberName="textButton1"
              virtualName="" explicitFocusOrder="0" pos="0 24 150 24" buttonText="Do Something 1"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="textButton2" id="4c02cbac1dd2ef10" memberName="textButton2"
              virtualName="" explicitFocusOrder="0" pos="0 56 150 24" buttonText="Do Something 2"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="textButton3" id="b96a0beb28cc5a63" memberName="textButton3"
              virtualName="" explicitFocusOrder="0" pos="16 88 150 24" buttonText="Do Something 3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="textButton4" id="4388e5b3be85fb3d" memberName="textButton4"
              virtualName="" explicitFocusOrder="0" pos="16 175 150 24" buttonText="Do Something 4"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="textButton5" id="172dc61b05590c40" memberName="textButton5"
              virtualName="" explicitFocusOrder="0" pos="24 216 150 24" buttonText="Do Something 5"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <SLIDER name="slider1" id="ebe91de3fee0858b" memberName="slider1" virtualName=""
          explicitFocusOrder="0" pos="208 24 150 128" min="0" max="10"
          int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="1"/>
  <SLIDER name="slider2" id="eccbe10ef791d472" memberName="slider2" virtualName=""
          explicitFocusOrder="0" pos="360 24 150 128" min="0" max="10"
          int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="1"/>
  <SLIDER name="slider3" id="adfe52a6c03512f9" memberName="slider3" virtualName=""
          explicitFocusOrder="0" pos="512 24 150 128" min="0" max="10"
          int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: introJucedComponent_xml, 1688, "IntroJucedComponent.xml"
static const unsigned char resource_IntroJucedComponent_introJucedComponent_xml[] = { 60,63,120,109,108,32,118,101,114,115,105,111,110,61,34,49,46,48,34,32,101,110,99,111,100,105,110,103,61,34,85,84,70,
45,56,34,63,62,10,10,60,76,97,121,111,117,116,32,111,114,105,101,110,116,97,116,105,111,110,61,34,108,101,102,116,84,111,82,105,103,104,116,34,32,115,116,114,101,116,99,104,88,61,34,45,49,34,32,115,116,
114,101,116,99,104,89,61,34,45,49,34,62,10,32,32,60,76,97,121,111,117,116,32,111,114,105,101,110,116,97,116,105,111,110,61,34,116,111,112,68,111,119,110,34,32,115,116,114,101,116,99,104,88,61,34,45,49,
34,32,115,116,114,101,116,99,104,89,61,34,45,49,34,62,10,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,99,111,109,112,111,110,101,110,116,78,97,109,101,61,34,116,101,120,116,66,117,116,116,111,
110,49,34,47,62,10,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,99,111,109,112,111,110,101,110,116,78,97,109,101,61,34,116,101,120,116,66,117,116,116,111,110,50,34,47,62,10,32,32,32,32,60,67,111,
109,112,111,110,101,110,116,32,99,111,109,112,111,110,101,110,116,78,97,109,101,61,34,116,101,120,116,66,117,116,116,111,110,51,34,47,62,10,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,99,111,
109,112,111,110,101,110,116,78,97,109,101,61,34,116,101,120,116,66,117,116,116,111,110,52,34,47,62,10,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,99,111,109,112,111,110,101,110,116,78,97,109,
101,61,34,116,101,120,116,66,117,116,116,111,110,53,34,47,62,10,32,32,60,47,76,97,121,111,117,116,62,10,32,32,60,83,112,108,105,116,116,101,114,32,105,115,72,111,114,105,122,111,110,116,97,108,61,34,49,
34,32,109,105,110,87,105,100,116,104,61,34,51,34,32,109,97,120,87,105,100,116,104,61,34,51,34,32,114,101,108,97,116,105,118,101,77,97,120,80,111,115,105,116,105,111,110,61,34,48,46,55,34,32,114,101,108,
97,116,105,118,101,80,111,115,105,116,105,111,110,61,34,48,46,51,34,47,62,10,32,32,60,76,97,121,111,117,116,32,111,114,105,101,110,116,97,116,105,111,110,61,34,116,111,112,68,111,119,110,34,32,115,116,
114,101,116,99,104,88,61,34,45,49,34,32,115,116,114,101,116,99,104,89,61,34,45,49,34,62,10,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,108,97,98,101,108,84,101,120,116,61,34,83,111,109,101,32,
67,111,110,116,114,111,108,115,34,32,109,105,110,72,101,105,103,104,116,61,34,50,53,34,32,109,97,120,72,101,105,103,104,116,61,34,50,53,34,32,108,97,98,101,108,70,111,110,116,83,105,122,101,61,34,50,53,
34,47,62,10,32,32,32,32,60,76,97,121,111,117,116,32,111,114,105,101,110,116,97,116,105,111,110,61,34,108,101,102,116,84,111,82,105,103,104,116,34,32,115,116,114,101,116,99,104,88,61,34,45,49,34,32,115,
116,114,101,116,99,104,89,61,34,45,49,34,62,10,32,32,32,32,32,32,60,76,97,121,111,117,116,32,111,114,105,101,110,116,97,116,105,111,110,61,34,116,111,112,68,111,119,110,34,32,115,116,114,101,116,99,104,
88,61,34,45,49,34,32,115,116,114,101,116,99,104,89,61,34,45,49,34,62,10,32,32,32,32,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,99,111,109,112,111,110,101,110,116,78,97,109,101,61,34,115,108,
105,100,101,114,49,34,32,97,115,112,101,99,116,82,97,116,105,111,61,34,49,46,48,34,32,112,97,100,100,105,110,103,76,101,102,116,61,34,53,34,32,112,97,100,100,105,110,103,82,105,103,104,116,61,34,53,34,
47,62,10,32,32,32,32,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,108,97,98,101,108,84,101,120,116,61,34,77,121,83,108,105,100,101,114,32,49,34,32,109,105,110,72,101,105,103,104,116,61,34,49,56,
34,32,109,97,120,72,101,105,103,104,116,61,34,49,56,34,47,62,10,32,32,32,32,32,32,32,32,60,83,112,97,99,101,114,32,115,116,114,101,116,99,104,88,61,34,49,34,32,115,116,114,101,116,99,104,89,61,34,49,34,
47,62,10,32,32,32,32,32,32,60,47,76,97,121,111,117,116,62,10,32,32,32,32,32,32,60,76,97,121,111,117,116,32,111,114,105,101,110,116,97,116,105,111,110,61,34,116,111,112,68,111,119,110,34,32,115,116,114,
101,116,99,104,88,61,34,45,49,34,32,115,116,114,101,116,99,104,89,61,34,45,49,34,62,10,32,32,32,32,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,99,111,109,112,111,110,101,110,116,78,97,109,101,
61,34,115,108,105,100,101,114,50,34,32,97,115,112,101,99,116,82,97,116,105,111,61,34,49,46,48,34,32,112,97,100,100,105,110,103,76,101,102,116,61,34,53,34,32,112,97,100,100,105,110,103,82,105,103,104,116,
61,34,53,34,47,62,10,32,32,32,32,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,108,97,98,101,108,84,101,120,116,61,34,77,121,83,108,105,100,101,114,32,50,34,32,109,105,110,72,101,105,103,104,116,
61,34,49,56,34,32,109,97,120,72,101,105,103,104,116,61,34,49,56,34,47,62,10,32,32,32,32,32,32,32,32,60,83,112,97,99,101,114,32,115,116,114,101,116,99,104,88,61,34,49,34,32,115,116,114,101,116,99,104,89,
61,34,49,34,47,62,10,32,32,32,32,32,32,60,47,76,97,121,111,117,116,62,10,32,32,32,32,32,32,60,76,97,121,111,117,116,32,111,114,105,101,110,116,97,116,105,111,110,61,34,116,111,112,68,111,119,110,34,32,
115,116,114,101,116,99,104,88,61,34,45,49,34,32,115,116,114,101,116,99,104,89,61,34,45,49,34,62,10,32,32,32,32,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,99,111,109,112,111,110,101,110,116,78,
97,109,101,61,34,115,108,105,100,101,114,51,34,32,97,115,112,101,99,116,82,97,116,105,111,61,34,49,46,48,34,32,112,97,100,100,105,110,103,76,101,102,116,61,34,53,34,32,112,97,100,100,105,110,103,82,105,
103,104,116,61,34,53,34,47,62,10,32,32,32,32,32,32,32,32,60,67,111,109,112,111,110,101,110,116,32,108,97,98,101,108,84,101,120,116,61,34,77,121,83,108,105,100,101,114,32,51,34,32,109,105,110,72,101,105,
103,104,116,61,34,49,56,34,32,109,97,120,72,101,105,103,104,116,61,34,49,56,34,47,62,10,32,32,32,32,32,32,32,32,60,83,112,97,99,101,114,32,115,116,114,101,116,99,104,88,61,34,49,34,32,115,116,114,101,
116,99,104,89,61,34,49,34,47,62,10,32,32,32,32,32,32,60,47,76,97,121,111,117,116,62,10,32,32,32,32,60,47,76,97,121,111,117,116,62,10,32,32,32,32,60,83,112,97,99,101,114,32,115,116,114,101,116,99,104,88,
61,34,49,34,32,115,116,114,101,116,99,104,89,61,34,50,34,47,62,10,32,32,60,47,76,97,121,111,117,116,62,10,60,47,76,97,121,111,117,116,62,10,0,0};

const char* IntroJucedComponent::introJucedComponent_xml = (const char*) resource_IntroJucedComponent_introJucedComponent_xml;
const int IntroJucedComponent::introJucedComponent_xmlSize = 1688;


//[EndFile] You can add extra defines here...
//[/EndFile]
