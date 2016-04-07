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
MainContentComponent::MainContentComponent() : layout (LayoutItem::LeftToRight, this)
{
    // Juce resizer to demo updates
    resizeConstraints = new ComponentBoundsConstrainer();
    resizeConstraints->setMinimumSize(200, 150);
    resizer = new ResizableCornerComponent (this, resizeConstraints);

    // minimal example
    LayoutItem minimal = LayoutItem::makeSubLayout (layout.state, LayoutItem::TopDown);
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
    
    LayoutItem::makeLabeledChildComponent (minimal.state, &gain,  "Gain");   // <- this is the only line necessary for each component
    LayoutItem::makeLabeledChildComponent (minimal.state, &phase, "Phase");
    LayoutItem::makeLabeledChildComponent (minimal.state, &pan,   "Pan");
    
    // other use cases - just tested as many edge cases I could think of...
    // add some sliders in a column
    LayoutItem column = LayoutItem::makeSubLayout (layout.state, LayoutItem::TopDown);
    for (int i=0; i<5; ++i) {
        Slider* slider = new Slider (Slider::LinearHorizontal, Slider::TextBoxBelow);
        components.add (slider);
        LayoutItem::makeChildComponent (column.state, slider);
        addAndMakeVisible (slider);
    }
    
    LayoutItem row = LayoutItem::makeSubLayout (column.state, LayoutItem::LeftToRight);
    TextButton* b1 = new TextButton ("B1");
    b1->setComponentID ("B1");
    components.add (b1);
    LayoutItem::makeChildComponent (row.state, b1).setItemParameters(0.5, 1.0, 50, 30, 100, 60, 3, 0, 3, 3, 1.5);
    addAndMakeVisible (b1);
    TextButton* b2 = new TextButton ("B2");
    components.add (b2);
    LayoutItem::makeChildComponent (row.state, b2);
    addAndMakeVisible (b2);
    TextButton* b3 = new TextButton ("B3");
    components.add (b3);
    addAndMakeVisible (b3);
    LayoutItem rowItem = LayoutItem::makeChildComponent (row.state, b3);
    rowItem.setMinimumWidth (80);
    rowItem.setMaximumWidth (120);

    LayoutSplitter splitter = LayoutItem::makeChildSplitter (layout.state, 0.5f);
    splitter.setMinimumRelativePosition (0.2f);
    
    // in another column we use a spacer
    LayoutItem column3 = LayoutItem::makeSubLayout (layout.state, LayoutItem::BottomUp);
    for (int i=0; i<5; ++i) {
        TextButton* button = new TextButton ("Button " + String (i+1));
        components.add (button);
        LayoutItem::makeChildComponent (column3.state, button);
        addAndMakeVisible (button);
    }
    
   
    // my favourite example, three labeled knobs in a column
    LayoutItem column4 = LayoutItem::makeSubLayout (layout.state, LayoutItem::TopDown);
    for (int i=0; i<3; ++i) {
        Slider* slider = new Slider (Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox);
        components.add (slider);
        
        LayoutItem::makeLabeledChildComponent (column4.state, slider, "Knob " + String(i+1), LayoutItem::BottomUp);
        if (i<2) {
            LayoutItem::makeChildSpacer (column4.state, 1.0, 0.2);
        }
        addAndMakeVisible (slider);
    }

    for (int i=0; i<components.size(); ++i) {
        Component* c = components.getUnchecked (i);
        if (c->getComponentID().isEmpty()) {
            c->setComponentID("RandomName " + String (i));
        }
    }
    
    // Saving and reloading layout test
    /*
    ValueTree myLayoutTree;
    layout.fixUpLayoutItems();
    layout.saveLayoutToValueTree (myLayoutTree);

    FileOutputStream out (File::getCurrentWorkingDirectory().getChildFile ("LayoutTest.jui"));
    if (out.openedOk()) {
        String layoutXML = myLayoutTree.toXmlString();
        out.setPosition (0);
        out.truncate();
        out.write (layoutXML.toRawUTF8(), layoutXML.getNumBytesAsUTF8());
        out.flush();
    }
    
    // Houdini trick:
    layout.clearLayout();

    ScopedPointer<XmlElement> doc = XmlDocument::parse (File::getCurrentWorkingDirectory().getChildFile ("LayoutTest.jui"));
    ValueTree myLoadedTree = ValueTree::fromXml (*doc);
    
    layout.loadLayoutFromValueTree (myLoadedTree, this);
    
    // for debug diff
    FileOutputStream outDiff (File::getCurrentWorkingDirectory().getChildFile ("LayoutDiff.jui"));
    if (outDiff.openedOk()) {
        ValueTree diffTree;
        layout.saveLayoutToValueTree (diffTree);
        String layoutXML = diffTree.toXmlString();
        outDiff.setPosition (0);
        outDiff.truncate();
        outDiff.write (layoutXML.toRawUTF8(), layoutXML.getNumBytesAsUTF8());
        outDiff.flush();
    }
     */
    
    layout.realize (this);

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
    layout.updateGeometry ();
    resizer->setBounds (getRight()-16, getBottom()-16, 16, 16);    
}
