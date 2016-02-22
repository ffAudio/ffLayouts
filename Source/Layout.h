/*
  ==============================================================================

    Layout.h
    Created: 21 Feb 2016 9:14:52pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef LAYOUT_H_INCLUDED
#define LAYOUT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include <deque>

class Layout;

class LayoutItem
{
public:
    enum ItemType {
        Invalid = 0,
        ComponentItem,
        SplitterItem,
        MovableSplitterItem,
        SpacerItem,
        SubLayout
        
    };
    
    LayoutItem (Component* c)
      : itemType (ComponentItem),
        componentPtr (c),
        preferredSize (c->getWidth(), c->getHeight()),
        stretchX (1.0),
        stretchY (1.0)
    {}
    
    LayoutItem (ItemType i=Invalid, Component* ptr=nullptr)
      : itemType (i),
        stretchX (1.0),
        stretchY (1.0)
    {}
        
    virtual ~LayoutItem() {}
    
    bool isValid()
    {
        if (itemType == Invalid) {
            return false;
        }
        if (itemType == ComponentItem && componentPtr == nullptr) {
            return false;
        }
        return true;
    }
    
    Component* getComponent ()  const { return componentPtr.getComponent(); }
    bool isComponentItem ()     const { return itemType == ComponentItem; }
    
    void setPreferredSize (const int w, const int h)
    {
        preferredSize.setXY (w, h);
    }
    
    void setMinimumSize (const int w, const int h)
    {
        minimumSize.setXY (w, h);
    }
    
    void setMaximumSize (const int w, const int h)
    {
        maximumSize.setXY (w, h);
    }
    
    virtual void getStretch (float& w, float& h) const
    {
        w = stretchX;
        h = stretchY;
    }

    virtual void setStretch (float w, float h)
    {
        stretchX = w;
        stretchY = h;
    }

    ItemType getItemType() const { return itemType; }
    
    virtual Layout* getLayout() { return nullptr; }
    
private:
    ItemType   itemType;
    
    Component::SafePointer<Component> componentPtr;
    
    Point<int> minimumSize;
    Point<int> preferredSize;
    Point<int> maximumSize;
    float stretchX;
    float stretchY;
    
};

class Layout
{
public:
    enum Orientation {
        Unknown = 0,
        LeftToRight,
        TopDown,
        RightToLeft,
        BottomUp,
        GridLayout
    };
    
    Layout (Orientation o=Unknown);
    virtual ~Layout();
    
    void setOrientation (const Orientation);
    
    virtual LayoutItem* addComponent (Component*);
    
    //virtual LayoutItem& addComponent (Component*, const int x, const int y);
    
    void removeComponent (Component*);
    
    Layout* addSubLayout (Orientation);
    
    LayoutItem* getLayoutItem (Component*);
    
    virtual void updateGeometry (Rectangle<int> bounds);
    
    void getCummulatedStretch (float& w, float& h) const;
    

protected:
    Orientation orientation;
    
    OwnedArray<LayoutItem> itemsList;

private:
    bool isUpdating;
    mutable bool isCummulatingStretch;

};

class SubLayout : public LayoutItem, public Layout
{
public:
    SubLayout (Orientation o=Unknown);
    virtual ~SubLayout() {};
    
    void getStretch (float& w, float& h) const override;
    
};



#endif  // LAYOUT_H_INCLUDED
