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


#include "juce_ak_layout.h"

Layout::Layout(Orientation o, juce::Component* owner, Layout* parent)
  : LayoutItem (SubLayout, parent),
    isUpdating (false),
    isCummulatingStretch (false),
    owningComponent (owner)
{
    setOrientation (o);
    setStretch (-1.0, -1.0);
}

Layout::~Layout()
{
}

void Layout::setOrientation (const Orientation o, juce::UndoManager* undo)
{
    setProperty ("orientation", getNameFromOrientation (o).toString(), undo);
}

Layout::Orientation Layout::getOrientation() const
{
    return getOrientationFromName (juce::Identifier(getProperty ("orientation", Layout::orientationUnknown.toString())));
}

bool Layout::isHorizontal () const
{
    Layout::Orientation o = getOrientation();
    return o == LeftToRight || o == RightToLeft;
}

bool Layout::isVertical () const
{
    Layout::Orientation o = getOrientation();
    return o == TopDown || o == BottomUp;
}

juce::Identifier Layout::orientationUnknown ("unknown");
juce::Identifier Layout::orientationLeftToRight ("leftToRight");
juce::Identifier Layout::orientationTopDown ("topDown");
juce::Identifier Layout::orientationRightToLeft ("rightToLeft");
juce::Identifier Layout::orientationBottomUp ("bottomUp");


Layout::Orientation Layout::getOrientationFromName (juce::Identifier name)
{
    if (name == orientationLeftToRight) {
        return Layout::LeftToRight;
    }
    else if (name == orientationTopDown) {
        return Layout::TopDown;
    }
    else if (name == orientationRightToLeft) {
        return Layout::RightToLeft;
    }
    else if (name == orientationBottomUp) {
        return Layout::BottomUp;
    }
    else
        return Layout::Unknown;
}

juce::Identifier Layout::getNameFromOrientation (Layout::Orientation o)
{
    if (o == Layout::LeftToRight) {
        return orientationLeftToRight;
    }
    else if (o == Layout::TopDown) {
        return orientationTopDown;
    }
    else if (o == Layout::RightToLeft) {
        return orientationRightToLeft;
    }
    else if (o == Layout::BottomUp) {
        return orientationBottomUp;
    }
    else
        return orientationUnknown;
}

LayoutItem* Layout::addComponent (juce::Component* c, int idx)
{
    LayoutItem* item = itemsList.insert (idx, new LayoutItem (c));
    updateGeometry();
    return item;
}

void Layout::removeComponent (juce::Component* c)
{
    for (int i=0; i<itemsList.size(); ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
        if (item->isComponentItem() && item->getComponent() == c) {
            itemsList.remove (i);
        }
    }
    updateGeometry();
}

LayoutItem* Layout::addLabeledComponent (juce::Component* c, Orientation o, juce::Label** labelPtr, int idx)
{
    // if the layout is not owned by a component, the label will not show up,
    // because addAndMakeVisible can not be called.
    jassert (owningComponent);
    
    juce::Label* label = new juce::Label();
    if (owningComponent) {
        owningComponent->addAndMakeVisible (label);
    }
    Layout* sub = addSubLayout (o, idx);
    if (sub->isVertical()) {
        float h = label->getFont().getHeight();
        sub->addComponent (label)->setFixedHeight (h);
    }
    else {
        sub->addComponent (label);
    }
    LabeledLayoutItem* labeledItem = new LabeledLayoutItem (c, label, this);
    sub->addRawItem (labeledItem);
    
    if (labelPtr) {
        *labelPtr = label;
    }

    updateGeometry();
    return labeledItem;
}

LayoutItem* Layout::addLabeledComponent (juce::Component* component, juce::StringRef text, Orientation o, int idx)
{
    LayoutItem* item = addLabeledComponent(component, o, nullptr, idx);
    if (juce::Label* label = item->getLabel()) {
        label->setText (text, juce::dontSendNotification);
        label->setJustificationType (juce::Justification::centred);
    }
    return item;
}

Layout* Layout::addSubLayout (Orientation o, int idx)
{
    Layout* sub = new Layout (o, owningComponent, this);
    itemsList.insert (idx, sub);
    updateGeometry();
    return sub;
}

