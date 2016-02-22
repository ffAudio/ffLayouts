/*
  ==============================================================================

    Layout.cpp
    Created: 21 Feb 2016 9:14:52pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "Layout.h"


Layout::Layout(Orientation o)
  : orientation (o),
    isUpdating (false),
    isCummulatingStretch (false)
{
}

Layout::~Layout()
{
}

void Layout::setOrientation (const Orientation o)
{
    orientation = o;
}


LayoutItem* Layout::addComponent (Component* c, const int idx)
{
    if (idx >= 0 && idx < itemsList.size()) {
        itemsList.insert (itemsList.begin() + idx, LayoutItem (c));
    }
    else {
        itemsList.push_back(LayoutItem (c));
    }
    
    return &(*itemsList.rbegin());
}

void Layout::removeComponent (Component* c)
{
    std::deque<LayoutItem>::iterator iter = itemsList.begin();
    while (iter!=itemsList.end()) {
        if (iter->isComponentItem() && iter->getComponent() == c) {
            itemsList.erase (iter);
        }
    }
}

Layout* Layout::addSubLayout (Orientation o)
{
    SubLayout sub(o);
    itemsList.push_back(sub);
    return itemsList.back().getLayout();
}


LayoutItem* Layout::getLayoutItem (Component* c)
{
    std::deque<LayoutItem>::iterator iter = itemsList.begin();
    while (iter!=itemsList.end()) {
        if (iter->isComponentItem() && iter->getComponent() == c) {
            return &(*iter);
        }
        ++iter;
    }
    return nullptr;
}

void Layout::updateGeometry (Rectangle<int> bounds)
{
    // recursion check
    if (isUpdating) {
        return;
    }
    isUpdating = true;
    
    // remove items of deleted or invalid components
    std::deque<LayoutItem>::iterator iter = itemsList.begin();
    while (iter!=itemsList.end()) {
        if (!iter->isValid()) {
            itemsList.erase (iter);
        }
        ++iter;
    }
    
    float cummulatedX, cummulatedY;
    getCummulatedStretch (cummulatedX, cummulatedY);
    
    if (orientation == TopDown) {
        std::deque<LayoutItem>::iterator item = itemsList.begin();
        float y = 0;
        while (item != itemsList.end()) {
            float sx = 0.0;
            float sy = 0.0;
            item->getStretch (sx, sy);
            float h = bounds.getHeight() * sy / cummulatedY;
            if (item->isComponentItem()) {
                item->getComponent()->setBounds (bounds.getX(), bounds.getY() + y, bounds.getWidth(), h);
                y += h;
            }
            ++item;
        }
    }
    else if (orientation == LeftToRight) {
        std::deque<LayoutItem>::iterator item = itemsList.begin();
        float x = 0;
        while (item != itemsList.end()) {
            float sx, sy;
            item->getStretch (sx, sy);
            float w = bounds.getWidth() * sx / cummulatedX;
            if (item->isComponentItem()) {
                item->getComponent()->setBounds (bounds.getX() + x, bounds.getY(), w, bounds.getHeight());
                x += w;
            }
            ++item;
        }
    }

    
    isUpdating = false;
}

void Layout::getCummulatedStretch (float& w, float& h) const
{
    w = 0.0;
    h = 0.0;
    
    if (isCummulatingStretch) {
        return;
    }
    isCummulatingStretch = true;
    
    std::deque<LayoutItem>::const_iterator item = itemsList.cbegin();
    while (item != itemsList.cend()) {
        float x, y;
        item->getStretch (x, y);
        if (orientation == LeftToRight || orientation == RightToLeft) {
            w += x;
            h = std::max (h, y);
        }
        else if (orientation == TopDown || orientation == BottomUp) {
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
SubLayout::SubLayout (Orientation o) : Layout (o)
{
    
}


void SubLayout::getStretch (float& w, float& h) const
{
    w = 0.0;
    h = 0.0;
    getCummulatedStretch (w, h);
}

