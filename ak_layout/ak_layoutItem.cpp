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
 
 juce_ak_layoutItem.cpp
 Created: 12 Mar 2016 13:14:52pm
 
 ==============================================================================
 */


#include "ak_layout.h"

const juce::Identifier LayoutItem::itemTypeInvalid            ("Invalid");
const juce::Identifier LayoutItem::itemTypeComponent          ("Component");
const juce::Identifier LayoutItem::itemTypeLabeledComponent   ("LabeledComponent");
const juce::Identifier LayoutItem::itemTypeSplitter           ("Splitter");
const juce::Identifier LayoutItem::itemTypeSpacer             ("Spacer");
const juce::Identifier LayoutItem::itemTypeSubLayout          ("Layout");

const juce::Identifier LayoutItem::propOverlay              ("overlay");
const juce::Identifier LayoutItem::propOverlayWidth         ("overlayWidth");
const juce::Identifier LayoutItem::propOverlayHeight        ("overlayHeight");
const juce::Identifier LayoutItem::propOverlayJustification ("overlayJusification");

const juce::Identifier LayoutItem::propComponentID            ("componentID");
const juce::Identifier LayoutItem::propComponentName          ("componentName");
const juce::Identifier LayoutItem::propLabelText              ("labelText");
const juce::Identifier LayoutItem::propLabelFontSize          ("labelFontSize");
const juce::Identifier LayoutItem::propGroupName              ("groupName");
const juce::Identifier LayoutItem::propGroupText              ("groupText");
const juce::Identifier LayoutItem::propGroupJustification     ("groupJustification");


LayoutItem::LayoutItem (juce::Component* c, Layout* parent, bool owned)
  : juce::ValueTree (itemTypeComponent),
    itemType (ComponentItem),
    parentLayout (parent)
{
    jassert (c);
    setComponent (c, owned);
    if (!c->getComponentID().isEmpty()) {
        setProperty (propComponentID, c->getComponentID(), nullptr);
    }
}

LayoutItem::LayoutItem (ItemType i, Layout* parent)
  : juce::ValueTree ((i==ComponentItem) ? itemTypeComponent :
                     (i==LabeledComponentItem) ? itemTypeLabeledComponent :
                     (i==SplitterItem) ? itemTypeSplitter :
                     (i==SpacerItem) ? itemTypeSpacer :
                     (i==SubLayout) ? itemTypeSubLayout : itemTypeInvalid),
    itemType (i),
    parentLayout (parent)
{}

LayoutItem::~LayoutItem()
{
}


bool LayoutItem::isValid()
{
    if (itemType == Invalid) {
        return false;
    }
    if (itemType == ComponentItem && componentPtr == nullptr && ownedComponent == nullptr) {
        return false;
    }
    return true;
}

Layout* LayoutItem::getParentLayout()
{
    return parentLayout;
}

const Layout* LayoutItem::getParentLayout() const
{
    return parentLayout;
}

Layout* LayoutItem::getRootLayout()
{
    Layout* p = parentLayout;
    while (p && p->getParentLayout()) {
        p = p->getParentLayout();
    }
    return p;
}

const Layout* LayoutItem::getRootLayout() const
{
    Layout* p = parentLayout;
    while (p && p->getParentLayout()) {
        p = p->getParentLayout();
    }
    return p;
}

int LayoutItem::isOverlay () const
{
    return getProperty (propOverlay, 0);
}

void LayoutItem::setIsOverlay (const int overlay)
{
    setProperty (propOverlay, overlay, nullptr);
}

float LayoutItem::getOverlayWidth () const
{
    return getProperty (propOverlayWidth, 1.0);
}
void LayoutItem::setOverlayWidth (float w)
{
    setProperty (propOverlayWidth, w, nullptr);
}
float LayoutItem::getOverlayHeight () const
{
    return getProperty (propOverlayHeight, 1.0);
}
void LayoutItem::setOverlayHeight (float h)
{
    setProperty (propOverlayHeight, h, nullptr);
}

int LayoutItem::getOverlayJustification () const
{
    return getProperty (propOverlayJustification, 36);
}
void LayoutItem::setOverlayJustification (int j)
{
    setProperty (propOverlayJustification, j, nullptr);
}

juce::Component* LayoutItem::getComponent () const
{
    if (ownedComponent) {
        return ownedComponent.get();
    }
    return componentPtr.getComponent();
}

juce::Component* LayoutItem::getWrappedComponent () const
{
    return componentPtr.getComponent();
}

void LayoutItem::setComponent (juce::Component* ptr, bool owned)
{
    if (owned) {
        ownedComponent = ptr;
        componentPtr = nullptr;
    }
    else {
        componentPtr = ptr;
        ownedComponent = nullptr;
    }

    if (ptr->getComponentID().isEmpty()) {
        removeProperty (propComponentID, nullptr);
    }
    else {
        setProperty (propComponentID, ptr->getComponentID(), nullptr);
    }
}

