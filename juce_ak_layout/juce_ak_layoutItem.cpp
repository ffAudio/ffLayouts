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
 
 juce_ak_layoutItem.cpp
 Created: 12 Mar 2016 13:14:52pm
 
 ==============================================================================
 */


#include "juce_ak_layout.h"


LayoutItem::LayoutItem (juce::Component* c, Layout* parent)
  : juce::ValueTree ("Component"),
    itemType (ComponentItem),
    parentLayout (parent),
    componentPtr (c)
{
    jassert (c);
    if (!c->getComponentID().isEmpty()) {
        setProperty ("componentID", c->getComponentID(), nullptr);
    }
}

LayoutItem::LayoutItem (ItemType i, Layout* parent)
  : juce::ValueTree ((i==ComponentItem) ? "Component" :
                     (i==LabeledComponentItem) ? "LabeledComponent" :
                     (i==SplitterItem) ? "Splitter" :
                     (i==SpacerItem) ? "Spacer" :
                     (i==SubLayout) ? "Layout" : ""),
    itemType (i),
    parentLayout (parent)
{}

LayoutItem::~LayoutItem()
{
}


bool LayoutItem::isValid()
{
    if (itemType == Invalid) {
        return false;
    }
    if (itemType == ComponentItem && componentPtr == nullptr) {
        return false;
    }
    return true;
}

Layout* LayoutItem::getParentLayout()
{
    return parentLayout;
}

Layout* LayoutItem::getRootLayout()
{
    Layout* p = parentLayout;
    while (p && p->getParentLayout()) {
        p = p->getParentLayout();
    }
    return p;
}

juce::Component* LayoutItem::getComponent () const
{
    return componentPtr.getComponent();
}

void LayoutItem::setComponent (juce::Component* ptr)
{
    componentPtr = ptr;
    if (ptr->getComponentID().isEmpty()) {
        removeProperty ("componentID", nullptr);
    }
    else {
        setProperty ("componentID", ptr->getComponentID(), nullptr);
    }
}

void LayoutItem::saveLayoutToValueTree (juce::ValueTree& tree) const
{
    tree = juce::ValueTree (*this);
}

void loadLayoutFromValueTree (const juce::ValueTree tree, juce::Component* owner)
{
    
}


// =============================================================================
void LayoutItem::addListener (LayoutItemListener* const newListener)
{
    layoutItemListeners.add (newListener);
}

void LayoutItem::removeListener (LayoutItemListener* const listener)
{
    layoutItemListeners.remove (listener);
}

void LayoutItem::callListenersCallback (juce::Rectangle<int> newBounds)
{
    layoutItemListeners.call(&LayoutItemListener::layoutBoundsChanged, newBounds);
}


//==============================================================================
LayoutSplitter::LayoutSplitter (juce::Component* owningComponent, float position, bool horizontal, Layout* parent)
:  LayoutItem(Layout::SplitterItem, parent)
{
    setComponent (this);
    setIsHorizontal (horizontal);
    if (horizontal) {
        setMouseCursor (juce::MouseCursor::LeftRightResizeCursor);
        setFixedWidth (3);
    }
    else {
        setMouseCursor (juce::MouseCursor::UpDownResizeCursor);
        setFixedHeight (3);
    }
}

LayoutSplitter::~LayoutSplitter()
{
}

void LayoutSplitter::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::grey);
}

void LayoutSplitter::mouseDrag (const juce::MouseEvent &event)
{
    if (Component* c = getParentComponent()) {
        float pos;
        juce::Rectangle<int> layoutBounds (c->getLocalBounds());
        if (Layout* p = getParentLayout()) {
            if (!p->getItemBounds().isEmpty()) {
                layoutBounds = p->getItemBounds();
            }
        }
        if (getIsHorizontal()) {
            pos = (event.getEventRelativeTo(c).position.getX() - layoutBounds.getX()) / layoutBounds.getWidth();
        }
        else {
            pos = (event.getEventRelativeTo(c).position.getY() - layoutBounds.getY()) / layoutBounds.getHeight();
        }
        setRelativePosition (juce::jmax (getMinimumRelativePosition(), juce::jmin (getMaximumRelativePosition(), pos)));
    }
    if (Layout* rootLayout = getRootLayout()) {
        rootLayout->updateGeometry();
    }
}

void LayoutSplitter::setRelativePosition (float position, juce::UndoManager* undo)
{
    setProperty ("relativePosition", position, undo);
}

float LayoutSplitter::getRelativePosition() const
{
    return getProperty ("relativePosition", 0.5);
}

void LayoutSplitter::setMinimumRelativePosition (const float min, juce::UndoManager* undo)
{
    setProperty ("relativeMinPosition", min, undo);
}

void LayoutSplitter::setMaximumRelativePosition (const float max, juce::UndoManager* undo)
{
    setProperty ("relativeMaxPosition", max, undo);
}

float LayoutSplitter::getMinimumRelativePosition() const
{
    return getProperty ("relativeMinPosition", 0.0);
}

float LayoutSplitter::getMaximumRelativePosition() const
{
    return getProperty ("relativeMaxPosition", 1.0);
}

void LayoutSplitter::setIsHorizontal (bool isHorizontal, juce::UndoManager* undo)
{
    setProperty ("isHorizontal", isHorizontal, undo);
}

bool LayoutSplitter::getIsHorizontal() const
{
    return getProperty("isHorizontal", false);
}

