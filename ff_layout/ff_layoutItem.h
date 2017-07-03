/*
  ==============================================================================

  Copyright (c) 2016 - 2017, Daniel Walz
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

    ff_layoutItem.h
    Created: 21 Feb 2016 9:14:52pm

  ==============================================================================
*/

#pragma once


#include "juce_gui_basics/juce_gui_basics.h"

class Layout;
class LayoutSplitter;

//==============================================================================
/**
 LayoutItems are used to store links to components to layout. Also information
 like size hints and stretch factors are stored in the LayoutItems. To create a new 
 node in the layout use the static makeChildComponent and similar methods.
 To interact with an item simply create one on the stack from it's state in the
 ValueTree, e.g.:
 \code{.cpp}
 LayoutItem item (myLayout.state);
 item.setOrientation (TopDown);
 \endcode
 
 @see makeSubLayout, makeChildComponent, makeChildSplitter, makeChildSpacer
 */
class LayoutItem
{

public:
    //==============================================================================
    /**
     The LayoutItem::Listener will be called, whenever the bounds of a LayoutItem are changed
     */
    class Listener {
    public:
        /** Destructor. */
        virtual ~Listener()  {}
        
        /** Callback when the layout items bounds are changed */
        virtual void layoutBoundsChanged (juce::ValueTree item, juce::Rectangle<int> newBounds) = 0;
        
        /**
         Callback when the item is a splitter and is just moved.
         Final is true, if the mouse button is released.
         To add a listener to a splitter from xml you can set the \p componentName or \p componentID
         to find it as child of the \p owningComponent.
         */
        virtual void layoutSplitterMoved (juce::ValueTree item, float relativePos, bool final) {}
        
    };

private:
    //==============================================================================
    /**
     Internal data shared over ValueTree instances
     */
    class SharedLayoutData : public juce::ReferenceCountedObject
    {
        
    private:
        friend LayoutItem;
        
        /** @internal */
        const juce::Component* getComponent() const;
        
        /** @internal */
        juce::Component* getComponent();
        
        /** @internal */
        void setComponent (juce::Component* c, bool owned=false);
        
        /** @internal */
        bool hasComponent () const;
        
        /** @internal */
        void addLayoutListener (LayoutItem::Listener* l);
        
        /** @internal */
        void removeLayoutListener (LayoutItem::Listener* l);
        
        /** @internal */
        void removeAllListeners ();
        
        /** @internal */
        void callListenersCallback (juce::ValueTree item, juce::Rectangle<int> newBounds);

        /** @internal */
        void callListenersCallback (juce::ValueTree item, float relativePosition, bool final);
        
    private:
        juce::Component::SafePointer<juce::Component>   componentPtr;
        
        juce::ScopedPointer<juce::Component>            ownedComponent;
        
        juce::ListenerList<Listener> layoutItemListeners;
    };


public:
    enum ItemType {
        Invalid = 0,
        ComponentItem,
        LabeledComponentItem,
        SplitterItem,
        SpacerItem,
        LineItem,
        SubLayout
    };
    
    enum Orientation {
        Unknown = 0,
        LeftToRight,
        TopDown,
        RightToLeft,
        BottomUp,
        //GridLayout
    };
    
    /**
     Create an empty layout as root node. Is created in the Layout constructor.
     */
    LayoutItem (LayoutItem::Orientation o);
    
    /**
     Instanciate a LayoutItem from it's state. To create a new item use the static methods
     @see make
     */
    LayoutItem (juce::ValueTree state);
        
    virtual ~LayoutItem();
    
    /**
     Return if the layout item has a valid type. A component item not pointing to a 
     valid component is also considered invalid
     */
    bool isValid();
    
    /**
     Changes the orientation of the layout
     */
    void setOrientation (const Orientation, juce::UndoManager* undo=nullptr);

