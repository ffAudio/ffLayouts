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

    juce_ak_layoutItem.h
    Created: 21 Feb 2016 9:14:52pm

  ==============================================================================
*/

#ifndef JUCE_AK_LAYOUTITEM_H_INCLUDED
#define JUCE_AK_LAYOUTITEM_H_INCLUDED

/**
 \mainpage
 <h1>Juce Layout</h1>
 
 This JUCE module provides a simple to use layout mechanism. The items to be laid out are LayoutItems.
 Each LayoutItem can be either a sub layout, a spacer to leave space or a component item which refers to a
 juce::Component. When Layout::updateGeometry is called, the calculated bounds of the item is then
 transfered to the component via setBounds.
 @see LayoutItem
 
 <p>
 The basic usage is to add a Layout instance to your parent component where you want to lay the children in.
 This component doesn't need to be the top most component, it also works for sub components.
 Use the Constructor providing a this pointer to the component, so the overall bounds can be used.
 Also if you want to use the helper LabeledComponent it is needed that the created Label can be displayed
 on the owned component.
 @see Layout
 
 <p>
 Add a call to updateGeometry() in your component's resized() method.
 @see Layout::updateGeometry()
 
 */



#include "juce_gui_basics/juce_gui_basics.h"

class Layout;

//==============================================================================
/**
 LayoutItems are used to store links to components to layout. Also information
 like size hints and stretch factors are stored in the LayoutItems.
 
 @see Layout
 */
class LayoutItem : protected juce::ValueTree
{
public:
    enum ItemType {
        Invalid = 0,
        ComponentItem,
        LabeledComponentItem,
        SplitterItem,
        SpacerItem,
        SubLayout
        
    };
    
    LayoutItem (juce::Component* c, Layout* parent=nullptr);
    
    LayoutItem (ItemType i=Invalid, Layout* parent=nullptr);
    
    virtual ~LayoutItem();
    
    /**
     Return if the layout item has a valid type. A component item not pointing to a 
     valid component is also considered invalid
     */
    bool isValid();

    /**
     Return the containing layout
     */
    Layout* getParentLayout();
    const Layout* getParentLayout() const;

    /**
     Return the topmost layout
     */
    Layout* getRootLayout();
    const Layout* getRootLayout() const;
    
    /**
     Return the wrapped component as pointer.
     */
    juce::Component* getComponent ()  const;
    
    /**
     Replace the component pointer
     */
    void setComponent (juce::Component* ptr);

    bool isComponentItem ()     const { return itemType == ComponentItem; }
    bool isSplitterItem ()      const { return itemType == SplitterItem; }
    bool isSubLayout ()         const { return itemType == SubLayout; }
    
    /**
     If the item is a label item (sublayout group), you can access the created item.
     The default item returns a nullptr.
     */
    virtual juce::Label* getLabel () { return nullptr; }

    /**
     Return the stretch value of an item.
     */
    virtual void getStretch (float& w, float& h) const
    {
        w = getProperty ("stretchX", 1.0);
        h = getProperty ("stretchY", 1.0);
    }

    /**
     Used to set stretch factors for the wrapped component. The space is distributed
     according the sum of stretch factors.
     For sub layouts, if you want to use the cummulated stretch of the child items,
     set this to a negative value. This is the default for new created sub layouts.
     */
    virtual void setStretch (float w, float h, juce::UndoManager* undo=nullptr)
    {
        setProperty ("stretchX", w, undo);
        setProperty ("stretchY", h, undo);
    }

    /**
     Set constraints to the items size. In doubt the minimum size is used.
     Constraints work only in that direction the layout operates (horizontally/vertically).
     For a fixed size set the same value to minimum and maximum. To remove a constraint 
     set it to -1.
     */
    void setMinimumWidth  (const int w, juce::UndoManager* undo=nullptr) { setProperty ("minWidth", w, undo); }
    /** Set the maximum width @see setMinimumWidth */
    void setMaximumWidth  (const int w, juce::UndoManager* undo=nullptr) { setProperty ("maxWidth", w, undo); }
    /** Set the minimum height @see setMinimumWidth */
    void setMinimumHeight (const int h, juce::UndoManager* undo=nullptr) { setProperty ("minHeight", h, undo); }
    /** Set the maximum height @see setMinimumWidth */
    void setMaximumHeight (const int h, juce::UndoManager* undo=nullptr) { setProperty ("maxHeight", h, undo); }

