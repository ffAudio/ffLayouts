/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent() : layout (Layout::LeftToRight, this)
{
    resizeConstraints = new ComponentBoundsConstrainer();
    resizeConstraints->setMinimumSize(400, 250);
    
    resizer = new ResizableCornerComponent (this, resizeConstraints);

    Layout* sub2 = layout.addSubLayout (Layout::TopDown);
    
    for (int i=0; i<5; ++i) {
        Slider* slider = new Slider();
        components.add (slider);
        slider->setSliderStyle (Slider::LinearHorizontal);
        slider->setTextBoxStyle (Slider::TextBoxBelow, true, 50, 18);
        addAndMakeVisible (slider);
        sub2->addComponent (slider);
    }
    
    for (int i=0; i<3; ++i)
    {
        Component* button = components.add (new TextButton ("Button " + String (i+1)));
        addAndMakeVisible (button);
        LayoutItem* item = layout.addComponent (button);
        if (i==1) {
            item->setStretch (3.0, 3.0);
        }
    }
    
    Layout* subLayout = layout.addSubLayout (Layout::TopDown);
    for (int i=0; i<5; ++i)
    {
        Component* button = components.add (new TextButton ("Button " + String (i+6)));
        addAndMakeVisible (button);
        subLayout->addComponent (button);
    }
    
    
    Component* extractButton = components.getUnchecked (6);
    
    layout.removeComponent (extractButton);
    Layout* subsub = layout.addSubLayout(Layout::TopDown, 2);
    subsub->addComponent (extractButton);
    
    Layout* subsubsub = subsub->addSubLayout (Layout::LeftToRight);
    
    for (int i=0; i<3; ++i)
    {
        Component* button = components.add (new TextButton ("Button " + String (i+11)));
        addAndMakeVisible (button);
        subsubsub->addComponent (button);
    }
    
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