    /**
     Returns the orientation including direction. If you are only intersted if horizontal or vertical
     @see isHorizontal and @see isVertival
     */
    Orientation getOrientation () const;
    
    static Orientation getOrientationFromName (juce::Identifier name);
    
    static juce::Identifier getNameFromOrientation (LayoutItem::Orientation o);
    
    /** Returns true if orientation is leftToRight or rightToLeft */
    bool isHorizontal () const;
    
    /** Returns true if orientation is topDown or BottomUp */
    bool isVertical ()   const;
    
    /**
     If overlay is set the item will be placed over an earlier defined item. Possible values are
     \li 0: normal (default) - no overlay
     \li 1: previous item 
     \li 2: parent item
     */
    int isOverlay () const;
    void setIsOverlay (const int overlay);
    
    /** Overlay width and overlay height are given in relative sizes to the referenced item @see isOverlay */
    float getOverlayWidth () const;
    /** Overlay width and overlay height are given in relative sizes to the referenced item @see isOverlay */
    void setOverlayWidth (float w);
    /** Overlay width and overlay height are given in relative sizes to the referenced item @see isOverlay */
    float getOverlayHeight () const;
    /** Overlay width and overlay height are given in relative sizes to the referenced item @see isOverlay */
    void setOverlayHeight (float h);

    /**
     overlayJustification defines, where the item is placed inside the referenced item
     */
    int getOverlayJustification () const;
    /**
     overlayJustification defines, where the item is placed inside the referenced item
     */
    void setOverlayJustification (int j);

    /**
     Return the managed component. Tries first the ownedComponent and then the wrappedComponent.
     */
    juce::Component* getComponent ()  const;
    
    /**
     Return the componentID. This is taken from the properties, not the actual component
     */
    juce::String getComponentID () const;
    
    /**
     Returns true, if a component is set and available as pointer
     */
    bool hasComponent () const;
    
    /**
     Return the wrapped component as pointer.
     */
    juce::Component* getWrappedComponent ()  const;
    
    /**
     Replace the component pointer. If owned is set to true, the component will be destroyed, 
     when the item is destroyed or another component or nullptr is set. It is always only one
     pointer present, each call to setComponent removes the other variant.
     */
    void setComponent (juce::Component* ptr, bool owned=false, juce::UndoManager* undo=nullptr);

    
    /**
     Get or create a shared layout data blob for state node. @see SharedLayoutData
     */
    static LayoutItem::SharedLayoutData* getOrCreateData (juce::ValueTree& node, juce::UndoManager* undo=nullptr);

    /**
     Get or create a shared layout data blob. @see SharedLayoutData
     */
    LayoutItem::SharedLayoutData* getOrCreateData (juce::UndoManager* undo=nullptr);
    
    
    bool isSpacerItem ()        const { return state.getType() == itemTypeSpacer; }
    bool isComponentItem ()     const { return state.getType() == itemTypeComponent; }
    bool isSplitterItem ()      const { return state.getType() == itemTypeSplitter; }
    bool isSubLayout ()         const { return state.getType() == itemTypeSubLayout; }
    
    /**
     Set the text for an automatically recreated Label as property
     */
    void setLabelText (const juce::String& text, juce::UndoManager* undo=nullptr);
    
    /**
     Return the stretch value of an item.
     */
    void getStretch (float& w, float& h) const;

    /**
     Used to set stretch factors for the wrapped component. The space is distributed
     according the sum of stretch factors.
     For sub layouts, if you want to use the cummulated stretch of the child items,
     set this to a negative value. This is the default for new created sub layouts.
     */
    void setStretch (float w, float h, juce::UndoManager* undo=nullptr);