LayoutSplitter* Layout::addSplitterItem (float position, int idx)
{
    // a splitter is a component. If you hit this assert your splitter will not be displayed
    jassert(owningComponent);
    
    LayoutSplitter* splitter = new LayoutSplitter (owningComponent, position, isHorizontal(), this);
    owningComponent->addAndMakeVisible (splitter);
    itemsList.insert (idx, splitter);
    updateGeometry();
    return splitter;
}

LayoutItem* Layout::addSpacer (float sx, float sy, int idx)
{
    LayoutItem* item = itemsList.insert (idx, new LayoutItem (LayoutItem::SpacerItem));
    item->setStretch (sx, sy);
    updateGeometry();
    return item;
}

LayoutItem* Layout::getLayoutItem (juce::Component* c)
{
    for (int i=0; i<itemsList.size(); ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
        if (item->isComponentItem() && item->getComponent() == c) {
            return item;
        }
        else if (item->isSubLayout()) {
            // search also sub layouts recoursively
            if (LayoutItem* subItem = getLayoutItem (c)) {
                return subItem;
            }
        }
    }
    return nullptr;
}

void Layout::addRawItem (LayoutItem* item, int idx)
{
    itemsList.insert (idx, item);
}


void Layout::updateGeometry ()
{
    if (!getItemBounds().isEmpty()) {
        updateGeometry (getItemBounds());
    }
    else if (owningComponent) {
        updateGeometry (owningComponent->getLocalBounds());
    }
}

void Layout::updateGeometry (juce::Rectangle<int> bounds)
{
    // remove items of deleted or invalid components
    for (int i=0; i<itemsList.size(); ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
        if (!item->isValid()) {
            itemsList.remove (i);
        }
    }
    
    const Orientation orientation = getOrientation();
    
    // find splitter items
    int last = 0;
    juce::Rectangle<int> childBounds (bounds);
    for (int i=0; i<itemsList.size(); ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
        if (item->isSplitterItem()) {
            if (LayoutSplitter* splitter = dynamic_cast<LayoutSplitter*>(item)) {
                juce::Rectangle<int> splitterBounds (bounds);
                if (orientation == Layout::LeftToRight) {
                    int right = childBounds.getX() + splitter->getRelativePosition() * bounds.getWidth();
                    updateGeometry (childBounds.withRight (right-1), last, i);
                    splitterBounds.setX (right-1);
                    splitterBounds.setWidth (3);
                    childBounds.setLeft (right+1);
                }
                else if (orientation == Layout::TopDown) {
                    int bottom = childBounds.getY() + splitter->getRelativePosition() * bounds.getHeight();
                    updateGeometry (childBounds.withBottom (bottom), last, i);
                    splitterBounds.setY (bottom-1);
                    splitterBounds.setHeight (3);
                    childBounds.setTop (bottom+1);
                }
                else if (orientation == Layout::RightToLeft) {
                    int left = childBounds.getX() + splitter->getRelativePosition() * bounds.getWidth();
                    updateGeometry (childBounds.withLeft (left), last, i);
                    splitterBounds.setX (left-1);
                    splitterBounds.setWidth (3);
                    childBounds.setRight (left-1);
                }
                else if (orientation == Layout::BottomUp) {
                    int top = childBounds.getY() + splitter->getRelativePosition() * bounds.getHeight();
                    updateGeometry (childBounds.withTop (top), last, i);
                    splitterBounds.setY (top-1);
                    splitterBounds.setHeight (3);
                    childBounds.setBottom (top-1);
                }
                splitter->setItemBounds (splitterBounds);
                splitter->setBounds (splitterBounds);
                splitter->setBoundsAreFinal (true);
            }
            i++;
            last = i;
        }
    }
    
    // layout rest right of splitter, if any
    updateGeometry (childBounds, last, itemsList.size());
}

