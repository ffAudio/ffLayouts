/*
  ==============================================================================

    juce_ak_layout.h
    Created: 21 Feb 2016 9:14:52pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef JUCE_AK_LAYOUT_H_INCLUDED
#define JUCE_AK_LAYOUT_H_INCLUDED

#include "juce_gui_basics/juce_gui_basics.h"

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
    
    LayoutItem (juce::Component* c)
      : itemType (ComponentItem),
        componentPtr (c),
        stretchX (1.0),
        stretchY (1.0),
        minWidth (-1.0),
        maxWidth (-1.0),
        minHeight (-1.0),
        maxHeight (-1.0),
        paddingTop (0.0),
        paddingLeft (0.0),
        paddingRight (0.0),
        paddingBottom (0.0)
    {}
    
    LayoutItem (ItemType i=Invalid)
      : itemType (i),
        stretchX (1.0),
        stretchY (1.0),
        minWidth (-1.0),
        maxWidth (-1.0),
        minHeight (-1.0),
        maxHeight (-1.0),
        paddingTop (0.0),
        paddingLeft (0.0),
        paddingRight (0.0),
        paddingBottom (0.0)
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
    
    juce::Component* getComponent ()  const { return componentPtr.getComponent(); }
    bool isComponentItem ()     const { return itemType == ComponentItem; }
    bool isSubLayout ()         const { return itemType == SubLayout; }
    
    virtual juce::Label* getLabel () { return nullptr; }
    
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

    /**
     Set constraints to the items size. In doubt the minimum size is used.
     Constraints work only in that direction the layout operates (horizontally/vertically).
     For a fixed size set the same value to minimum and maximum.
     */
    void setMinimumWidth  (const int w) { minWidth = w; }
    void setMaximumWidth  (const int w) { maxWidth = w; }
    void setMinimumHeight (const int h) { minHeight = h; }
    void setMaximumHeight (const int h) { maxHeight = h; }

    void setPaddingTop    (const float p) { paddingTop = p; }
    void setPaddingLeft   (const float p) { paddingLeft = p; }
    void setPaddingRight  (const float p) { paddingRight = p; }
    void setPaddingBottom (const float p) { paddingBottom = p; }
    void setPadding (const float p) {
        setPaddingTop (p);
        setPaddingLeft (p);
        setPaddingRight (p);
        setPaddingBottom (p);
    }
    float getPaddingTop () const    { return paddingTop; }
    float getPaddingLeft () const   { return paddingLeft; }
    float getPaddingRight () const  { return paddingRight; }
    float getPaddingBottom () const { return paddingBottom; }
    
    juce::Rectangle<int> getPaddedBounds (juce::Rectangle<int> b)
    {
        return juce::Rectangle<int> (b.getX() + paddingLeft,
                                     b.getY() + paddingTop,
                                     b.getWidth() - (paddingLeft+paddingRight),
                                     b.getHeight() - (paddingTop+paddingBottom));
    }

    ItemType getItemType() const { return itemType; }

    /**
     applies the size constraints to the items
     */
    void constrainBounds (juce::Rectangle<int>& bounds, bool& changedWidth, bool& changedHeight)
    {
        changedWidth  = false;
        changedHeight = false;
        if (maxWidth > 0.0 && maxWidth < bounds.getWidth()) {
            bounds.setWidth (maxWidth);
            changedWidth = true;
        }
        if (minWidth > 0.0 && minWidth > bounds.getWidth()) {
            bounds.setWidth (minWidth);
            changedWidth = true;
        }
        if (maxHeight > 0.0 && maxHeight < bounds.getHeight()) {
            bounds.setHeight (maxHeight);
            changedHeight = true;
        }
        if (minHeight > 0.0 && minHeight > bounds.getHeight()) {
            bounds.setHeight (minHeight);
            changedHeight = true;
        }
    }
    
private:
    ItemType   itemType;
    
    juce::Component::SafePointer<juce::Component> componentPtr;
    
    float stretchX;
    float stretchY;
    
    float minWidth;
    float maxWidth;
    float minHeight;
    float maxHeight;

    float paddingTop;
    float paddingLeft;
    float paddingRight;
    float paddingBottom;
};

//==============================================================================
/**
 The LabeledLayoutItem takes ownership of the label component
 */
class LabeledLayoutItem : public LayoutItem
{
public:
    LabeledLayoutItem (juce::Component* c, juce::Label* l)
      : LayoutItem (c),
        label (l) {}
    
    virtual ~LabeledLayoutItem() {};
    
    /**
     Return the created label, if there is any.
     */
    juce::Label* getLabel() override { return label; }
    
    
private:
    juce::ScopedPointer<juce::Label> label;
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
    
    Layout (Orientation o=Unknown, juce::Component* owner=nullptr);
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
    virtual LayoutItem* addComponent (juce::Component*, int idx=-1);
    
    // TODO: grid layout
    //virtual LayoutItem* addComponent (Component*, const int x, const int y);
    
    /**
     Remove a component from the layout. The LayoutItem is destructed, but the
     Component is left untouched.
     */
    void removeComponent (juce::Component*);
    
    /**
     Add a component with a label in a sub layout. By chosing the orientation the 
     placement of the label can be set. Either a pointer to a Label pointer can be 
     set to return the created label, or you can call getLabel on the returned LayoutItem.
     */
    LayoutItem* addLabeledComponent (juce::Component*, Orientation, juce::Label** labelPtr=nullptr, int idx=-1);
    

    /**
     Convenience method to ad a labeled component with a given text
     */
    LayoutItem* addLabeledComponent (juce::Component*, juce::StringRef, Orientation o=Layout::TopDown, int idx=-1);
    
    /**
     Creates a nested layout inside a layout.
     */
    Layout* addSubLayout (Orientation, int idx=-1, juce::Component* owner=nullptr);

    /**
     Creates a spacer to put space between items. Use stretch factors to increase
     the space it occupies
     */
    LayoutItem* addSSpacer (float sx=1.0, float sy=1.0, int idx=-1);
    
    /**
     Retrieve the LayoutItem for a component. If the Component is not found in the
     Layout, a nullptr is returned.
     */
    LayoutItem* getLayoutItem (juce::Component*);
    
    /**
     If the layout has an owning component, this calls updateGeometry with the
     bounds of the owning component.
     */
    virtual void updateGeometry ();
    
    /**
     Recompute the geometry of all components. Recoursively recomputes all sub layouts.
     */
    virtual void updateGeometry (juce::Rectangle<int> bounds);
    
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
    juce::OwnedArray<LayoutItem>       itemsList;
    juce::Array<juce::Rectangle<int> > itemsBounds;
    juce::Array<bool>                  itemBoundsFinal;

    bool isUpdating;
    mutable bool isCummulatingStretch;
    juce::Component::SafePointer<juce::Component> owningComponent;

};

/**
 A SubLayout can be used to add a nested layout inside a layout.
 
 @see Layout
 */
class SubLayout : public Layout, public LayoutItem
{
public:
    SubLayout (Orientation o=Unknown, juce::Component* owner=nullptr);
    virtual ~SubLayout() {};
    
    /**
     Cummulates all stretch factors inside the nested layout
     
     Along the orientation the factors are summed up. In the other
     dimension the maximum of the stretch factors is returned.
     */
    void getStretch (float& w, float& h) const override;

};



#endif  // JUCE_AK_LAYOUT_H_INCLUDED