    /**
     Set constraints to the items size. In doubt the minimum size is used.
     Constraints work only in that direction the layout operates (horizontally/vertically).
     For a fixed size set the same value to minimum and maximum. To remove a constraint 
     set it to -1.
     */
    void setMinimumWidth  (const int w, juce::UndoManager* undo=nullptr);
    /** Set the maximum width @see setMinimumWidth */
    void setMaximumWidth  (const int w, juce::UndoManager* undo=nullptr);
    /** Set the minimum height @see setMinimumWidth */
    void setMinimumHeight (const int h, juce::UndoManager* undo=nullptr);
    /** Set the maximum height @see setMinimumWidth */
    void setMaximumHeight (const int h, juce::UndoManager* undo=nullptr);

    /** Returns the minimum width */
    int getMinimumWidth  () const;
    /** Returns the maximum width */
    int getMaximumWidth  () const;
    /** Returns the minimum height */
    int getMinimumHeight () const;
    /** Returns the maximum height */
    int getMaximumHeight () const;

    /**
     Return the size limits of the item. You can use this method to cummulate
     the limits. if you don't want that, it is your responsibility to set the
     parameters to no limit (i.e. a negative value) before calling this method
     */
    virtual void getSizeLimits (int& minW, int& maxW, int& minH, int& maxH);

    /** Set a padding value for the wrapped component or item to it's calculated top bounds */
    void setPaddingTop    (const int p, juce::UndoManager* undo=nullptr);
    /** Set a padding value for the wrapped component or item to it's calculated left bounds */
    void setPaddingLeft   (const int p, juce::UndoManager* undo=nullptr);
    /** Set a padding value for the wrapped component or item to it's calculated right bounds */
    void setPaddingRight  (const int p, juce::UndoManager* undo=nullptr);
    /** Set a padding value for the wrapped component or item to it's calculated bottom bounds */
    void setPaddingBottom (const int p, juce::UndoManager* undo=nullptr);
    
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
    int getPaddingTop () const;
    /** Returns the padding value for the wrapped component or item to it's calculated left bounds */
    int getPaddingLeft () const;
    /** Returns the padding value for the wrapped component or item to it's calculated right bounds */
    int getPaddingRight () const;
    /** Returns the padding value for the wrapped component or item to it's calculated bottom bounds */
    int getPaddingBottom () const;
    
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
    void setAspectRatio (const float ratio, juce::UndoManager* undo=nullptr);
    
    /**
     Set an aspect ratio to constrain the bounds
     */
    float getAspectRatio () const;

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

    /**
     create a sub layout under the node parent with the given orientation.
     @param parent the node in the hierarchical layout structure
     @param orientation of the child nodes
     @param idx the position in the sequence of already present nodes. If -1 the item is appended at the end
     @param undo the UndoManager for the ValueTree, so changes can be rolled back
     */
    static LayoutItem makeSubLayout (juce::ValueTree& parent, Orientation orientation, int idx=-1, juce::UndoManager* undo=nullptr);

    /**
     create a component container under the node parent.
     @param parent the node in the hierarchical layout structure
     @param component to be sized and moved by the layout
     @param owned if set to true the layout takes ownership of the component and will destroy it when the layout or this specific node is destroyed
     @param idx the position in the sequence of already present nodes. If -1 the item is appended at the end
     @param undo the UndoManager for the ValueTree, so changes can be rolled back
     */
    static LayoutItem makeChildComponent (juce::ValueTree& parent, juce::Component* component, bool owned=false, int idx=-1, juce::UndoManager* undo=nullptr);

    /**
     create a sub layout under the node parent with a child component and a label component.
     @param parent the node in the hierarchical layout structure
     @param component to be sized and moved by the layout
     @param text to be displayed beneath the component
     @param o is the orientation to define, on what side to put the text
     @param owned if set to true the layout takes ownership of the component and will destroy it when the layout or this specific node is destroyed
     @param idx the position in the sequence of already present nodes. If -1 the item is appended at the end
     @param undo the UndoManager for the ValueTree, so changes can be rolled back
     */
    static LayoutItem makeLabeledChildComponent (juce::ValueTree& parent, juce::Component* component, juce::String text,
                                                 LayoutItem::Orientation o=LayoutItem::TopDown, bool owned=false, int idx=-1, juce::UndoManager* undo=nullptr);
    
