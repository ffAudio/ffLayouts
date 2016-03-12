/*
  ==============================================================================

Copyright (c) 2016, Daniel Walz
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this 
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software without 
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.
    
  ==============================================================================

    Created: Feb, 2016

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent() : layout (Layout::LeftToRight, this)
{
    // Juce resizer to demo updates
    resizeConstraints = new ComponentBoundsConstrainer();
    resizeConstraints->setMinimumSize(200, 150);
    resizer = new ResizableCornerComponent (this, resizeConstraints);


    // minimal example
    Layout* minimal = layout.addSubLayout (Layout::TopDown);
    gain.setSliderStyle  (Slider::RotaryHorizontalVerticalDrag);
    phase.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    pan.setSliderStyle   (Slider::RotaryHorizontalVerticalDrag);
    gain.setTextBoxStyle  (Slider::TextBoxBelow, true, 40, 18);
    phase.setTextBoxStyle (Slider::TextBoxBelow, true, 40, 18);
    pan.setTextBoxStyle   (Slider::TextBoxBelow, true, 40, 18);
    gain.setComponentID ("myGainSlider");
    phase.setComponentID ("myPhaseSlider");
    pan.setComponentID ("myPanSlider");
    addAndMakeVisible (gain);
    addAndMakeVisible (phase);
    addAndMakeVisible (pan);
    minimal->addLabeledComponent (&gain,  "Gain");      // <- this is the only line necessary for each component
    minimal->addLabeledComponent (&phase, "Phase");
    minimal->addLabeledComponent (&pan,   "Pan");

    // other use cases - just tested as many edge cases I could think of...
    // add some sliders in a column
    Layout* column = layout.addSubLayout (Layout::TopDown);
    for (int i=0; i<5; ++i) {
        Slider* slider = new Slider (Slider::LinearHorizontal, Slider::TextBoxBelow);
        components.add (slider);
        column->addComponent (slider);
        addAndMakeVisible (slider);
    }
    
    Layout* row = column->addSubLayout(Layout::LeftToRight);
    TextButton* b1 = new TextButton ("B1");
    components.add (b1);
    row->addComponent (b1)->setItemParameters(0.5, 1.0, 50, 30, 100, 60, 3, 0, 3, 3, 1.5);
    addAndMakeVisible (b1);
    TextButton* b2 = new TextButton ("B2");
    components.add (b2);
    row->addComponent (b2);
    addAndMakeVisible (b2);
    TextButton* b3 = new TextButton ("B3");
    components.add (b3);
    addAndMakeVisible (b3);
    LayoutItem* rowItem = row->addComponent (b3);
    rowItem->setMinimumWidth (80);
    rowItem->setMaximumWidth (120);

    // you can also access the component from the item
    LayoutItem* item = layout.addComponent (new Label ("some text", "Some Text"));
    components.add (item->getComponent());
    addAndMakeVisible (item->getComponent());

    layout.addSplitterItem (0.5)->setMinimumRelativePosition (0.2);
    
    // in another column we use a spacer
    Layout* column3 = layout.addSubLayout (Layout::BottomUp);
    for (int i=0; i<5; ++i) {
        TextButton* button = new TextButton ("Button " + String (i+1));
        components.add (button);
        column3->addComponent (button);
        addAndMakeVisible (button);
    }
    TextButton* button = new TextButton ("Constrained");
    components.add (button);
    LayoutItem* constrained = column3->addComponent (button);
    constrained->setMinimumHeight (100);
    constrained->setMaximumHeight (110);
    addAndMakeVisible (button);

    column3->addSplitterItem (0.5, 3)->setMinimumRelativePosition (0.2);

    column3->addSpacer();

    // my favourite example, three labeled knobs in a column
    Layout* column4 = layout.addSubLayout (Layout::TopDown);
    if (LayoutItem* c4 = dynamic_cast<LayoutItem*>(column4)) {
        c4->setPadding (10);
    }
    for (int i=0; i<3; ++i) {
        Slider* slider = new Slider (Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox);
        components.add (slider);
        column4->addLabeledComponent (slider, "Knob " + String(i+1), Layout::BottomUp);
        if (i<2) {
            column4->addSpacer (1.0, 0.2);
        }
        addAndMakeVisible (slider);
    }
    
    ValueTree myLayoutTree;
    layout.saveLayoutToValueTree (myLayoutTree);
    
    String layoutXML = myLayoutTree.toXmlString();
    
    FileOutputStream out (File::getCurrentWorkingDirectory().getChildFile ("LayoutTest.jui"));
    if (out.openedOk()) {
        out.setPosition (0);
        out.truncate();
        out.write (layoutXML.toRawUTF8(), layoutXML.getNumBytesAsUTF8());
    }

    // juce again
    setSize (600, 400);
    addAndMakeVisible (resizer);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
}

void MainContentComponent::resized()
{
    resizer->setBounds (getRight()-16, getBottom()-16, 16, 16);
    
    layout.updateGeometry (getLocalBounds());
}
