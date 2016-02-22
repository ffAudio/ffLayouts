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


LayoutItem* Layout::addComponent (Component* c)
{
    LayoutItem* item = itemsList.add (new LayoutItem (c));

    return item;
}

void Layout::removeComponent (Component* c)
{
    for (int i=0; i<itemsList.size(); ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
        if (item->isComponentItem() && item->getComponent() == c) {
            itemsList.remove (i);
        }
    }
}

Layout* Layout::addSubLayout (Orientation o)
{
    SubLayout* sub = new SubLayout (o);
    itemsList.add (sub);
    return sub;
}


LayoutItem* Layout::getLayoutItem (Component* c)
{
    for (int i=0; i<itemsList.size(); ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
        if (item->isComponentItem() && item->getComponent() == c) {
            return item;
        }
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
    for (int i=0; i<itemsList.size(); ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
        if (!item->isValid()) {
            itemsList.remove (i);
        }
    }
    
    float cummulatedX, cummulatedY;
    getCummulatedStretch (cummulatedX, cummulatedY);
    
    if (orientation == TopDown) {
        float y = 0;
        for (int i=0; i<itemsList.size(); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);
            float sx = 0.0;
            float sy = 0.0;
            item->getStretch (sx, sy);
            float h = bounds.getHeight() * sy / cummulatedY;
            Rectangle<int> childBounds (bounds.getX(), bounds.getY() + y, bounds.getWidth(), h);
            if (item->isComponentItem()) {
                item->getComponent()->setBounds (childBounds);
                y += h;
            }
            else {
                SubLayout* sub = dynamic_cast<SubLayout*>(item);
                if (sub) {
                    sub->updateGeometry (childBounds);
                }
            }
            ++item;
        }
    }
    else if (orientation == LeftToRight) {
        float x = 0;
        for (int i=0; i<itemsList.size(); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);
            float sx, sy;
            item->getStretch (sx, sy);
            float w = bounds.getWidth() * sx / cummulatedX;
            Rectangle<int> childBounds (bounds.getX() + x, bounds.getY(), w, bounds.getHeight());
            if (item->isComponentItem()) {
                item->getComponent()->setBounds (childBounds);
                x += w;
            }
            else {
                SubLayout* sub = dynamic_cast<SubLayout*>(item);
                if (sub) {
                    sub->updateGeometry (childBounds);
                }
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
    
    for (int i=0; i<itemsList.size(); ++i) {
        LayoutItem* item = itemsList.getUnchecked (i);
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

