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



Layout::Layout (juce::Component* owner)
: owningComponent (owner)
{
}

Layout::Layout (const juce::String& xml, juce::Component* owner)
: owningComponent (owner)
{
    juce::ScopedPointer<juce::XmlElement> mainElement = juce::XmlDocument::parse (xml);
    
    // loading of the xml failed. Probably the xml was malformed, so that ValueTree could not parse it.
    jassert (mainElement);
    
    if (mainElement) {
        state = juce::ValueTree::fromXml (*mainElement);
        LayoutItem root (state);
        root.realize (state, owner, this);
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

Layout* Layout::addSubLayout (juce::ValueTree& parent, LayoutItem::Orientation o, int idx)
{
    Layout* sub = new Layout (o, owningComponent, this);
    itemsList.insert (idx, sub);
    return sub;
}

LayoutSplitter* Layout::addSplitterItem (juce::ValueTree& parent, float position, int idx)
{
    // a splitter is a component. If you hit this assert your splitter will not be displayed
    jassert(owningComponent);
    
    LayoutSplitter* splitter = new LayoutSplitter (owningComponent, position, isHorizontal(), this);
    owningComponent->addAndMakeVisible (splitter);
    itemsList.insert (idx, splitter);
    return splitter;
}

LayoutItem* Layout::addSpacer (juce::ValueTree& parent, float sx, float sy, int idx)
{
    LayoutItem* item = itemsList.insert (idx, new LayoutItem (LayoutItem::SpacerItem));
    item->setStretch (sx, sy);
    return item;
}

LayoutItem* Layout::addLine (int width, int idx)
{
    juce::DrawableRectangle* c = new juce::DrawableRectangle;
    c->setFill (juce::FillType (juce::Colours::black));
    if (juce::Component* owningComponent = getOwningComponent()) {
        owningComponent->addAndMakeVisible (c);
    }
    LayoutItem* item = itemsList.insert (idx, new LayoutItem (c, this, true));
    if (isHorizontal()) {
        setFixedWidth (width);
    }
    if (isVertical()) {
        setFixedHeight (width);
    }
    
    return item;
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

void Layout::updateGeometry ()
{
    LayoutItem root (state);
    juce::Rectangle<int> bounds = root.getItemBounds();
    if (!bounds.isEmpty()) {
        LayoutItem::updateGeometry (state, bounds);
    }
    else if (owningComponent) {
        LayoutItem::updateGeometry (state, owningComponent->getLocalBounds());
    }
}

void Layout::paintBounds (juce::Graphics& g) const
{
    LayoutItem::paintBounds (state, g);
}

/*
void Layout::fixUpLayoutItems ()
{
    if (isFixing) {
        return;
    }
    isFixing = true;
    
    // if it's the root layout save the bounds as well
    if (this == getRootLayout() && !getItemBounds().isEmpty()) {
        setProperty ("layoutBounds", getItemBounds().toString(), nullptr);
    }
    
    for (int i=0; i<itemsList.size(); ++i) {
        itemsList.getUnchecked (i)->fixUpLayoutItems();
    }
    isFixing = false;
}

void Layout::saveLayoutToValueTree (juce::ValueTree& tree) const
{
    tree = juce::ValueTree (getType());
    for (int i=0; i < itemsList.size(); ++i) {
        juce::ValueTree child;
        itemsList.getUnchecked (i)->saveLayoutToValueTree (child);
        tree.addChild (child, -1, nullptr);
    }
    tree.copyPropertiesFrom (*this, nullptr);
}
*/

//==============================================================================


