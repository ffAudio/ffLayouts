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

    juce_ak_layout.cpp
    Created: 21 Feb 2016 9:14:52pm

  ==============================================================================
*/

#ifdef JUCE_AK_LAYOUT_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif


#include "ak_layout.h"

#include "ak_layoutItem.cpp"


const juce::Identifier Layout::propResizable        ("resizable");
const juce::Identifier Layout::propResizerWidth     ("resizerWidth");
const juce::Identifier Layout::propResizerHeight    ("resizerHeight");
const juce::Identifier Layout::propMinWidth         ("minWidth");
const juce::Identifier Layout::propMaxWidth         ("maxWidth");
const juce::Identifier Layout::propMinHeight        ("minHeight");
const juce::Identifier Layout::propMaxHeight        ("maxHeight");
const juce::Identifier Layout::propAspectRatio      ("aspectRatio");


Layout::Layout (LayoutItem::Orientation o, juce::Component* owner)
: owningComponent (owner)
{
    state = LayoutItem (o).state;
}

Layout::Layout (const juce::String& xml, juce::Component* owner)
: owningComponent (owner)
{
    juce::ScopedPointer<juce::XmlElement> mainElement = juce::XmlDocument::parse (xml);
    
    // loading of the xml failed. Probably the xml was malformed, so that ValueTree could not parse it.
    jassert (mainElement);
    
    if (mainElement) {
        state = juce::ValueTree::fromXml (*mainElement);
        realize ();
        updateGeometry();
    }
}

Layout::Layout (const juce::ValueTree& state_, juce::Component* owner)
: owningComponent (owner)
{
    state = state_;
    LayoutItem root (state);
    root.realize (state, owner, this);
    updateGeometry();
}

Layout::~Layout ()
{
    masterReference.clear();
}

juce::Component* Layout::getOwningComponent()
{
    return owningComponent;
}

const juce::Component* Layout::getOwningComponent() const
{
    return owningComponent;    
}
/*
LayoutItem Layout::addComponent (juce::ValueTree& parent, juce::Component* c, bool owned, int idx)
{
    return LayoutItem::makeChildComponent (parent, c, owned, idx);
}
*/

void Layout::removeComponent (juce::Component* component)
{
    juce::ValueTree node = getLayoutItem (component);
    if (node.isValid() && node.getParent().isValid()) {
        node.getParent().removeChild (node, nullptr);
    }
}

/*
LayoutItem* Layout::addLabeledComponent (juce::Component* c, juce::StringRef text, Orientation o, int idx)
{
    // if the layout is not owned by a component, the label will not show up,
    // because addAndMakeVisible can not be called.
    jassert (owningComponent);
    
    juce::Label* label = new juce::Label (juce::String::empty, text);
    label->setJustificationType (juce::Justification::centred);
    if (owningComponent) {
        owningComponent->addAndMakeVisible (label);
    }
    Layout* sub = addSubLayout (o, idx);
    LayoutItem* labelItem = sub->addComponent (label, true);
    if (sub->isVertical()) {
        float h = label->getFont().getHeight();
        labelItem->setFixedHeight (h);
    }
    labelItem->setLabelText (label->getText());

    LayoutItem* labeledItem = sub->addComponent (c);
    return labeledItem;
}
*/

juce::ValueTree Layout::getLayoutItem (juce::Component* component)
{
    return LayoutItem::getLayoutItem (state, component);
}

void Layout::clearLayout (juce::UndoManager* undo)
{
    state.removeAllProperties (undo);
    state.removeAllChildren (undo);
}

void Layout::realize (juce::Component* owningComponent_)
{
    LayoutItem root (state);

    if (owningComponent_) {
        owningComponent = owningComponent_;
    }
    
    if (owningComponent &&
        state.hasProperty (propResizable) &&
        state.getProperty (propResizable)) {
        resizeConstraints = std::unique_ptr<juce::ComponentBoundsConstrainer> (new juce::ComponentBoundsConstrainer());
        if (state.hasProperty (propMinWidth))    resizeConstraints->setMinimumWidth     (state.getProperty (propMinWidth));
        if (state.hasProperty (propMaxWidth))    resizeConstraints->setMaximumWidth     (state.getProperty (propMaxWidth));
        if (state.hasProperty (propMinHeight))   resizeConstraints->setMinimumHeight    (state.getProperty (propMinHeight));
        if (state.hasProperty (propMaxHeight))   resizeConstraints->setMaximumHeight    (state.getProperty (propMaxHeight));
        if (state.hasProperty (propAspectRatio)) resizeConstraints->setFixedAspectRatio (state.getProperty (propAspectRatio));

        resizer = std::unique_ptr<juce::ResizableCornerComponent> (new juce::ResizableCornerComponent (owningComponent, resizeConstraints.get()));
        resizer->setSize (state.getProperty (propResizerWidth,  16),
                          state.getProperty (propResizerHeight, 16));
        owningComponent->addAndMakeVisible (resizer.get());
        juce::Rectangle<int> newBounds = owningComponent->getLocalBounds();
        resizeConstraints->setBoundsForComponent (owningComponent, newBounds, false, false, true, true);
        
    }
    root.realize (state, owningComponent, this);
}

void Layout::updateGeometry ()
{
    LayoutItem root (state);
    juce::Rectangle<int> bounds = root.getItemBounds();
    if (bounds.isEmpty() && owningComponent) {
        bounds = owningComponent->getLocalBounds();
    }
    if (!bounds.isEmpty()) {
        LayoutItem::updateGeometry (state, bounds);
        if (resizer) {
            resizer->setBounds(bounds.getRight() - resizer->getWidth(), bounds.getBottom() - resizer->getHeight(), resizer->getWidth(), resizer->getHeight());
        }
    }
}

void Layout::paintBounds (juce::Graphics& g) const
{
    LayoutItem::paintBounds (state, g);
}

//==============================================================================