void Layout::updateGeometry (juce::Rectangle<int> bounds, int start, int end)
{
    // recursion check
    if (isUpdating) {
        return;
    }
    isUpdating = true;

    float cummulatedX, cummulatedY;
    getCummulatedStretch (cummulatedX, cummulatedY, start, end);
    float availableWidth  = bounds.getWidth();
    float availableHeight = bounds.getHeight();
    const Orientation orientation = getOrientation();
    
    if (isVertical()) {
        for (int i=start; i<juce::jmin (itemsList.size(), end); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);
            float sx, sy;
            item->getStretch (sx, sy);
            float h = bounds.getHeight() * sy / cummulatedY;
            juce::Rectangle<int> childBounds (bounds.getX(), bounds.getY(), bounds.getWidth(), h);
            bool changedWidth, changedHeight;
            item->constrainBounds (childBounds, changedWidth, changedHeight, true);
            item->setItemBounds (childBounds);
            if (changedHeight) {
                item->setBoundsAreFinal (true);
                availableHeight -= childBounds.getHeight();
                cummulatedY -= sy;
            }
            else {
                item->setBoundsAreFinal (false);
            }
            if (changedWidth) {
                availableWidth = std::max (bounds.getWidth(), childBounds.getWidth());
            }
        }

        float y = bounds.getY();
        if (orientation == BottomUp) {
            y = bounds.getY() + bounds.getHeight();
        }
        for (int i=start; i<juce::jmin (itemsList.size(), end); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);

            if (item->getBoundsAreFinal()) {
                float h = item->getItemBounds().getHeight();
                if (orientation == BottomUp) {
                    y -= h;
                }
                item->setItemBounds (bounds.getX(), y, availableWidth, h);
                if (item->isSubLayout()) {
                    if (Layout* sub = dynamic_cast<Layout*>(item)) {
                        sub->updateGeometry (item->getPaddedItemBounds());
                    }
                }
                if (juce::Component* c = item->getComponent()) {
                    c->setBounds (item->getPaddedItemBounds());
                }

                if (orientation == TopDown) {
                    y += h;
                }
            }
            else {
                float sx, sy;
                item->getStretch (sx, sy);
                float h = availableHeight * sy /cummulatedY;
                if (orientation == BottomUp) {
                    y -= h;
                }
                item->setItemBounds (bounds.getX(), y, availableWidth, h );
                if (item->isSubLayout()) {
                    if (Layout* sub = dynamic_cast<Layout*>(item)) {
                        sub->updateGeometry (item->getPaddedItemBounds());
                    }
                }
                if (juce::Component* c = item->getComponent()) {
                    c->setBounds (item->getPaddedItemBounds());
                }
                item->callListenersCallback (item->getPaddedItemBounds());
                if (orientation == TopDown) {
                    y += h;
                }
            }
        }
    } else if (isHorizontal()) {
        for (int i=start; i<juce::jmin (itemsList.size(), end); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);
            float sx, sy;
            item->getStretch (sx, sy);
            float w = bounds.getWidth() * sx / cummulatedX;
            juce::Rectangle<int> childBounds (bounds.getX(), bounds.getY(), w, bounds.getHeight());
            bool changedWidth, changedHeight;
            item->constrainBounds (childBounds, changedWidth, changedHeight, false);
            item->setItemBounds (childBounds);
            if (changedWidth) {
                item->setBoundsAreFinal (true);
                availableWidth -= childBounds.getWidth();
                cummulatedX -= sx;
            }
            else {
                item->setBoundsAreFinal (false);
            }
            if (changedHeight) {
                availableHeight = std::max (bounds.getHeight(), childBounds.getHeight());
            }
        }

        float x = bounds.getX();
        if (orientation == RightToLeft) {
            x = bounds.getX() + bounds.getWidth();
        }
        for (int i=start; i<juce::jmin (itemsList.size(), end); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);

            if (item->getBoundsAreFinal()) {
                float w = item->getItemBounds().getWidth();
                if (orientation == RightToLeft) {
                    x -= w;
                }
                item->setItemBounds (x, bounds.getY(), w, availableHeight);
                juce::Rectangle<int> childBounds (x, bounds.getY(), w, availableHeight);
                if (item->isSubLayout()) {
                    if (Layout* sub = dynamic_cast<Layout*>(item)) {
                        sub->updateGeometry (item->getPaddedItemBounds());
                    }
                }
                if (juce::Component* c = item->getComponent()) {
                    c->setBounds (item->getPaddedItemBounds());
                }

                if (orientation == LeftToRight) {
                    x += w;
                }
            }
            else {
                float sx, sy;
                item->getStretch (sx, sy);
                float w = availableWidth * sx /cummulatedX;
                if (orientation == RightToLeft) {
                    x -= w;
                }
                item->setItemBounds (x, bounds.getY(), w, availableHeight);
                if (item->isSubLayout()) {
                    if (Layout* sub = dynamic_cast<Layout*>(item)) {
                        sub->updateGeometry (item->getPaddedItemBounds());
                    }
                }
                if (juce::Component* c = item->getComponent()) {
                    c->setBounds (item->getPaddedItemBounds());
                }
                item->callListenersCallback (item->getPaddedItemBounds());
                if (orientation == LeftToRight) {
                    x += w;
                }
            }
        }
    }

    isUpdating = false;
}