    /** Returns the minimum width */
    int getMinimumWidth  () const { return getProperty ("minWidth", -1); }
    /** Returns the maximum width */
    int getMaximumWidth  () const { return getProperty ("maxWidth", -1); }
    /** Returns the minimum height */
    int getMinimumHeight () const { return getProperty ("minHeight", -1); }
    /** Returns the maximum height */
    int getMaximumHeight () const { return getProperty ("maxHeight", -1); }

    /**
     Return the size limits of the item. You can use this method to cummulate
     the limits. if you don't want that, it is your responsibility to set the
     parameters to no limit, i.e. a negative value
     */
    virtual void getSizeLimits (int& minW, int& maxW, int& minH, int& maxH)
    {
        const int minWidth = getMinimumWidth();
        const int maxWidth = getMinimumWidth();
        const int minHeight = getMinimumWidth();
        const int maxHeight = getMinimumWidth();
        if (minWidth >= 0) minW = (minW < 0) ? minWidth : juce::jmax (minW, minWidth);
        if (maxWidth >= 0) maxW = (maxW < 0) ? maxWidth : juce::jmin (maxW, maxWidth);
        if (minHeight >= 0) minH = (minH < 0) ? minHeight : juce::jmax (minH, minHeight);
        if (maxHeight >= 0) maxH = (maxH < 0) ? maxHeight : juce::jmin (maxH, maxHeight);
    }

    /** Set a padding value for the wrapped component or item to it's calculated top bounds */
    void setPaddingTop    (const int p, juce::UndoManager* undo=nullptr) { setProperty ("paddingTop", p, undo); }
    /** Set a padding value for the wrapped component or item to it's calculated left bounds */
    void setPaddingLeft   (const int p, juce::UndoManager* undo=nullptr) { setProperty ("paddingLeft", p, undo); }
    /** Set a padding value for the wrapped component or item to it's calculated right bounds */
    void setPaddingRight  (const int p, juce::UndoManager* undo=nullptr) { setProperty ("paddingRight", p, undo); }
    /** Set a padding value for the wrapped component or item to it's calculated bottom bounds */
    void setPaddingBottom (const int p, juce::UndoManager* undo=nullptr) { setProperty ("paddingBottom", p, undo); }
    
    /**
     Set one value to all bounaries
     */
    void setPadding (const int p, juce::UndoManager* undo=nullptr) {
        setPaddingTop (p, undo);
        setPaddingLeft (p, undo);
        setPaddingRight (p, undo);
        setPaddingBottom (p, undo);
    }

    /** Returns the padding value for the wrapped component or item to it's calculated top bounds */
    int getPaddingTop () const    { return getProperty ("paddingTop", 0); }
    /** Returns the padding value for the wrapped component or item to it's calculated left bounds */
    int getPaddingLeft () const   { return getProperty ("paddingLeft", 0); }
    /** Returns the padding value for the wrapped component or item to it's calculated right bounds */
    int getPaddingRight () const  { return getProperty ("paddingRight", 0); }
    /** Returns the padding value for the wrapped component or item to it's calculated bottom bounds */
    int getPaddingBottom () const { return getProperty ("paddingBottom", 0); }
    
    /** Sets fixed width as minimum width = maximum width */
    void setFixedWidth (const int w, juce::UndoManager* undo=nullptr)
    {
        setMinimumWidth (w, undo);
        setMaximumWidth (w, undo);
    }

    /** Sets fixed height as minimum height = maximum height */
    void setFixedHeight (const int h, juce::UndoManager* undo=nullptr)
    {
        setMinimumHeight (h, undo);
        setMaximumHeight (h, undo);
    }

    /** Convenience method to set fixed width and height in one call */
    void setFixedSize (const int w, const int h, juce::UndoManager* undo=nullptr)
    {
        setFixedWidth (w, undo);
        setFixedHeight (h, undo);
    }