    /**
     create a splitter under the node parent. The splitter has an owned component to be dragged around.
     @param parent the node in the hierarchical layout structure
     @param position is a relative position where the splitter is placed initially
     @param idx the position in the sequence of already present nodes. If -1 the item is appended at the end
     @param undo the UndoManager for the ValueTree, so changes can be rolled back
     */
    static LayoutSplitter makeChildSplitter (juce::ValueTree& parent, float position, int idx=-1, juce::UndoManager* undo=nullptr);

    /**
     create a spacer under the node parent. It simply leaves out space.
     @param parent the node in the hierarchical layout structure
     @param stretchX is a multiplier how many space the spacer wants to reserve in horizontal direction
     @param stretchY is a multiplier how many space the spacer wants to reserve in vertical direction
     @param idx the position in the sequence of already present nodes. If -1 the item is appended at the end
     @param undo the UndoManager for the ValueTree, so changes can be rolled back
     */
    static LayoutItem makeChildSpacer (juce::ValueTree& parent, float stretchX=1.0, float stretchY=1.0, int idx=-1, juce::UndoManager* undo=nullptr);
    
    /**
     Remove a component item from a specific layout level
     */
    static void removeComponent (juce::ValueTree& parent, juce::Component* c, juce::UndoManager* undo=nullptr);
    
    /**
     Retrieve the LayoutItem for a component. If the Component is not found in the
     Layout, an invalid ValueTree node is returned.
     */
    static juce::ValueTree getLayoutItem (juce::ValueTree& node, juce::Component*);

    /**
     Retrieve the LayoutItem for a component by componentID. If the Component is 
     not found in the Layout, an invalid ValueTree node is returned.
     */
    static juce::ValueTree getLayoutItem (juce::ValueTree& node, juce::String& componentID);

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
    void setItemBounds (juce::Rectangle<int> b);

    /**
     Convenience method to save creation of a rectangle struct.
     @see setItemBounds
     */
    void setItemBounds (int x, int y, int w, int h);

    /**
     returns the calculated bounds of the item or layout.
     Calling this on the root node returns only a valid rectangle, if it was set
     as fixed bounds.
     */
    juce::Rectangle<int> getItemBounds() const;

    /**
     Returns the computed bounds reduced by the specified padding of the item
     */
    juce::Rectangle<int> getPaddedItemBounds () const;

    /**
     set the flag that the bounds are adapted with size limits and shall not change
     This is computed on each updateGeometry and should not be set
     */
    void setBoundsAreFinal (bool final);

    /**
     This is computed on each updateGeometry and should not be set.
     It means that the bounds are constrained and the update algorithm shall not change
     the item's size.
     */
    bool getBoundsAreFinal() const;
    
    /** Set the wrapped components componentID and the item's componentID property */
    void setWrappedComponentID (const juce::String& name, bool setComp);

    /**
     paint the bounds of the item and sub items for debugging
     */
    static void paintBounds (const juce::ValueTree& node, juce::Graphics& g);
    
    /**
     Chance for LayoutItems to fix properties that might have changed for saving
     */
    virtual void fixUpLayoutItems ();
    
    /**
     Hook to component or create an owned component
     */
    virtual void realize (juce::ValueTree& node, juce::Component* owningComponent, Layout* layout);
    
    /**
     Recompute the geometry of all components. Recoursively recomputes all sub layouts.
     */
    static int updateGeometry (juce::ValueTree& node, juce::Rectangle<int> bounds);
    
    /**
     Recompute the geometry of all components. Recoursively recomputes all sub layouts.
     */
    static int updateGeometry (juce::ValueTree& node, juce::Rectangle<int> bounds, int start, int end);
    
