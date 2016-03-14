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

#ifndef __JUCE_HEADER_B295FC5F7B1BCC7C__
#define __JUCE_HEADER_B295FC5F7B1BCC7C__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class IntroJucedComponent  : public Component,
                             public ButtonListener,
                             public SliderListener,
                             public LayoutItemListener
{
public:
    //==============================================================================
    IntroJucedComponent ();
    ~IntroJucedComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    // The line below can be used to visualize the computed bounds for debugging
    //void paintOverChildren (Graphics& g) override { if (layout) layout->paintBounds (g); }

    void layoutBoundsChanged (juce::Rectangle<int> newBounds) override;
    void layoutSplitterMoved (float pos, bool final) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    ScopedPointer<Layout> layout;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<TextButton> textButton1;
    ScopedPointer<TextButton> textButton2;
    ScopedPointer<TextButton> textButton3;
    ScopedPointer<TextButton> textButton4;
    ScopedPointer<TextButton> textButton5;
    ScopedPointer<Slider> slider1;
    ScopedPointer<Slider> slider2;
    ScopedPointer<Slider> slider3;
    ScopedPointer<Label> label;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IntroJucedComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_B295FC5F7B1BCC7C__
