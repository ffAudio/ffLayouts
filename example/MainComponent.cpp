/*
  ==============================================================================

    This file was auto-generated!

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
    row->addComponent (b1);
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

    column3->addSSpacer();

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
            column4->addSSpacer (1.0, 0.2);
        }
        addAndMakeVisible (slider);
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