juce::Component* LayoutItem::getOwnedComponent () const
{
    return ownedComponent.get();
}

void LayoutItem::setOwnedComponent (juce::Component* c)
{
    ownedComponent = c;
}

void LayoutItem::setLabelText (const juce::String& text)
{
    setProperty (propLabelText, text, nullptr);
}

void LayoutItem::getSizeLimits (int& minW, int& maxW, int& minH, int& maxH)
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

void LayoutItem::constrainBounds (juce::Rectangle<int>& bounds, bool& changedWidth, bool& changedHeight, bool preferVertical)
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

void LayoutItem::setWrappedComponentID (const juce::String& name, bool setComp)
{
    if (setComp && getComponent()) {
        getComponent()->setComponentID (name);
    }
    if (name.isEmpty()) {
        removeProperty(propComponentID, nullptr);
    }
    else {
        setProperty (propComponentID, name, nullptr);
    }
}


void LayoutItem::fixUpLayoutItems ()
{
    if (componentPtr) {
        setWrappedComponentID (componentPtr->getComponentID(), false);
    }
}

void LayoutItem::saveLayoutToValueTree (juce::ValueTree& tree) const
{
    tree = juce::ValueTree (getType());
    tree.copyPropertiesFrom (*this, nullptr);
}

LayoutItem* LayoutItem::loadLayoutFromValueTree (const juce::ValueTree& tree, juce::Component* owner)
{
    copyPropertiesFrom (tree, nullptr);

    if (isSubLayout()) {
        Layout* layout = dynamic_cast<Layout*>(this);
        for (int i=0; i<tree.getNumChildren(); ++i) {
            juce::ValueTree child = tree.getChild (i);
            LayoutItem* item = nullptr;
            if (child.getType() == itemTypeComponent) {
                if (child.hasProperty (propLabelText)) {
                    juce::String labelText = child.getProperty (propLabelText);
                    juce::Label* label = new juce::Label (juce::String::empty, labelText);
                    label->setJustificationType (juce::Justification::centred);
                    if (child.hasProperty(propLabelFontSize)) {
                        label->setFont(juce::Font (child.getProperty (propLabelFontSize)));
                    }
                    item = layout->addComponent (label, true);
                    if (owner) {
                        owner->addAndMakeVisible (item->getComponent());
                    }
                }
                else if (child.hasProperty (propComponentID)) {
                    juce::String componentID = child.getProperty (propComponentID);
                    if (juce::Component* component = owner->findChildWithID (componentID)) {
                        item = layout->addComponent (component);
                    }
                }
                else if (child.hasProperty (propComponentName)) {
                    juce::String componentName = child.getProperty (propComponentName);
                    for (int k=0; k<owner->getNumChildComponents(); ++k) {
                        juce::Component* component = owner->getChildComponent (k);
                        if (component->getName() == componentName) {
                            item = layout->addComponent (component);
                            break;
                        }
                    }
                }
            }
            else if (child.getType() == itemTypeSpacer) {
                item = layout->addSpacer();
            }
            else if (child.getType() == itemTypeSplitter) {
                // property will be replaced automatically
                LayoutSplitter* splitter = layout->addSplitterItem (0.5);
                if (child.hasProperty (propComponentID)) {
                    splitter->setComponentID (child.getProperty (propComponentID));
                }
                if (child.hasProperty (propComponentName)) {
                    splitter->setName (child.getProperty (propComponentName));
                }
                item = splitter;
            }
            else if (child.getType() == itemTypeSubLayout) {
                Layout* subLayout = layout->addSubLayout (Layout::LeftToRight);
                item = subLayout->loadLayoutFromValueTree(child, owner);
                
                // add a group around if provided
                if (child.hasProperty (propGroupName) || child.hasProperty (propGroupText)) {
                    juce::String groupName = child.getProperty (propGroupName, juce::String::empty);
                    juce::String groupText = child.getProperty (propGroupText, juce::String::empty);
                    if (owner) {
                        juce::GroupComponent* groupComponent = new juce::GroupComponent (groupName, groupText);
                        if (child.hasProperty (propGroupJustification)) {
                            juce::Justification j (child.getProperty (propGroupJustification, 1));
                            groupComponent->setTextLabelPosition (j);
                        }
                        subLayout->setComponent(groupComponent, true);
                        owner->addAndMakeVisible (groupComponent);
                    }
                }
            }
            
            if (item) {
                item->copyPropertiesFrom (child, nullptr);
            }
        }
    }
    
    return this;
}


// =============================================================================
void LayoutItem::addListener (LayoutItemListener* const newListener)
{
    layoutItemListeners.add (newListener);
}

void LayoutItem::removeListener (LayoutItemListener* const listener)
{
    layoutItemListeners.remove (listener);
}

void LayoutItem::callListenersCallback (juce::Rectangle<int> newBounds)
{
    layoutItemListeners.call(&LayoutItemListener::layoutBoundsChanged, newBounds);
}

