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
        LabeledComponentItem,
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
    
    virtual Label* getLabel () { return nullptr; }
    
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
 The LabeledLayoutItem takes ownership of the label component
 */
class LabeledLayoutItem : public LayoutItem
{
public:
    LabeledLayoutItem (Component* c, Label* l)
      : LayoutItem (c),
        label (l) {}
    
    virtual ~LabeledLayoutItem() {};
    
    /**
     Return the created label, if there is any.
     */
    Label* getLabel() override { return label; }
    
    
private:
    ScopedPointer<Label> label;
};

//==============================================================================
/**
 Layout aligns a bunch of Components in a row. It can be nested to get any kind of layout.
 
 A minimal example to layout three buttons from left to right would look like this:
 
 \code{.cpp}
 class MainContentComponent   : public Component
 {
 public:
     MainContentComponent () : 
         myLayout (Layout::LeftToRight, this),
         button1 ("Button 1"),
         button2 ("Button 2"),
         button3 ("Button 3")
     {
         addAndMakeVisible (button1);
         addAndMakeVisible (button2);
         addAndMakeVisible (button3);
         myLayout.addComponent (&button1);
         myLayout.addComponent (&button2);
         myLayout.addComponent (&button3);
     }
 
     void resized() override {
         myLayout.updateGeometry ();
     }
 
 private:
     Layout myLayout;
 
     TextButton button1;
     TextButton button2;
     TextButton button3;
 };
 \endcode

 
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
     properties like stretch factor, minimum sizes etc. a pointer to the created
     LayoutItem is returned. You don't need and should not keep this pointer longer 
     than current scope. If you need to alter the item you can access it via @see Layout::getLayoutItem
     */
    virtual LayoutItem* addComponent (Component*, int idx=-1);
    
    // TODO: grid layout
    //virtual LayoutItem* addComponent (Component*, const int x, const int y);
    
    /**
     Remove a component from the layout. The LayoutItem is destructed, but the
     Component is left untouched.
     */
    void removeComponent (Component*);
    
    /**
     Add a component with a label in a sub layout. By chosing the orientation the 
     placement of the label can be set. Either a pointer to a Label pointer can be 
     set to return the created label, or you can call getLabel on the returned LayoutItem.
     */
    LayoutItem* addLabeledComponent (Component*, Orientation, Label** labelPtr=nullptr, int idx=-1);
    

    /**
     Convenience method to ad a labeled component with a given text
     */
    LayoutItem* addLabeledComponent (Component*, StringRef, Orientation o=Layout::TopDown, int idx=-1);
    
    /**
     Creates a nested layout inside a layout.
     */
    Layout* addSubLayout (Orientation, int idx=-1, Component* owner=nullptr);

    /**
     Creates a spacer to put space between items. Use stretch factors to increase
     the space it occupies
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
    
protected:
    /**
     This is for internal use only. You should not need to call this method
     */
    void addRawItem (LayoutItem* item, int idx=-1);

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
    SubLayout (Orientation o=Unknown, Component* owner=nullptr);
    virtual ~SubLayout() {};
    
    /**
     Cummulates all stretch factors inside the nested layout
     
     Along the orientation the factors are summed up. In the other
     dimension the maximum of the stretch factors is returned.
     */
    void getStretch (float& w, float& h) const override;

};



#endif  // LAYOUT_H_INCLUDED