    /**
     Set an aspect ratio to constrain the bounds
     */
    void setAspectRatio (const float ratio, juce::UndoManager* undo=nullptr)
    {
        setProperty ("aspectRatio", ratio, undo);
    }
    
    /**
     Set an aspect ratio to constrain the bounds
     */
    float getAspectRatio () const
    {
        return getProperty ("aspectRatio", 0.0);
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
                            int inPaddingTop,
                            int inPaddingLeft,
                            int inPaddingRight,
                            int inPaddingBottom,
                            float inAspectRatio,
                            juce::UndoManager* undo=nullptr)
    {
        setStretch (inStretchX, inStretchY, undo);
        setMinimumWidth (inMinWidth, undo);
        setMaximumWidth (inMaxWidth, undo);
        setMinimumHeight (inMinHeight, undo);
        setMaximumHeight (inMaxHeight, undo);
        setPaddingTop (inPaddingTop, undo);
        setPaddingLeft (inPaddingLeft, undo);
        setPaddingRight (inPaddingRight, undo);
        setPaddingBottom (inPaddingBottom, undo);
        setAspectRatio (inAspectRatio, undo);
    }

    ItemType getItemType() const { return itemType; }

    /**
     Applies the size constraints to the items.
     set preferVertical to true to adapt height of the item or false to adapt the width.
     */
    void constrainBounds (juce::Rectangle<int>& bounds, bool& changedWidth, bool& changedHeight, bool preferVertical)
    {
        int cbMinWidth = -1;
        int cbMaxWidth = -1;
        int cbMinHeight = -1;
        int cbMaxHeight = -1;
        float aspectRatio = getAspectRatio();
        
        getSizeLimits (cbMinWidth, cbMaxWidth, cbMinHeight, cbMaxHeight);
        changedWidth  = false;
        changedHeight = false;

        if (cbMaxWidth > 0 && cbMaxWidth < bounds.getWidth()) {
            bounds.setWidth (cbMaxWidth);
            changedWidth = true;
        }
        if (aspectRatio > 0.0 && !preferVertical) {
            bounds.setWidth (bounds.getHeight() * aspectRatio);
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
        if (aspectRatio > 0.0 && preferVertical) {
            bounds.setHeight (bounds.getWidth() / aspectRatio);
            changedHeight = true;
        }
        if (cbMinHeight > 0 && cbMinHeight > bounds.getHeight()) {
            bounds.setHeight (cbMinHeight);
            changedHeight = true;
        }
    }

    /**
     This property is dynamically calculated each time updateGeometry is called.
     Setting this property on the root node lets you place the layout on a fixed
     position in the owningComponent. If it is set empty (i.e. size equals 0, 0)
     which is the default, the local bounds of the owningComponent are used as
     available space.
     */
    void setItemBounds (juce::Rectangle<int> b)
    {
        itemBounds = b;
    }

    /**
     Convenience method to save creation of a rectangle struct.
     @see setItemBounds
     */
    void setItemBounds (int x, int y, int w, int h)
    {
        itemBounds.setBounds (x, y, w, h);
    }

    /**
     returns the calculated bounds of the item or layout.
     Calling this on the root node returns only a valid rectangle, if it was set
     as fixed bounds.
     */
    juce::Rectangle<int> getItemBounds() const
    {
        return itemBounds;
    }

    /**
     Returns the computed bounds reduced by the specified padding of the item
     */
    juce::Rectangle<int> getPaddedItemBounds () const
    {
        const int paddingLeft = getPaddingLeft();
        const int paddingTop = getPaddingTop();
        return juce::Rectangle<int> (itemBounds.getX() + paddingLeft,
                                     itemBounds.getY() + paddingTop,
                                     itemBounds.getWidth() - (paddingLeft+getPaddingRight()),
                                     itemBounds.getHeight() - (paddingTop+getPaddingBottom()));
    }

    /**
     set the flag that the bounds are adapted with size limits and shall not change
     This is computed on each updateGeometry and should not be set
     */
    void setBoundsAreFinal (bool final)
    {
        boundsAreFinal = final;
    }

    /**
     This is computed on each updateGeometry and should not be set.
     It means that the bounds are constrained and the update algorithm shall not change
     the item's size.
     */
    bool getBoundsAreFinal() const
    {
        return boundsAreFinal;
    }
    
    /**
     Save the layout into a ValueTree. To get proper references to the components,
     don't forget to set unique componentIDs.
     */
    virtual void saveLayoutToValueTree (juce::ValueTree& tree) const;
    
    /**
     Load the layout from a ValueTree. The component references are restored to the owning
     Component using findChildWithID()
     */
    void loadLayoutFromValueTree (const juce::ValueTree tree, juce::Component* owner);

    // =============================================================================
    
    /**
     The LayoutItem::Listener will be called, whenever the bounds of a LayoutItem are changed
     */
    class Listener {
    public:
        /** Destructor. */
        virtual ~Listener()  {}
        
        /** Callback when the layout items bounds are changed */
        virtual void layoutBoundsChanged (juce::Rectangle<int> newBounds) = 0;
        
    };
    
    /** Registers a listener to receive events when this button's state changes.
     If the listener is already registered, this will not register it again.
     @see removeListener
     */
    void addListener (Listener* newListener);
    
    /** Removes a previously-registered button listener
     @see addListener
     */
    void removeListener (Listener* listener);
    
    /** Call the callbacks of LayoutItem::Listeners
     */
    void callListenersCallback (juce::Rectangle<int> newBounds);
    
private:
    ItemType   itemType;
    
    Layout* parentLayout;

    juce::Component::SafePointer<juce::Component> componentPtr;

    // computed values, not for setting
    juce::Rectangle<int> itemBounds;
    bool                 boundsAreFinal;
    
    juce::ListenerList<Listener> layoutItemListeners;
};

#ifndef DOXYGEN
/** This typedef is just for compatibility with old code and VC6 - newer code should use LayoutItem::Listener instead. */
typedef LayoutItem::Listener LayoutItemListener;
#endif

//==============================================================================
/**
 A splitter is a handle to drag boundaries inside the layout
 */
class LayoutSplitter : public LayoutItem, public juce::Component
{
public:
    LayoutSplitter (juce::Component* owningComponent, float position, bool horizontal, Layout* parent);