void LayoutItem::callListenersCallback (float relativePosition, bool final)
{
    layoutItemListeners.call(&LayoutItemListener::layoutSplitterMoved, relativePosition, final);
}

//==============================================================================

const juce::Identifier LayoutSplitter::propRelativePosition       ("relativePosition");
const juce::Identifier LayoutSplitter::propRelativeMinPosition    ("relativeMinPosition");
const juce::Identifier LayoutSplitter::propRelativeMaxPosition    ("relativeMaxPosition");
const juce::Identifier LayoutSplitter::propIsHorizontal           ("isHorizontal");


LayoutSplitter::LayoutSplitter (juce::Component* owningComponent, float position, bool horizontal, Layout* parent)
:  LayoutItem(Layout::SplitterItem, parent)
{
    setComponent (this);
    setIsHorizontal (horizontal);
    if (horizontal) {
        setMouseCursor (juce::MouseCursor::LeftRightResizeCursor);
        setFixedWidth (3);
    }
    else {
        setMouseCursor (juce::MouseCursor::UpDownResizeCursor);
        setFixedHeight (3);
    }
}

LayoutSplitter::~LayoutSplitter()
{
}

void LayoutSplitter::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::grey);
}

void LayoutSplitter::mouseDrag (const juce::MouseEvent &event)
{
    if (Component* c = getParentComponent()) {
        float pos;
        bool horizontal = true;
        juce::Rectangle<int> layoutBounds (c->getLocalBounds());
        if (Layout* p = getParentLayout()) {
            horizontal = p->isHorizontal();
            if (!p->getItemBounds().isEmpty()) {
                layoutBounds = p->getItemBounds();
            }
        }
        if (horizontal) {
            pos = (event.getEventRelativeTo(c).position.getX() - layoutBounds.getX()) / layoutBounds.getWidth();
        }
        else {
            pos = (event.getEventRelativeTo(c).position.getY() - layoutBounds.getY()) / layoutBounds.getHeight();
        }
        setRelativePosition (juce::jmax (getMinimumRelativePosition(), juce::jmin (getMaximumRelativePosition(), pos)));
    }
    if (Layout* rootLayout = getRootLayout()) {
        rootLayout->updateGeometry();
    }
    
    callListenersCallback (getRelativePosition(), false);
}

void LayoutSplitter::mouseUp (const juce::MouseEvent& event)
{
    if (event.mouseWasDraggedSinceMouseDown()) {
        callListenersCallback (getRelativePosition(), true);
    }
}

void LayoutSplitter::setRelativePosition (float position, juce::UndoManager* undo)
{
    setProperty (propRelativePosition, position, undo);
}

float LayoutSplitter::getRelativePosition() const
{
    return getProperty (propRelativePosition, 0.5);
}

void LayoutSplitter::setMinimumRelativePosition (const float min, juce::UndoManager* undo)
{
    setProperty (propRelativeMinPosition, min, undo);
}

void LayoutSplitter::setMaximumRelativePosition (const float max, juce::UndoManager* undo)
{
    setProperty (propRelativeMaxPosition, max, undo);
}

float LayoutSplitter::getMinimumRelativePosition() const
{
    return getProperty (propRelativeMinPosition, 0.0);
}

float LayoutSplitter::getMaximumRelativePosition() const
{
    return getProperty (propRelativeMaxPosition, 1.0);
}

void LayoutSplitter::setIsHorizontal (bool isHorizontal, juce::UndoManager* undo)
{
    setProperty (propIsHorizontal, isHorizontal, undo);
}

bool LayoutSplitter::getIsHorizontal() const
{
    return getProperty (propIsHorizontal, false);
}

//==============================================================================


/*
void LabeledLayoutItem::fixUpLayoutItems ()
{
    // fix componentID as well
    LayoutItem::fixUpLayoutItems();
    
    if (label) {
        if (label->getText().isEmpty()) {
            removeProperty (propLabelText, nullptr);
        }
        else {
            setProperty (propLabelText, label->getText(), nullptr);
            if (juce::Component* comp = getComponent()) {
                if (Layout* parent = getParentLayout()) {
                    if (LayoutItem* labelItem = parent->getLayoutItem (0)) {
                        labelItem->setComponentID (comp->getComponentID() + "_label", true);
                    }
                }
            }
        }
    }
}

LayoutItem* LabeledLayoutItem::loadLayoutFromValueTree (const juce::ValueTree& tree, juce::Component* owner)
{
    copyPropertiesFrom (tree, nullptr);
    
    if (hasProperty (propLabelText)) {
        if (label) {
            label->setText (getProperty (propLabelText).toString(), juce::dontSendNotification);
        }
        else {
            label = new juce::Label (juce::String::empty, getProperty (propLabelText).toString());
            if (Layout* parent = getParentLayout()) {
                if (LayoutItem* first = parent->getLayoutItem (0)) {
                    first->setComponent (label);
                }
            }
        }
    }
    else {
        if (label) {
            label = nullptr;
        }
    }
    
    return this;
}
*/
