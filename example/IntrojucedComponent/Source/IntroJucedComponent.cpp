/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.2.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
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

    addAndMakeVisible (label = new Label ("new label",
                                          TRANS("Hello, jucers!")));
    label->setFont (Font (15.00f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    String xml = String::fromUTF8 (BinaryData::IntroJucedComponent_xml, BinaryData::IntroJucedComponent_xmlSize);
    layout = new Layout (xml, this);

    // just fancy stuff to demonstrate LayoutListener
    Component* splitterComponent = findChildWithID ("splitter");
    if (splitterComponent) {
        juce::ValueTree node = layout->getLayoutItem (splitterComponent);
        if (node.isValid()) {
            LayoutSplitter item (node);
            item.addListener (this);
        }
    }
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
    label = nullptr;


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
    /*
    //[/UserPreResize]

    textButton1->setBounds (0, 24, 150, 24);
    textButton2->setBounds (0, 56, 150, 24);
    textButton3->setBounds (16, 88, 150, 24);
    textButton4->setBounds (16, 175, 150, 24);
    textButton5->setBounds (24, 216, 150, 24);
    slider1->setBounds (208, 24, 150, 128);
    slider2->setBounds (360, 24, 150, 128);
    slider3->setBounds (512, 24, 150, 128);
    label->setBounds (336, 192, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    */
    if (layout) layout->updateGeometry();
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

void IntroJucedComponent::layoutBoundsChanged (juce::ValueTree node, juce::Rectangle<int> newBounds)
{
}

void IntroJucedComponent::layoutSplitterMoved (juce::ValueTree node, float pos, bool final)
{
    label->setText(String (pos), dontSendNotification);
    label->setColour (Label::textColourId, final ? Colours::black : Colours::red);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="IntroJucedComponent" componentName=""
                 parentClasses="public Component, public LayoutItemListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
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
  <LABEL name="new label" id="76934d4e7fa631f2" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="336 192 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Hello, jucers!" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
