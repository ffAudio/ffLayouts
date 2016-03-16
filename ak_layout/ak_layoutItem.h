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

    ak_layoutItem.h
    Created: 21 Feb 2016 9:14:52pm

  ==============================================================================
*/

#ifndef JUCE_AK_LAYOUTITEM_H_INCLUDED
#define JUCE_AK_LAYOUTITEM_H_INCLUDED


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

    LayoutItem (juce::Component* c, Layout* parent=nullptr, bool owned=false);
    
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
     If overlay is set the item will be placed over an earlier defined item. Possible values are
     \li 0: normal (default) - no overlay
     \li 1: previous item 
     \li 2: parent item
     */
    int isOverlay () const;
    void setIsOverlay (const int overlay);
    
    /**
     Overlay width and overlay height are given in relative sizes to the referenced item @see isOverlay
     */
    float getOverlayWidth () const;
    void setOverlayWidth (float w);
    float getOverlayHeight () const;
    void setOverlayHeight (float h);

    /**
     overlayJustification defines, where the item is placed inside the referenced item
     */
    int getOverlayJustification () const;
    void setOverlayJustification (int j);

    /**
     Return the managed component. Tries first the ownedComponent and then the wrappedComponent.
     */
    juce::Component* getComponent ()  const;
    
    /**
     Return the wrapped component as pointer.
     */
    juce::Component* getWrappedComponent ()  const;

    /**
     Replace the component pointer. If owned is set to true, the component will be destroyed, 
     when the item is destroyed or another component or nullptr is set. It is always only one
     pointer present, each call to setComponent removes the other variant.
     */
    void setComponent (juce::Component* ptr, bool owned=false);
    
    /**
     Return a pointer to the owned component
     */
    juce::Component* getOwnedComponent () const;
    
    /**
     Set the owned compnent. It will be removed when the layout item goes out of scope.
     This is especially usefull for adding e.g. labels you don't need to access later
     */
    void setOwnedComponent (juce::Component* c);
    
    bool isComponentItem ()     const { return itemType == ComponentItem; }
    bool isSplitterItem ()      const { return itemType == SplitterItem; }
    bool isSubLayout ()         const { return itemType == SubLayout; }

    /**
     Set the text for an automatically recreated Label as property
     */
    void setLabelText (const juce::String& text);
    
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
    virtual void getSizeLimits (int& minW, int& maxW, int& minH, int& maxH);

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
    void constrainBounds (juce::Rectangle<int>& bounds, bool& changedWidth, bool& changedHeight, bool preferVertical);

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
    
    /** Set the wrapped components componentID and the item's componentID property */
    void setWrappedComponentID (const juce::String& name, bool setComp);

    /**
     Chance for LayoutItems to fix properties that might have changed for saving
     */
    virtual void fixUpLayoutItems ();
    
    /**
     Save the layout into a ValueTree. To get proper references to the components,
     don't forget to set unique componentIDs.
     */
    virtual void saveLayoutToValueTree (juce::ValueTree& tree) const;
    
    /**
     Load the layout from a ValueTree. The component references are restored to the owning
     Component using findChildWithID()
     */
    virtual LayoutItem* loadLayoutFromValueTree (const juce::ValueTree& tree, juce::Component* owner);

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
        
        /** 
         Callback when the item is a splitter and is just moved. 
         Final is true, if the mouse button is released.
         To add a listener to a splitter from xml you can set the \p componentName or \p componentID
         to find it as child of the \p owningComponent.
         */
        virtual void layoutSplitterMoved (float relativePos, bool final) {}
        
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

    /** Call the callbacks of LayoutItem::Listeners when splitter moved
     */
    void callListenersCallback (float relativePosition, bool final);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutItem)
    
    ItemType   itemType;
    
    Layout* parentLayout;

    juce::Component::SafePointer<juce::Component>   componentPtr;
    
    juce::ScopedPointer<juce::Component>            ownedComponent;

    // computed values, not for setting
    juce::Rectangle<int> itemBounds;
    bool                 boundsAreFinal;
    
    juce::ListenerList<Listener> layoutItemListeners;
    
    static const juce::Identifier itemTypeInvalid;
    static const juce::Identifier itemTypeComponent;
    static const juce::Identifier itemTypeLabeledComponent;
    static const juce::Identifier itemTypeSplitter;
    static const juce::Identifier itemTypeSpacer;
    static const juce::Identifier itemTypeSubLayout;
    
    static const juce::Identifier propOverlay;
    static const juce::Identifier propOverlayWidth;
    static const juce::Identifier propOverlayHeight;
    static const juce::Identifier propOverlayJustification;

    static const juce::Identifier propComponentID;
    static const juce::Identifier propComponentName;
    static const juce::Identifier propLabelText;
    static const juce::Identifier propLabelFontSize;
    static const juce::Identifier propGroupName;
    static const juce::Identifier propGroupText;
    static const juce::Identifier propGroupJustification;

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

    /**
     mouse callback when slider dragging ended
     */
    void mouseUp (const juce::MouseEvent& event) override;
    
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
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutSplitter)

    static const juce::Identifier propRelativePosition;
    static const juce::Identifier propRelativeMinPosition;
    static const juce::Identifier propRelativeMaxPosition;
    static const juce::Identifier propIsHorizontal;

};




#endif  // JUCE_AK_LAYOUTITEM_H_INCLUDED
