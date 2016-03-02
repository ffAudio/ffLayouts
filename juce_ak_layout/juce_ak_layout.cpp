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

Layout::Layout(Orientation o, juce::Component* owner)
  : orientation (o),
    isUpdating (false),
    isCummulatingStretch (false),
    owningComponent (owner)
{
}

Layout::~Layout()
{
}

void Layout::setOrientation (const Orientation o)
{
    orientation = o;
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
    Layout* sub = addSubLayout (o, idx, owningComponent);
    if (isVertical()) {
        float h = label->getFont().getHeight();
        sub->addComponent (label)->setFixedHeight (h);
    }
    else {
        sub->addComponent (label);
    }
    LabeledLayoutItem* labeledItem = new LabeledLayoutItem (c, label);
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

Layout* Layout::addSubLayout (Orientation o, int idx, juce::Component* owner)
{
    SubLayout* sub = new SubLayout (o, owningComponent);
    itemsList.insert (idx, sub);
    updateGeometry();
    return sub;
}

LayoutItem* Layout::addSSpacer (float sx, float sy, int idx)
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
    if (owningComponent) {
        updateGeometry (owningComponent->getLocalBounds());
    }
}

void Layout::updateGeometry (juce::Rectangle<int> bounds)
{
    // recursion check
    if (isUpdating) {
        return;
    }
    isUpdating = true;
    
    // remove items of deleted or invalid components
    for (int i=0; i<itemsList.size(); ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
        if (!item->isValid()) {
            itemsList.remove (i);
        }
    }

    float cummulatedX, cummulatedY;
    getCummulatedStretch (cummulatedX, cummulatedY);
    float availableWidth  = bounds.getWidth();
    float availableHeight = bounds.getHeight();
    
    if (isVertical()) {
        for (int i=0; i<itemsList.size(); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);
            float sx, sy;
            item->getStretch (sx, sy);
            float h = bounds.getHeight() * sy / cummulatedY;
            juce::Rectangle<int> childBounds (bounds.getX(), bounds.getY(), bounds.getWidth(), h);
            bool changedWidth, changedHeight;
            item->constrainBounds (childBounds, changedWidth, changedHeight);
            item->setLayoutBounds (childBounds);
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
        for (int i=0; i<itemsList.size(); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);

            if (item->getBoundsAreFinal()) {
                float h = item->getLayoutBounds().getHeight();
                if (orientation == BottomUp) {
                    y -= h;
                }
                item->setLayoutBounds (bounds.getX(), y, availableWidth, h);
                if (item->isSubLayout()) {
                    if (Layout* sub = dynamic_cast<Layout*>(item)) {
                        sub->updateGeometry (item->getPaddedLayoutBounds());
                    }
                }
                if (juce::Component* c = item->getComponent()) {
                    c->setBounds (item->getPaddedLayoutBounds());
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
                item->setLayoutBounds (bounds.getX(), y, availableWidth, h );
                if (item->isSubLayout()) {
                    if (Layout* sub = dynamic_cast<Layout*>(item)) {
                        sub->updateGeometry (item->getPaddedLayoutBounds());
                    }
                }
                if (juce::Component* c = item->getComponent()) {
                    c->setBounds (item->getPaddedLayoutBounds());
                }
                if (orientation == TopDown) {
                    y += h;
                }
            }
        }
    } else if (isHorizontal()) {
        for (int i=0; i<itemsList.size(); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);
            float sx, sy;
            item->getStretch (sx, sy);
            float w = bounds.getWidth() * sx / cummulatedX;
            juce::Rectangle<int> childBounds (bounds.getX(), bounds.getY(), w, bounds.getHeight());
            bool changedWidth, changedHeight;
            item->constrainBounds (childBounds, changedWidth, changedHeight);
            item->setLayoutBounds (childBounds);
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
        for (int i=0; i<itemsList.size(); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);

            if (item->getBoundsAreFinal()) {
                float w = item->getLayoutBounds().getWidth();
                if (orientation == RightToLeft) {
                    x -= w;
                }
                item->setLayoutBounds (x, bounds.getY(), w, availableHeight);
                juce::Rectangle<int> childBounds (x, bounds.getY(), w, availableHeight);
                if (item->isSubLayout()) {
                    if (Layout* sub = dynamic_cast<Layout*>(item)) {
                        sub->updateGeometry (item->getPaddedLayoutBounds());
                    }
                }
                if (juce::Component* c = item->getComponent()) {
                    c->setBounds (item->getPaddedLayoutBounds());
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
                item->setLayoutBounds (x, bounds.getY(), w, availableHeight);
                if (item->isSubLayout()) {
                    if (Layout* sub = dynamic_cast<Layout*>(item)) {
                        sub->updateGeometry (item->getPaddedLayoutBounds());
                    }
                }
                if (juce::Component* c = item->getComponent()) {
                    c->setBounds (item->getPaddedLayoutBounds());
                }
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
        const LayoutItem* item = getItem (i);
        if (item->isSubLayout()) {
            dynamic_cast<const Layout*>(item)->paintBounds (g);
            g.drawRect(item->getLayoutBounds());
        }
        else {
            g.drawRect(item->getLayoutBounds().reduced(1));
        }
    }
}

void Layout::getCummulatedStretch (float& w, float& h) const
{
    w = 0.0;
    h = 0.0;
    
    if (isCummulatingStretch) {
        return;
    }
    isCummulatingStretch = true;
    
    for (int i=0; i<itemsList.size(); ++i) {
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


// =============================================================================
SubLayout::SubLayout (Orientation o, juce::Component* owner) : Layout (o, owner), LayoutItem (LayoutItem::SubLayout)
{
}

void SubLayout::getStretch (float& w, float& h) const
{
    w = 0.0;
    h = 0.0;
    getCummulatedStretch (w, h);
}

void SubLayout::getSizeLimits (int& minW, int& maxW, int& minH, int& maxH)
{
    bool canConsumeWidth = false;
    bool canConsumeHeight = false;
    if (isVertical()) {
        for (int i=0; i<getNumItems(); ++i) {
            LayoutItem* item = getItem (i);
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
            LayoutItem* item = getItem (i);
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

