/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent()
{
    resizeConstraints = new ComponentBoundsConstrainer();
    resizeConstraints->setMinimumSize(400, 250);
    
    resizer = new ResizableCornerComponent (this, resizeConstraints);
    
    
    layout.setOrientation (Layout::TopDown);
    
    for (int i=0; i<10; ++i)
    {
        Component* button = components.add (new TextButton ("Button " + String (i+1)));
        addAndMakeVisible (button);
        LayoutItem* item = layout.addComponent (button);
        if (i==4) {
            item->setStretch (3.0, 3.0);
        }
    }
    
    Layout* subLayout = layout.addSubLayout (Layout::LeftToRight);
    for (int i=0; i<5; ++i)
    {
        Component* button = components.add (new TextButton ("Button " + String (i+1)));
        addAndMakeVisible (button);
        subLayout->addComponent (button);
    }

    
    layout.getLayoutItem(components.getUnchecked(8))->setStretch (2.0, 2.0);
    
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