void Layout::paintBounds (juce::Graphics& g) const
{
    if (isHorizontal()) {
        g.setColour (juce::Colours::red);
    }
    else if (isVertical()) {
        g.setColour (juce::Colours::green);
    }
    else {
        g.setColour (juce::Colours::grey);
    }
    for (int i=0; i<getNumItems(); ++i) {
        const LayoutItem* item = getLayoutItem (i);
        if (item->isSubLayout()) {
            dynamic_cast<const Layout*>(item)->paintBounds (g);
            g.drawRect(item->getItemBounds());
        }
        else {
            g.drawRect(item->getItemBounds().reduced(1));
        }
    }
}

void Layout::getStretch (float& w, float& h) const
{
    w = 0.0;
    h = 0.0;
    LayoutItem::getStretch(w, h);
    if (w < 0 && h < 0) {
        w = 0.0;
        h = 0.0;
        getCummulatedStretch (w, h);
    }
}

void Layout::getCummulatedStretch (float& w, float& h, int start, int end) const
{
    w = 0.0;
    h = 0.0;
    
    if (isCummulatingStretch) {
        return;
    }
    isCummulatingStretch = true;
    
    if (end<0) {
        end = itemsList.size();
    }
    
    for (int i=start; i<end; ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
        float x, y;
        item->getStretch (x, y);
        if (isHorizontal()) {
            w += x;
            h = std::max (h, y);
        }
        else if (isVertical()) {
            w = std::max (w, x);
            h += y;
        }
        else {
            w += x;
            h += y;
        }
        ++item;
    }
    
    isCummulatingStretch = false;
}

void Layout::getSizeLimits (int& minW, int& maxW, int& minH, int& maxH)
{
    bool canConsumeWidth = false;
    bool canConsumeHeight = false;
    if (isVertical()) {
        for (int i=0; i<getNumItems(); ++i) {
            LayoutItem* item = getLayoutItem (i);
            if (item->getMinimumWidth() >= 0) minW = (minW < 0) ? item->getMinimumWidth() : juce::jmax(minW, item->getMinimumWidth());
            if (item->getMaximumWidth() >= 0) maxW = (maxW < 0) ? item->getMaximumWidth() : juce::jmin(maxW, item->getMaximumWidth());
            if (item->getMinimumHeight() >= 0) minH = (minH < 0) ? item->getMinimumHeight() : minH + item->getMinimumHeight();
            if (item->getMaximumHeight() >= 0) {
                maxH = (maxH < 0) ? item->getMaximumHeight() : maxH + item->getMaximumHeight();
            }
            else {
                canConsumeHeight = true;
            }
        }
    }
    else if (isHorizontal()) {
        for (int i=0; i<getNumItems(); ++i) {
            LayoutItem* item = getLayoutItem (i);
            if (item->getMinimumWidth() >= 0) minW = (minW < 0) ? item->getMinimumWidth() : minW + item->getMinimumWidth();
            if (item->getMaximumWidth() >= 0) {
                maxW = (maxW < 0) ? item->getMaximumWidth() : maxW + item->getMaximumWidth();
            }
            else {
                canConsumeWidth = true;
            }
            if (item->getMinimumHeight() >= 0) minH = (minH < 0) ? item->getMinimumHeight() : juce::jmax(minH, item->getMinimumHeight());
            if (item->getMaximumHeight() >= 0) maxH = (maxH < 0) ? item->getMaximumHeight() : juce::jmin(maxH, item->getMaximumHeight());
        }
    }
    if (canConsumeWidth)  maxW = -1;
    if (canConsumeHeight) maxH = -1;
}

void Layout::saveLayoutToValueTree (juce::ValueTree& tree) const
{
    tree = juce::ValueTree (*this);
    for (int i=0; i < itemsList.size(); ++i) {
        juce::ValueTree child;
        itemsList.getUnchecked (i)->saveLayoutToValueTree (child);
        tree.addChild (child, -1, nullptr);
    }
}

//==============================================================================


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

void LayoutItem::saveLayoutToValueTree (juce::ValueTree& tree) const
{
    tree = juce::ValueTree (*this);
}

void loadLayoutFromValueTree (const juce::ValueTree tree, juce::Component* owner)
{
    
}

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