    /**
     Cummulates all stretch factors inside the nested layout
     */
    static void getStretch (const juce::ValueTree& node, float& w, float& h, int start=0, int end=-1);

    /**
     Compute size limits over all child nodes
     */
    static void getSizeLimits (const juce::ValueTree& node, int& minW, int& maxW, int& minH, int& maxH, int start, int end);
    
    // =============================================================================
    
    
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

    // =============================================================================

    /**
     The layout item's node in the ValueTree. You can use this to e.g. specify a 
     position in the graph where to hang child nodes into. To manipulate rather use 
     the methods implemented in LayoutItem
     */
    juce::ValueTree state;


    static const juce::Identifier itemTypeInvalid;
    static const juce::Identifier itemTypeComponent;
    static const juce::Identifier itemTypeBuilder;
    static const juce::Identifier itemTypeLabeledComponent;
    static const juce::Identifier itemTypeSplitter;
    static const juce::Identifier itemTypeSpacer;
    static const juce::Identifier itemTypeLine;
    static const juce::Identifier itemTypeSubLayout;
    
    static const juce::Identifier orientationUnknown;
    static const juce::Identifier orientationLeftToRight;
    static const juce::Identifier orientationTopDown;
    static const juce::Identifier orientationRightToLeft;
    static const juce::Identifier orientationBottomUp;

    static const juce::Identifier propStretchX;
    static const juce::Identifier propStretchY;
    static const juce::Identifier propMinWidth;
    static const juce::Identifier propMaxWidth;
    static const juce::Identifier propMinHeight;
    static const juce::Identifier propMaxHeight;
    static const juce::Identifier propAspectRatio;
    static const juce::Identifier propPaddingTop;
    static const juce::Identifier propPaddingLeft;
    static const juce::Identifier propPaddingRight;
    static const juce::Identifier propPaddingBottom;

    static const juce::Identifier propOrientation;
    static const juce::Identifier propLayoutBounds;
    static const juce::Identifier propOverlay;
    static const juce::Identifier propOverlayWidth;
    static const juce::Identifier propOverlayHeight;
    static const juce::Identifier propOverlayJustification;

    static const juce::Identifier propComponentID;
    static const juce::Identifier propComponentName;
    static const juce::Identifier propLabelText;
    static const juce::Identifier propLabelFontSize;
    static const juce::Identifier propLabelJustification;
    static const juce::Identifier propGroupName;
    static const juce::Identifier propGroupText;
    static const juce::Identifier propGroupJustification;
    
private:
    JUCE_LEAK_DETECTOR (LayoutItem)

    static const juce::Identifier volatileSharedLayoutData;
    static const juce::Identifier volatileItemBounds;
    static const juce::Identifier volatileItemBoundsFixed;
    static const juce::Identifier volatileIsUpdating;
    

};

#ifndef DOXYGEN
/** This typedef is just for compatibility with old code and VC6 - newer code should use LayoutItem::Listener instead. */
typedef LayoutItem::Listener LayoutItemListener;
#endif

//==============================================================================
/**
 A splitter is a handle to drag boundaries inside the layout
 */
class LayoutSplitter : public LayoutItem
{
public:
    LayoutSplitter (juce::ValueTree& node);

    virtual ~LayoutSplitter();
    
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

    /** Return, if parent is horizontal */
    bool isHorizontal () const;
    
    /** Set the bounds of the splitters component */
    void setBounds (juce::Rectangle<int> b);
    
    /** 
     Splitter component for mouse interaction 
     */
    class Component : public juce::Component {
    public:
        /**
         Constructor with a reference to the layout
         */
        Component (Layout* layout);
        
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
        
    private:
        juce::WeakReference<Layout> layoutPtr;
    };

    static const juce::Identifier propRelativePosition;
    static const juce::Identifier propRelativeMinPosition;
    static const juce::Identifier propRelativeMaxPosition;
    
private:
    JUCE_LEAK_DETECTOR (LayoutSplitter)

};