    virtual ~LayoutSplitter();

    /** 
     Paint the splitter handle. You can override this to customize the drawing
     */
    void paint (juce::Graphics& g) override;

    /**
     mouse callback to drag the splitter
     */
    void mouseDrag (const juce::MouseEvent &event) override;

    /** Set the position in normalized form */
    void setRelativePosition (float position, juce::UndoManager* undo=nullptr);

    /** Return the position in normalized form */
    float getRelativePosition() const;
    
    /** Set the minimum relative position in normalized coordinates */
    void setMinimumRelativePosition (const float min, juce::UndoManager* undo=nullptr);

    /** Set the maximum relative position in normalized coordinates */
    void setMaximumRelativePosition (const float max, juce::UndoManager* undo=nullptr);
    
    /** Return the minimum normalized position */
    float getMinimumRelativePosition() const;

    /** Return the maximum normalized position */
    float getMaximumRelativePosition() const;

    /** Set the horizontal flag */
    void setIsHorizontal (bool isHorizontal, juce::UndoManager* undo=nullptr);
    
    /** Return the horizontal flag */
    bool getIsHorizontal() const;
};


//==============================================================================
/**
 The LabeledLayoutItem is a sub layout containing a LayoutItem and a Label.
 The LabeledLayoutItem takes ownership of the label component, so it is destroyed when the layout is destructed.
 */
class LabeledLayoutItem : public LayoutItem
{
public:
    LabeledLayoutItem (juce::Component* c, juce::Label* l, Layout* parent)
      : LayoutItem (c, parent),
        label (l) {}
    
    virtual ~LabeledLayoutItem() {};
    
    /**
     Return the created label, if there is any.
     */
    juce::Label* getLabel() override { return label; }
    
    
private:
    juce::ScopedPointer<juce::Label> label;
};


#endif  // JUCE_AK_LAYOUTITEM_H_INCLUDED