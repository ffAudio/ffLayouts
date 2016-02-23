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

class Layout;

//==============================================================================
/**
 LayoutItems are used to store links to components to layout. Also information
 like size hints and stretch factors are stored in the LayoutItems.
 
 
 @see Layout
 */
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
        stretchX (1.0),
        stretchY (1.0)
    {}
    
    LayoutItem (ItemType i=Invalid)
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
    bool isSubLayout ()         const { return itemType == SubLayout; }
    
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

    /**
     Used to set stretch factors for the wrapped component. The space is distributed
     according the sum of stretch factors.
     */
    virtual void setStretch (float w, float h)
    {
        stretchX = w;
        stretchY = h;
    }

    ItemType getItemType() const { return itemType; }
    
private:
    ItemType   itemType;
    
    Component::SafePointer<Component> componentPtr;
    
    Point<int> minimumSize;
    Point<int> maximumSize;
    float stretchX;
    float stretchY;
    
};

//==============================================================================
/**
 Layout aligns a bunch of Components in a row. It can be nested to get any kind of layout.
 
 @see Component
 @see LayoutItem
 */

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
    
    Layout (Orientation o=Unknown, Component* owner=nullptr);
    virtual ~Layout();
    
    /**
     Changes the orientation of the layout
     */
    void setOrientation (const Orientation);
    
    /**
     addComponent creates a LayoutItem to wrap the given Component. To add 
     properties like stretch factor, minimum sizes etc.  pointer to the created
     LayoutItem is returned. You don't need and should not keep this pointer longer 
     than current scope.
     */
    virtual LayoutItem* addComponent (Component*, int idx=-1);
    
    // TODO: grid layout
    //virtual LayoutItem& addComponent (Component*, const int x, const int y);
    
    /**
     Remove a component from the layout. The LayoutItem is destructed, but the
     Component is left untouched.
     */
    void removeComponent (Component*);
    
    /**
     Creates a nested layout inside a layout.
     */
    Layout* addSubLayout (Orientation, int idx=-1);

    /**
     Creates a spacer to put space between items.
     */
    LayoutItem* addSSpacer (int idx=-1, float sx=1.0, float sy=1.0);
    
    /**
     Retrieve the LayoutItem for a component. If the Component is not found in the
     Layout, a nullptr is returned.
     */
    LayoutItem* getLayoutItem (Component*);
    
    /**
     If the layout has an owning component, this calls updateGeometry with the
     bounds of the owning component.
     */
    virtual void updateGeometry ();
    
    /**
     Recompute the geometry of all components. Recoursively recomputes all sub layouts.
     */
    virtual void updateGeometry (Rectangle<int> bounds);
    
    /**
     Cummulates all stretch factors inside the nested layout
     */
    void getCummulatedStretch (float& w, float& h) const;
    

private:
    Orientation orientation;
    OwnedArray<LayoutItem> itemsList;
    bool isUpdating;
    mutable bool isCummulatingStretch;
    Component::SafePointer<Component> owningComponent;

};

/**
 A SubLayout can be used to add a nested layout inside a layout.
 
 @see Layout
 */
class SubLayout : public Layout, public LayoutItem
{
public:
    SubLayout (Orientation o=Unknown);
    virtual ~SubLayout() {};
    
    /**
     Cummulates all stretch factors inside the nested layout
     
     Along the orientation the factors are summed up. In the other
     dimension the maximum of the stretch factors is returned.
     */
    void getStretch (float& w, float& h) const override;
    
};



#endif  // LAYOUT_H_INCLUDED
