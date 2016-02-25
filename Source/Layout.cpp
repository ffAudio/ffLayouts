/*
  ==============================================================================

    Layout.cpp
    Created: 21 Feb 2016 9:14:52pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "Layout.h"


Layout::Layout(Orientation o, Component* owner)
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


LayoutItem* Layout::addComponent (Component* c, int idx)
{
    LayoutItem* item = itemsList.insert (idx, new LayoutItem (c));
    updateGeometry();
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
    updateGeometry();
}

LayoutItem* Layout::addLabeledComponent (Component* c, Orientation o, Label** labelPtr, int idx)
{
    // if the layout is not owned by a component, the label will not show up,
    // because addAndMakeVisible can not be called.
    jassert (owningComponent);
    
    Label* label = new Label();
    if (owningComponent) {
        owningComponent->addAndMakeVisible (label);
    }
    Layout* sub = addSubLayout (o, idx);
    sub->addComponent (label)->setStretch (0.2, 0.2);
    LayoutItem* item = itemsList.insert (idx, new LabeledLayoutItem (c, label));
    
    if (labelPtr) {
        *labelPtr = label;
    }

    updateGeometry();
    return item;
}


Layout* Layout::addSubLayout (Orientation o, int idx, Component* owner)
{
    SubLayout* sub = new SubLayout (o, owningComponent);
    itemsList.insert (idx, sub);
    updateGeometry();
    return sub;
}

LayoutItem* Layout::addSSpacer (int idx, float sx, float sy)
{
    LayoutItem* item = itemsList.insert (idx, new LayoutItem (LayoutItem::SpacerItem));
    updateGeometry();
    return item;
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

void Layout::updateGeometry ()
{
    if (owningComponent) {
        updateGeometry (owningComponent->getLocalBounds());
    }
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
    
    if (orientation == TopDown || orientation == BottomUp) {
        float y = 0;
        if (orientation == BottomUp) {
            y = bounds.getHeight();
        }
        for (int i=0; i<itemsList.size(); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);
            float sx = 0.0;
            float sy = 0.0;
            item->getStretch (sx, sy);
            float h = bounds.getHeight() * sy / cummulatedY;
            if (item->isValid() && orientation == BottomUp) {
                y -= h;
            }
            Rectangle<int> childBounds (bounds.getX(), bounds.getY() + y, bounds.getWidth(), h);
            if (item->isComponentItem()) {
                item->getComponent()->setBounds (childBounds);
            }
            else if (item->isSubLayout()) {
                SubLayout* sub = dynamic_cast<SubLayout*>(item);
                if (sub) {
                    sub->updateGeometry (childBounds);
                }
            }
            if (item->isValid() && orientation == TopDown) {
                y += h;
            }
        }
    }
    else if (orientation == LeftToRight || orientation == RightToLeft) {
        float x = 0;
        if (orientation == RightToLeft) {
            x = bounds.getWidth();
        }
        for (int i=0; i<itemsList.size(); ++i) {
            LayoutItem* item = itemsList.getUnchecked (i);
            float sx, sy;
            item->getStretch (sx, sy);
            float w = bounds.getWidth() * sx / cummulatedX;
            if (item->isValid() && orientation == RightToLeft) {
                x -= w;
            }
            Rectangle<int> childBounds (bounds.getX() + x, bounds.getY(), w, bounds.getHeight());
            if (item->isComponentItem()) {
                item->getComponent()->setBounds (childBounds);
            }
            else if (item->isSubLayout()) {
                SubLayout* sub = dynamic_cast<SubLayout*>(item);
                if (sub) {
                    sub->updateGeometry (childBounds);
                }
            }
            if (item->isValid() && orientation == LeftToRight) {
                x += w;
            }
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
SubLayout::SubLayout (Orientation o, Component* owner) : Layout (o, owner), LayoutItem (LayoutItem::SubLayout)
{
    
}


void SubLayout::getStretch (float& w, float& h) const
{
    w = 0.0;
    h = 0.0;
    getCummulatedStretch (w, h);
}

