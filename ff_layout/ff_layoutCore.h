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

    ff_layoutCore.h
    Created: 21 Feb 2016 9:14:52pm

  ==============================================================================
*/

#pragma once


#include "ff_layoutItem.h"

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
         LayoutItem::makeComponentItem (myLayout.state, &button1);
         LayoutItem::makeComponentItem (myLayout.state, &button2);
         LayoutItem::makeComponentItem (myLayout.state, &button3);
         myLayout.realize ();
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

 You can also instanciate the layout from a xml string.
 
 @see Component
 @see LayoutItem
 */

class Layout : public LayoutItemListener
{
public:
    
    /**
     Create a layout instance defined by a xml string. The xml items are:
     \li \p Layout: A layout containing components or layouts
     \li \p Component: An item referring to a component. It is connected via the property \p componentID or \p componentName
     \li \p Spacer: An item which leaves an empty space between the other components or items
     \li \p Splitter: A movable item which squeezes the items around the splitter according to th mouse moves
     
     The items can have the following properties:
     \li \p stretchX: a factor how much proportional space the item requires in horizontal direction
     \li \p stretchY: a factor how much proportional space the item requires in vertical direction
     \li \p minWidth: the width the item shall not shrink below
     \li \p maxWidth: the maximum width the item may occupy
     \li \p minHeight: the height the item shall not shrink below
     \li \p maxHeight: the maximum height the item may occupy
     \li \p paddingTop: a space which will be left between the calculated top and the item's top edge
     \li \p paddingLeft: a space which will be left between the calculated left and the item's right edge
     \li \p paddingRight: a space which will be left between the calculated right and the item's right edge
     \li \p paddingBottom: a space which will be left between the calculated bottom and the item's bottom
     \li \p overlay: use the space of an other item instead using it's own space. 1: parent node, 2: previous node. Having no previous node or parent results in zero size
     \li \p overlayWidth: relative width inside the referenced item
     \li \p overlayHeight: relative height inside the referenced item
     \li \p overlayJustification: justification flag (as integer) where to put the overlay inside the referenced item
     
     The Component understands the additional properties:
     \li \p componentID: the componentID to connect to. All child components of the \p owningComponent are searched
     \li \p componentName: the componentName to connect to. All child components of the \p owningComponent are searched
     \li \p labelText: if a text is provided, a label owned by the layout is created to display the text
     \li \p labelFontSize: if a \p labelText is present, this font size is used for the label
     \li \p labelJustification: justification flag (as integer) how to align the text in the label
     
     The Splitter understands the following proberties:
     \li \p relativePosition: The position in normalized form where the splitter is initially set
     \li \p relativeMaxPosition: the maximum normalized position to where the splitter can be moved
     \li \p relativeMinPosition: the minimum normalized position to where the splitter can be moved
     
     The Layout has the following properties:
     \li \p orientation: the direction in which the items are laid out. Possible values are: \p leftToRight, \p topDown, \p rightToLeft and \p bottomUp
     \li \p layoutBounds: this has only an effect in the root layout, so the layout can be set at fixed positions
     \li \p groupName: add a GroupComponent around the sub-layout
     \li \p groupText: add a GroupComponent around the sub-layout with the given text
     \li \p groupJustification: specifies the position of the groupText. Add the flags to one integer value
     
     Additionally the root node may contain
     \li \p resizable: set this to 1 to add a resizer to the component the layout manages
     \li \p resizerWidth: the width of the resizer handle
     \li \p resizerHeight: the height of the resizer handle
     \li \p minWidth: the width the component shall not shrink below
     \li \p maxWidth: the maximum width the component may occupy
     \li \p minHeight: the height the component shall not shrink below
     \li \p maxHeight: the maximum height the component may occupy
     
     */
    
    Layout (LayoutItem::Orientation o, juce::Component* owner=nullptr);
    Layout (const juce::String& xml, juce::Component* owner=nullptr);
    Layout (const juce::ValueTree& state, juce::Component* owner=nullptr);

    ~Layout ();
    
    /**
     return the owning component, if it's set
     */
    juce::Component* getOwningComponent();
    const juce::Component* getOwningComponent() const;
    
    /**
     Remove a component from the layout. The LayoutItem is destructed, but the
     Component is left untouched.
     */
    void removeComponent (juce::Component* component);

    /**
     Retrieve the LayoutItem for a component. If the Component is not found in the
     Layout, an invalid ValueTree node is returned.
     */
    juce::ValueTree getLayoutItem (juce::Component*);
    
    /**
     Call this to connect a fresh state to the owningComponent. In this step layout defined 
     components like the splitter component and text labels are created and the items in the 
     hierarchy are hooked up to children of owningComponent identified by componentID or 
     componentName.
     */
    void realize (juce::Component* owningComponent=nullptr);
    
    /**
     Call this method in your Component::resized() callback.
     If the layout has an owning component, this calls updateGeometry with the
     bounds of the owning component.
     */
    void updateGeometry ();

    /**
     To show the layout bounds e.g. for debugging yout layout structure simply add the following line to yout Component:
     \code{.cpp}
     void Component::paintOverChildren (Graphics& g) override
     {
         myLayout.paintBounds (g);
     }
     \endcode
     */
    void paintBounds (juce::Graphics& g) const;

    /** Clears the layout and resets to zero state */
    void clearLayout (juce::UndoManager* undo=nullptr);
    
    /** 
     Set a ValueTree for keeping track of current positions. The layout will create a new lode inside the given tree.
     */
    void setSettingsTree (juce::ValueTree settings);

    /** This is the callback to track resizer settings */
    void layoutBoundsChanged (juce::ValueTree item, juce::Rectangle< int > newBounds) override;

    /** This is the callback to track resizer settings */
    void layoutSplitterMoved (juce::ValueTree item, float relativePos, bool final) override;
    
    /** Use the state to identify nodes in the hierarchy where to add layout items */
    juce::ValueTree state;
    
    
    static const juce::Identifier propResizable;
    static const juce::Identifier propResizerWidth;
    static const juce::Identifier propResizerHeight;
    static const juce::Identifier propMinWidth;
    static const juce::Identifier propMaxWidth;
    static const juce::Identifier propMinHeight;
    static const juce::Identifier propMaxHeight;

    static const juce::Identifier propAspectRatio;

    static const juce::Identifier settingsType;
    static const juce::Identifier settingsPositionX;
    static const juce::Identifier settingsPositionY;
    static const juce::Identifier settingsWidth;
    static const juce::Identifier settingsHeight;
    static const juce::Identifier settingsSplittersList;
    static const juce::Identifier settingsSplitterPos;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Layout)
    
    juce::WeakReference<Layout>::Master masterReference;
    friend class juce::WeakReference<Layout>;
    
    juce::Component::SafePointer<juce::Component> owningComponent;
    
    /**
     This tree keeps track of resize bounds, splitter positions et al. You can
     use it for saving and restoring the positions
     */
    juce::ValueTree currentSettings;
    
    std::unique_ptr<juce::ResizableCornerComponent>     resizer;
    std::unique_ptr<juce::ComponentBoundsConstrainer>   resizeConstraints;


};
