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

    juce_ak_layout.h
    Created: 21 Feb 2016 9:14:52pm

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
        //SplitterItem,
        //MovableSplitterItem,
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
        minWidth (-1),
        maxWidth (-1),
        minHeight (-1),
        maxHeight (-1),
        paddingTop (0),
        paddingLeft (0),
        paddingRight (0),
        paddingBottom (0)
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

    int getMinimumWidth  () const { return minWidth; }
    int getMaximumWidth  () const { return maxWidth; }
    int getMinimumHeight () const { return minHeight; }
    int getMaximumHeight () const { return maxHeight; }

    virtual void getSizeLimits (int& minW, int& maxW, int& minH, int& maxH)
    {
        if (minWidth >= 0) minW = (minW < 0) ? minWidth : juce::jmax (minW, minWidth);
        if (maxWidth >= 0) maxW = (maxW < 0) ? maxWidth : juce::jmin (maxW, maxWidth);
        if (minHeight >= 0) minH = (minH < 0) ? minHeight : juce::jmax (minH, minHeight);
        if (maxHeight >= 0) maxH = (maxH < 0) ? maxHeight : juce::jmin (maxH, maxHeight);
    }

    void setPaddingTop    (const int p) { paddingTop = p; }
    void setPaddingLeft   (const int p) { paddingLeft = p; }
    void setPaddingRight  (const int p) { paddingRight = p; }
    void setPaddingBottom (const int p) { paddingBottom = p; }
    void setPadding (const int p) {
        setPaddingTop (p);
        setPaddingLeft (p);
        setPaddingRight (p);
        setPaddingBottom (p);
    }
    int getPaddingTop () const    { return paddingTop; }
    int getPaddingLeft () const   { return paddingLeft; }
    int getPaddingRight () const  { return paddingRight; }
    int getPaddingBottom () const { return paddingBottom; }
    
    void setFixedWidth (const int w)
    {
        minWidth = w;
        maxWidth = w;
    }

    void setFixedHeight (const int h)
    {
        minHeight = h;
        maxHeight = h;
    }

    void setFixedSize (const int w, const int h)
    {
        setFixedWidth (w);
        setFixedHeight (h);
    }

    /**
     Convenience method to set all parameters in one line
     */
    void setItemParameters (float inStretchX,
                            float inStretchY,
                            int inMinWidth,
                            int inMinHeight,
                            int inMaxWidth,
                            int inMaxHeight,
                            float inPaddingTop,
                            float inPaddingLeft,
                            float inPaddingRight,
                            float inPaddingBottom)
    {
        stretchX = inStretchX;
        stretchY = inStretchY;
        minWidth  = inMinWidth;
        minHeight = inMinHeight;
        maxWidth  = inMaxWidth;
        maxHeight = inMaxHeight;
        paddingTop    = inPaddingTop;
        paddingLeft   = inPaddingLeft;
        paddingRight  = inPaddingRight;
        paddingBottom = inPaddingBottom;
    }

    ItemType getItemType() const { return itemType; }

    /**
     applies the size constraints to the items
     */
    void constrainBounds (juce::Rectangle<int>& bounds, bool& changedWidth, bool& changedHeight)
    {
        int cbMinWidth = -1;
        int cbMaxWidth = -1;
        int cbMinHeight = -1;
        int cbMaxHeight = -1;
        getSizeLimits (cbMinWidth, cbMaxWidth, cbMinHeight, cbMaxHeight);
        changedWidth  = false;
        changedHeight = false;

        if (cbMaxWidth > 0 && cbMaxWidth < bounds.getWidth()) {
            bounds.setWidth (cbMaxWidth);
            changedWidth = true;
        }
        if (cbMinWidth > 0 && cbMinWidth > bounds.getWidth()) {
            bounds.setWidth (cbMinWidth);
            changedWidth = true;
        }
        if (cbMaxHeight > 0 && cbMaxHeight < bounds.getHeight()) {
            bounds.setHeight (cbMaxHeight);
            changedHeight = true;
        }
        if (cbMinHeight > 0 && cbMinHeight > bounds.getHeight()) {
            bounds.setHeight (cbMinHeight);
            changedHeight = true;
        }
    }
    
protected:
    friend Layout;
    void setLayoutBounds (juce::Rectangle<int> b)
    {
        layoutBounds = b;
    }
    void setLayoutBounds (int x, int y, int w, int h)
    {
        layoutBounds.setBounds (x, y, w, h);
    }
    juce::Rectangle<int> getLayoutBounds() const
    {
        return layoutBounds;
    }
    juce::Rectangle<int> getPaddedLayoutBounds () const
    {
        return juce::Rectangle<int> (layoutBounds.getX() + paddingLeft,
                                     layoutBounds.getY() + paddingTop,
                                     layoutBounds.getWidth() - (paddingLeft+paddingRight),
                                     layoutBounds.getHeight() - (paddingTop+paddingBottom));
    }

    /**
     set the flag that the bounds are adapted with size limits and shall not change
     */
    void setBoundsAreFinal (bool final)
    {
        boundsAreFinal = final;
    }
    bool getBoundsAreFinal() const
    {
        return boundsAreFinal;
    }
    
private:
    ItemType   itemType;
    
    juce::Component::SafePointer<juce::Component> componentPtr;
    
    float stretchX;
    float stretchY;
    
    int minWidth;
    int maxWidth;
    int minHeight;
    int maxHeight;

    float paddingTop;
    float paddingLeft;
    float paddingRight;
    float paddingBottom;
    
    // computed values, not for setting
    juce::Rectangle<int> layoutBounds;
    bool                 boundsAreFinal;
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
        //GridLayout
    };
    
    Layout (Orientation o=Unknown, juce::Component* owner=nullptr);
    virtual ~Layout();
    
    /**
     Changes the orientation of the layout
     */
    void setOrientation (const Orientation);

    Orientation getOrientation () const { return orientation; }
    
    bool isHorizontal () const { return orientation == LeftToRight || orientation == RightToLeft; }
    bool isVertical ()   const { return orientation == TopDown || orientation == BottomUp; }

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
     Call this method in your Component::resized() callback.
     If the layout has an owning component, this calls updateGeometry with the
     bounds of the owning component.
     */
    virtual void updateGeometry ();
    
    /**
     Recompute the geometry of all components. Recoursively recomputes all sub layouts.
     */
    virtual void updateGeometry (juce::Rectangle<int> bounds);
    
    /**
     To show the layout bounds e.g. for debugging yout layout structure simply add the following line to yout Component:
     \code{.cpp}
     void Component::paintOverChildren (Graphics& g) override
     {
         myLayout.paintBounds (g);
     }
     \endcode
     */
    virtual void paintBounds (juce::Graphics& g) const;
    
    /**
     Cummulates all stretch factors inside the nested layout
     */
    void getCummulatedStretch (float& w, float& h) const;
    
protected:
    /**
     This is for internal use only. You should not need to call this method
     */
    void addRawItem (LayoutItem* item, int idx=-1);

    int getNumItems() const { return itemsList.size(); }

    LayoutItem* getItem (const int idx) { return itemsList.getUnchecked (idx); }
    const LayoutItem* getItem (const int idx) const { return itemsList.getUnchecked (idx); }

private:
    Orientation orientation;
    juce::OwnedArray<LayoutItem>       itemsList;

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

    void getSizeLimits (int& minW, int& maxW, int& minH, int& maxH) override;

};



#endif  // JUCE_AK_LAYOUT_H_INCLUDED
