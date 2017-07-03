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
 
 ff_layoutItem.cpp
 Created: 12 Mar 2016 13:14:52pm
 
 ==============================================================================
 */


#include "ff_layout.h"

const juce::Identifier LayoutItem::itemTypeInvalid          ("Invalid");
const juce::Identifier LayoutItem::itemTypeComponent        ("Component");
const juce::Identifier LayoutItem::itemTypeBuilder          ("Builder");
const juce::Identifier LayoutItem::itemTypeLabeledComponent ("LabeledComponent");
const juce::Identifier LayoutItem::itemTypeSplitter         ("Splitter");
const juce::Identifier LayoutItem::itemTypeSpacer           ("Spacer");
const juce::Identifier LayoutItem::itemTypeLine             ("Line");
const juce::Identifier LayoutItem::itemTypeSubLayout        ("Layout");

const juce::Identifier LayoutItem::orientationUnknown       ("unknown");
const juce::Identifier LayoutItem::orientationLeftToRight   ("leftToRight");
const juce::Identifier LayoutItem::orientationTopDown       ("topDown");
const juce::Identifier LayoutItem::orientationRightToLeft   ("rightToLeft");
const juce::Identifier LayoutItem::orientationBottomUp      ("bottomUp");

const juce::Identifier LayoutItem::propStretchX             ("stretchX");
const juce::Identifier LayoutItem::propStretchY             ("stretchY");
const juce::Identifier LayoutItem::propMinWidth             ("minWidth");
const juce::Identifier LayoutItem::propMaxWidth             ("maxWidth");
const juce::Identifier LayoutItem::propMinHeight            ("minHeight");
const juce::Identifier LayoutItem::propMaxHeight            ("maxHeight");
const juce::Identifier LayoutItem::propAspectRatio          ("aspectRatio");
const juce::Identifier LayoutItem::propPaddingTop           ("paddingTop");
const juce::Identifier LayoutItem::propPaddingLeft          ("paddingLeft");
const juce::Identifier LayoutItem::propPaddingRight         ("paddingRight");
const juce::Identifier LayoutItem::propPaddingBottom        ("paddingBottom");

const juce::Identifier LayoutItem::propOrientation          ("orientation");
const juce::Identifier LayoutItem::propLayoutBounds         ("layoutBounds");
const juce::Identifier LayoutItem::propOverlay              ("overlay");
const juce::Identifier LayoutItem::propOverlayWidth         ("overlayWidth");
const juce::Identifier LayoutItem::propOverlayHeight        ("overlayHeight");
const juce::Identifier LayoutItem::propOverlayJustification ("overlayJustification");

const juce::Identifier LayoutItem::propComponentID          ("componentID");
const juce::Identifier LayoutItem::propComponentName        ("componentName");
const juce::Identifier LayoutItem::propLabelText            ("labelText");
const juce::Identifier LayoutItem::propLabelFontSize        ("labelFontSize");
const juce::Identifier LayoutItem::propLabelJustification   ("labelJustification");
const juce::Identifier LayoutItem::propGroupName            ("groupName");
const juce::Identifier LayoutItem::propGroupText            ("groupText");
const juce::Identifier LayoutItem::propGroupJustification   ("groupJustification");

const juce::Identifier LayoutItem::volatileSharedLayoutData ("volatileSharedLayoutData");
const juce::Identifier LayoutItem::volatileItemBounds       ("volatileItemBounds");
const juce::Identifier LayoutItem::volatileItemBoundsFixed  ("volatileItemBoundsFixed");
const juce::Identifier LayoutItem::volatileIsUpdating       ("volatileIsUpdating");


LayoutItem::LayoutItem (LayoutItem::Orientation o)
{
    state = juce::ValueTree (LayoutItem::itemTypeSubLayout);
    setOrientation (o);
}

LayoutItem::LayoutItem (juce::ValueTree state_)
  : state (state_)
{
}

LayoutItem::~LayoutItem()
{
}

LayoutItem LayoutItem::makeSubLayout (juce::ValueTree& parent, Orientation o, int idx, juce::UndoManager* undo)
{
    if (undo) undo->beginNewTransaction (TRANS ("Add sub layout"));
    juce::ValueTree child (itemTypeSubLayout);
    parent.addChild (child, idx, undo);
    LayoutItem item (child);
    item.setOrientation (o, undo);
    return item;
}

LayoutItem LayoutItem::makeChildComponent (juce::ValueTree& parent, juce::Component* component, bool owned, int idx, juce::UndoManager* undo)
{
    if (undo) undo->beginNewTransaction (TRANS ("Add Component item"));
    juce::ValueTree child (itemTypeComponent);
    parent.addChild (child, idx, undo);
    LayoutItem item (child);
    item.setComponent (component, owned, undo);
    return item;
}

LayoutItem LayoutItem::makeLabeledChildComponent (juce::ValueTree& parent, juce::Component* component, juce::String text, LayoutItem::Orientation o, bool owned, int idx, juce::UndoManager* undo)
{
    LayoutItem sub = LayoutItem::makeSubLayout (parent, o, idx, undo);
    LayoutItem container = LayoutItem::makeChildComponent(sub.state, component, owned);
    juce::Label* label = new juce::Label (component->getName() + "_label");
    label->setText (text, juce::dontSendNotification);
    label->setComponentID (component->getComponentID() + "_label");
    label->setJustificationType (juce::Justification::centred);
    LayoutItem labelItem = LayoutItem::makeChildComponent (sub.state, label, true);
    labelItem.setLabelText (text, undo);
    return sub;
}

LayoutSplitter LayoutItem::makeChildSplitter (juce::ValueTree& parent, float position, int idx, juce::UndoManager* undo)
{
    if (undo) undo->beginNewTransaction (TRANS ("Add splitter item"));
    juce::ValueTree child (itemTypeSplitter);
    parent.addChild (child, idx, undo);
    LayoutSplitter item (child);
    item.setRelativePosition (position, undo);
    return item;
}

LayoutItem LayoutItem::makeChildSpacer (juce::ValueTree& parent, float stretchX, float stretchY, int idx, juce::UndoManager* undo)
{
    if (undo) undo->beginNewTransaction (TRANS ("Add spacer item"));
    juce::ValueTree child (itemTypeSpacer);
    parent.addChild (child, idx, undo);
    LayoutItem item (child);
    item.setStretch (stretchX, stretchY, undo);
    return item;
}

void LayoutItem::removeComponent (juce::ValueTree& parent, juce::Component* c, juce::UndoManager* undo)
{
    for (int i=0; i<parent.getNumChildren(); ++i) {
        juce::ValueTree child (parent.getChild (i));
        LayoutItem item (child);
        if (item.getComponent() == c) {
            parent.removeChild (child, undo);
        }
    }
}

bool LayoutItem::isValid()
{
    if (state.getType() == itemTypeComponent) {
        return hasComponent();
    }
    return state.isValid();
}

void LayoutItem::setOrientation (const Orientation o, juce::UndoManager* undo)
{
    state.setProperty (propOrientation, getNameFromOrientation (o).toString(), undo);
}

LayoutItem::Orientation LayoutItem::getOrientation() const
{
    return getOrientationFromName (juce::Identifier (state.getProperty (propOrientation, LayoutItem::orientationUnknown.toString())));
}

bool LayoutItem::isHorizontal () const
{
    LayoutItem::Orientation o = getOrientation();
    return o == LeftToRight || o == RightToLeft;
}

bool LayoutItem::isVertical () const
{
    LayoutItem::Orientation o = getOrientation();
    return o == TopDown || o == BottomUp;
}

LayoutItem::Orientation LayoutItem::getOrientationFromName (juce::Identifier name)
{
    if (name == orientationLeftToRight) {
        return LayoutItem::LeftToRight;
    }
    else if (name == orientationTopDown) {
        return LayoutItem::TopDown;
    }
    else if (name == orientationRightToLeft) {
        return LayoutItem::RightToLeft;
    }
    else if (name == orientationBottomUp) {
        return LayoutItem::BottomUp;
    }
    else
        return LayoutItem::Unknown;
}

juce::Identifier LayoutItem::getNameFromOrientation (LayoutItem::Orientation o)
{
    if (o == LayoutItem::LeftToRight) {
        return orientationLeftToRight;
    }
    else if (o == LayoutItem::TopDown) {
        return orientationTopDown;
    }
    else if (o == LayoutItem::RightToLeft) {
        return orientationRightToLeft;
    }
    else if (o == LayoutItem::BottomUp) {
        return orientationBottomUp;
    }
    else
        return orientationUnknown;
}

int LayoutItem::isOverlay () const
{
    return state.getProperty (propOverlay, 0);
}

void LayoutItem::setIsOverlay (const int overlay)
{
    state.setProperty (propOverlay, overlay, nullptr);
}

float LayoutItem::getOverlayWidth () const
{
    return state.getProperty (propOverlayWidth, 1.0);
}
void LayoutItem::setOverlayWidth (float w)
{
    state.setProperty (propOverlayWidth, w, nullptr);
}
float LayoutItem::getOverlayHeight () const
{
    return state.getProperty (propOverlayHeight, 1.0);
}
void LayoutItem::setOverlayHeight (float h)
{
    state.setProperty (propOverlayHeight, h, nullptr);
}

int LayoutItem::getOverlayJustification () const
{
    return state.getProperty (propOverlayJustification, 36);
}
void LayoutItem::setOverlayJustification (int j)
{
    state.setProperty (propOverlayJustification, j, nullptr);
}

bool LayoutItem::hasComponent () const
{
    if (state.hasProperty (volatileSharedLayoutData)) {
        if (SharedLayoutData* data = dynamic_cast<SharedLayoutData*>(state.getProperty(volatileSharedLayoutData).getObject())) {
            return data->hasComponent();
        }
    }
    return false;
}

juce::Component* LayoutItem::getComponent () const
{
    if (state.hasProperty (volatileSharedLayoutData)) {
        if (SharedLayoutData* data = dynamic_cast<SharedLayoutData*>(state.getProperty(volatileSharedLayoutData).getObject())) {
            return data->getComponent();
        }
    }
    return nullptr;
}

juce::String LayoutItem::getComponentID () const
{
    return state.getProperty (propComponentID, "");
}

LayoutItem::SharedLayoutData* LayoutItem::getOrCreateData (juce::ValueTree& node, juce::UndoManager* undo)
{
    if (node.hasProperty (volatileSharedLayoutData)) {
        if (SharedLayoutData* data = dynamic_cast<SharedLayoutData*>(node.getProperty(volatileSharedLayoutData).getObject())) {
            return data;
        }
        // ouch, somebody put a wrong object into that property!
        jassertfalse;
    }
    SharedLayoutData* data = new SharedLayoutData;
    node.setProperty (volatileSharedLayoutData, data, undo);
    return data;
}

LayoutItem::SharedLayoutData* LayoutItem::getOrCreateData (juce::UndoManager* undo)
{
    return LayoutItem::getOrCreateData (state, undo);
}

void LayoutItem::setComponent (juce::Component* ptr, bool owned, juce::UndoManager* undo)
{
    SharedLayoutData* data = getOrCreateData (undo);
    data->setComponent(ptr, owned);

    if (ptr->getComponentID().isEmpty()) {
        state.removeProperty (propComponentID, undo);
    }
    else {
        state.setProperty (propComponentID, ptr->getComponentID(), undo);
    }
}

void LayoutItem::setLabelText (const juce::String& text, juce::UndoManager* undo)
{
    state.setProperty (propLabelText, text, undo);
}

void LayoutItem::getStretch (float& w, float& h) const
{
    w = state.getProperty (propStretchX, 1.0);
    h = state.getProperty (propStretchY, 1.0);
}

void LayoutItem::setStretch (float w, float h, juce::UndoManager* undo)
{
    state.setProperty (propStretchX, w, undo);
    state.setProperty (propStretchY, h, undo);
}

void LayoutItem::setMinimumWidth  (const int w, juce::UndoManager* undo)
{
    state.setProperty (propMinWidth, w, undo);
}

void LayoutItem::setMaximumWidth  (const int w, juce::UndoManager* undo)
{
    state.setProperty (propMaxWidth, w, undo);
}

void LayoutItem::setMinimumHeight (const int h, juce::UndoManager* undo)
{
    state.setProperty (propMinHeight, h, undo);
}

void LayoutItem::setMaximumHeight (const int h, juce::UndoManager* undo)
{
    state.setProperty (propMaxHeight, h, undo);
}

int LayoutItem::getMinimumWidth  () const
{
    return state.getProperty (propMinWidth, -1);
}

int LayoutItem::getMaximumWidth  () const
{
    return state.getProperty (propMaxWidth, -1);
}

int LayoutItem::getMinimumHeight () const
{
    return state.getProperty (propMinHeight, -1);
}

int LayoutItem::getMaximumHeight () const
{
    return state.getProperty (propMaxHeight, -1);
}


void LayoutItem::getSizeLimits (int& minW, int& maxW, int& minH, int& maxH)
{
    const int minWidth = getMinimumWidth();
    const int maxWidth = getMinimumWidth();
    const int minHeight = getMinimumHeight();
    const int maxHeight = getMinimumHeight();
    if (minWidth >= 0) minW = (minW < 0) ? minWidth : juce::jmax (minW, minWidth);
    if (maxWidth >= 0) maxW = (maxW < 0) ? maxWidth : juce::jmin (maxW, maxWidth);
    if (minHeight >= 0) minH = (minH < 0) ? minHeight : juce::jmax (minH, minHeight);
    if (maxHeight >= 0) maxH = (maxH < 0) ? maxHeight : juce::jmin (maxH, maxHeight);
}

void LayoutItem::setPaddingTop    (const int p, juce::UndoManager* undo)
{
    state.setProperty (propPaddingTop, p, undo);
}
void LayoutItem::setPaddingLeft   (const int p, juce::UndoManager* undo)
{
    state.setProperty (propPaddingLeft, p, undo);
}
void LayoutItem::setPaddingRight  (const int p, juce::UndoManager* undo)
{
    state.setProperty (propPaddingRight, p, undo);
}
void LayoutItem::setPaddingBottom (const int p, juce::UndoManager* undo)
{
    state.setProperty (propPaddingBottom, p, undo);
}

int LayoutItem::getPaddingTop () const
{
    return state.getProperty (propPaddingTop, 0);
}
int LayoutItem::getPaddingLeft () const
{
    return state.getProperty (propPaddingLeft, 0);
}
int LayoutItem::getPaddingRight () const
{
    return state.getProperty (propPaddingRight, 0);
}
int LayoutItem::getPaddingBottom () const
{
    return state.getProperty (propPaddingBottom, 0);
}

void LayoutItem::setAspectRatio (const float ratio, juce::UndoManager* undo)
{
    state.setProperty (propAspectRatio, ratio, undo);
}

float LayoutItem::getAspectRatio () const
{
    return state.getProperty (propAspectRatio, 0.0);
}

juce::ValueTree LayoutItem::getLayoutItem (juce::ValueTree& node, juce::Component* component)
{
    LayoutItem item (node);
    if (item.hasComponent()) {
        if (item.getComponent() == component) {
            return node;
        }
    }
    if (node.getType() == itemTypeSubLayout) {
        for (int i=0; i < node.getNumChildren(); ++i) {
            juce::ValueTree child (node.getChild (i));
            juce::ValueTree result = LayoutItem::getLayoutItem (child, component);
            if (result.isValid()) {
                return result;
            }
        }
    }
    return juce::ValueTree();
}

juce::ValueTree LayoutItem::getLayoutItem (juce::ValueTree& node, juce::String& componentID)
{
    if (node.hasProperty (propComponentID)) {
        if (node.getProperty (propComponentID) == componentID) {
            return node;
        }
    }
    if (node.getType() == itemTypeSubLayout) {
        for (int i=0; i < node.getNumChildren(); ++i) {
            juce::ValueTree child (node.getChild (i));
            juce::ValueTree result = LayoutItem::getLayoutItem (child, componentID);
            if (result.isValid()) {
                return result;
            }
        }
    }
    return juce::ValueTree();
}

void LayoutItem::constrainBounds (juce::Rectangle<int>& bounds, bool& changedWidth, bool& changedHeight, bool preferVertical)
{
    int cbMinWidth = getMinimumWidth();
    int cbMaxWidth = getMaximumWidth();
    int cbMinHeight = getMinimumHeight();
    int cbMaxHeight = getMaximumHeight();
    float aspectRatio = getAspectRatio();
    
    changedWidth  = false;
    changedHeight = false;
    
    if (cbMaxWidth > 0 && cbMaxWidth < bounds.getWidth()) {
        bounds.setWidth (cbMaxWidth);
        changedWidth = true;
    }
    if (aspectRatio > 0.001 && !preferVertical) {
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
    if (aspectRatio > 0.001 && preferVertical) {
        bounds.setHeight (bounds.getWidth() / aspectRatio);
        changedHeight = true;
    }
    if (cbMinHeight > 0 && cbMinHeight > bounds.getHeight()) {
        bounds.setHeight (cbMinHeight);
        changedHeight = true;
    }
}

void LayoutItem::setItemBounds (juce::Rectangle<int> b)
{
    state.setProperty (volatileItemBounds, b.toString(), nullptr);
}

void LayoutItem::setItemBounds (int x, int y, int w, int h)
{
    setItemBounds (juce::Rectangle<int> (x, y, w, h));
}

juce::Rectangle<int> LayoutItem::getItemBounds() const
{
    if (state.hasProperty (volatileItemBounds)) {
        juce::Rectangle<int> bounds = juce::Rectangle<int>::fromString (state.getProperty (volatileItemBounds).toString());
        return bounds;
    }
    return juce::Rectangle<int>();
}

juce::Rectangle<int> LayoutItem::getPaddedItemBounds () const
{
    const int paddingLeft = getPaddingLeft();
    const int paddingTop = getPaddingTop();
    juce::Rectangle<int> bounds = getItemBounds();
    return juce::Rectangle<int> (bounds.getX() + paddingLeft,
                                 bounds.getY() + paddingTop,
                                 bounds.getWidth() - (paddingLeft+getPaddingRight()),
                                 bounds.getHeight() - (paddingTop+getPaddingBottom()));
}

void LayoutItem::setBoundsAreFinal (bool final)
{
    state.setProperty (volatileItemBoundsFixed, final, nullptr);
}

bool LayoutItem::getBoundsAreFinal() const
{
    return state.getProperty (volatileItemBoundsFixed, false);
}

void LayoutItem::paintBounds (const juce::ValueTree& node, juce::Graphics& g)
{
    LayoutItem item (node);
    if (item.isHorizontal()) {
        g.setColour (juce::Colours::red);
    }
    else if (item.isVertical()) {
        g.setColour (juce::Colours::green);
    }
    else {
        g.setColour (juce::Colours::grey);
    }
    for (int i=0; i<node.getNumChildren(); ++i) {
        juce::ValueTree child (node.getChild (i));
        const LayoutItem childItem (child);
        if (child.getType() == itemTypeSubLayout) {
            g.saveState();
            LayoutItem::paintBounds (child, g);
            juce::Rectangle<int> bounds (childItem.getItemBounds());
            if (!bounds.isEmpty())
                g.drawRect (bounds);
            g.restoreState();
        }
        else {
            g.drawRect (childItem.getItemBounds().reduced(1));
        }
    }
}


void LayoutItem::setWrappedComponentID (const juce::String& name, bool setComp)
{
    if (setComp && getComponent()) {
        getComponent()->setComponentID (name);
    }
    if (name.isEmpty()) {
        state.removeProperty(propComponentID, nullptr);
    }
    else {
        state.setProperty (propComponentID, name, nullptr);
    }
}


void LayoutItem::fixUpLayoutItems ()
{
//    if (juce::Component* comp = sharedLayoutData->getComponent()) {
//        setWrappedComponentID (comp->getComponentID(), false);
//    }
}

void LayoutItem::realize (juce::ValueTree& node, juce::Component* owningComponent, Layout* layout)
{
    LayoutItem item (node);
    if (node.getType() == itemTypeComponent) {
        if (node.hasProperty (propComponentID)) {
            if (juce::Component* component = owningComponent->findChildWithID (node.getProperty (propComponentID).toString())) {
                item.setComponent (component, false);
                return;
            }
        }
        if (node.hasProperty (propComponentName)) {
            juce::String name = node.getProperty (propComponentName);
            for (int i=0; i < owningComponent->getNumChildComponents(); ++i) {
                juce::Component* child = owningComponent->getChildComponent (i);
                if (child->getName() == name) {
                    item.setComponent (child, false);
                    return;
                }
            }
        }
        if (node.hasProperty (propLabelText)) {
            juce::Label* newLabel = new juce::Label();
            newLabel->setText (juce::LocalisedStrings::translateWithCurrentMappings (node.getProperty (propLabelText).toString()), juce::dontSendNotification);
            if (node.hasProperty (propLabelFontSize)) {
                newLabel->setFont (juce::Font (node.getProperty (propLabelFontSize), 12.0));
            }
            if (node.hasProperty (propLabelJustification)) {
                newLabel->setJustificationType (juce::Justification (node.getProperty (propLabelJustification, 36)));
            }
            if (node.hasProperty (propComponentName)) {
                newLabel->setName (node.getProperty (propComponentName).toString());
            }
            if (node.hasProperty (propComponentID)) {
                newLabel->setComponentID (node.getProperty (propComponentID).toString());
            }
            
            owningComponent->addAndMakeVisible (newLabel);
            item.setComponent (newLabel, true);
        }
    }
    else if (node.getType() == itemTypeSplitter) {
        LayoutSplitter::Component* splitterComponent = new LayoutSplitter::Component (layout);
        LayoutSplitter splitter (node);
        LayoutItem parent (node.getParent());
        if (node.hasProperty (propComponentID)) {
            splitterComponent->setComponentID (node.getProperty (propComponentID).toString());
        }
        if (parent.isHorizontal()) {
            splitter.setFixedWidth (3);
            splitterComponent->setMouseCursor (juce::MouseCursor::LeftRightResizeCursor);
        }
        else {
            splitter.setFixedHeight (3);
            splitterComponent->setMouseCursor (juce::MouseCursor::UpDownResizeCursor);
        }
        splitter.setComponent (splitterComponent, owningComponent);
        owningComponent->addAndMakeVisible (splitterComponent);
        splitter.addListener (layout);
    }
    else if (node.getType() == itemTypeBuilder) {
        if (node.getNumChildren() > 0) {
            juce::ValueTree buildNode = node.getChild (0);
            juce::ComponentBuilder builder (buildNode);
            builder.registerStandardComponentTypes();
            juce::Component* component = builder.createComponent();
            if (node.hasProperty (propComponentName)) {
                component->setName (node.getProperty (propComponentName).toString());
            }
            if (node.hasProperty (propComponentID)) {
                component->setComponentID (node.getProperty (propComponentID).toString());
            }
            owningComponent->addAndMakeVisible (component);
            item.setComponent (component, true /* owned */);
        }
    }
    else if (node.getType() == itemTypeSubLayout) {
        if (node.hasProperty (propGroupName) || node.hasProperty (propGroupText)) {
            juce::GroupComponent* group = new juce::GroupComponent();
            if (node.hasProperty (propGroupName)) {
                group->setName (node.getProperty (propGroupName).toString());
            }
            if (node.hasProperty (propGroupText)) {
                group->setText (juce::LocalisedStrings::translateWithCurrentMappings (node.getProperty (propGroupText).toString()));
            }
            if (node.hasProperty (propGroupJustification)) {
                group->setTextLabelPosition (juce::Justification (node.getProperty (propGroupJustification)));
            }
            owningComponent->addAndMakeVisible(group);
            item.setComponent (group, true);
        }
        for (int i=0; i < node.getNumChildren(); ++i) {
            juce::ValueTree child = node.getChild (i);
            LayoutItem item (child);
            item.realize (child, owningComponent, layout);
        }
    }
}


int LayoutItem::updateGeometry (juce::ValueTree& node, juce::Rectangle<int> bounds)
{
    int needsGrowing = 0;
    
    if (node.getType() == itemTypeSubLayout) {
        LayoutItem layout (node);
        const Orientation orientation = layout.getOrientation();
        
        // find splitter items
        int last = 0;
        juce::Rectangle<int> childBounds (bounds);
        for (int i=0; i<node.getNumChildren(); ++i) {
            juce::ValueTree childNode = node.getChild (i);
            LayoutItem item (childNode);
            if (item.isOverlay() < 1) {
                if (childNode.getType() == itemTypeSplitter) {
                    LayoutSplitter splitter (childNode);
                    juce::Rectangle<int> splitterBounds (bounds);
                    
                    // calculate minimum and maximum splitter positions
                    int leftMinW = -1;
                    int leftMaxW = -1;
                    int leftMinH = -1;
                    int leftMaxH = -1;
                    getSizeLimits (node, leftMinW, leftMaxW, leftMinH, leftMaxH, last, i);
                    int rightMinW = -1;
                    int rightMaxW = -1;
                    int rightMinH = -1;
                    int rightMaxH = -1;
                    getSizeLimits (node, rightMinW, rightMaxW, rightMinH, rightMaxH, i, node.getNumChildren());
                    
                    if (orientation == LayoutItem::LeftToRight) {
                        if (bounds.getWidth() > 0) {
                            float relPosition = splitter.getRelativePosition();
                            float minRelPosition = static_cast<float>(leftMinW) / bounds.getWidth();
                            float maxRelPosition = 1.0 - static_cast<float>(rightMinW) / bounds.getWidth();
                            if (relPosition < minRelPosition) {
                                splitter.setRelativePosition (minRelPosition);
                            }
                            else if (relPosition > maxRelPosition) {
                                splitter.setRelativePosition (maxRelPosition);
                            }
                        }
                        int right = childBounds.getX() + splitter.getRelativePosition() * bounds.getWidth();
                        updateGeometry (node, childBounds.withRight (right-1), last, i);
                        splitterBounds.setX (right-1);
                        splitterBounds.setWidth (3);
                        childBounds.setLeft (right+1);
                    }
                    else if (orientation == LayoutItem::TopDown) {
                        if (bounds.getWidth() > 0) {
                            float relPosition = splitter.getRelativePosition();
                            float minRelPosition = static_cast<float>(leftMinH) / bounds.getWidth();
                            float maxRelPosition = 1.0 - static_cast<float>(rightMinH) / bounds.getWidth();
                            if (relPosition < minRelPosition) {
                                splitter.setRelativePosition (minRelPosition);
                            }
                            else if (relPosition > maxRelPosition) {
                                splitter.setRelativePosition (maxRelPosition);
                            }
                        }
                        int bottom = childBounds.getY() + splitter.getRelativePosition() * bounds.getHeight();
                        updateGeometry (node, childBounds.withBottom (bottom), last, i);
                        splitterBounds.setY (bottom-1);
                        splitterBounds.setHeight (3);
                        childBounds.setTop (bottom+1);
                    }
                    else if (orientation == LayoutItem::RightToLeft) {
                        if (bounds.getWidth() > 0) {
                            float relPosition = splitter.getRelativePosition();
                            float minRelPosition = 1.0 - static_cast<float>(leftMinW) / bounds.getWidth();
                            float maxRelPosition = static_cast<float>(rightMinW) / bounds.getWidth();
                            if (relPosition < minRelPosition) {
                                splitter.setRelativePosition (minRelPosition);
                            }
                            else if (relPosition > maxRelPosition) {
                                splitter.setRelativePosition (maxRelPosition);
                            }
                        }
                        int left = childBounds.getX() + splitter.getRelativePosition() * bounds.getWidth();
                        updateGeometry (node, childBounds.withLeft (left), last, i);
                        splitterBounds.setX (left-1);
                        splitterBounds.setWidth (3);
                        childBounds.setRight (left-1);
                    }
                    else if (orientation == LayoutItem::BottomUp) {
                        if (bounds.getWidth() > 0) {
                            float relPosition = splitter.getRelativePosition();
                            float minRelPosition = 1.0 - static_cast<float>(leftMinH) / bounds.getWidth();
                            float maxRelPosition = static_cast<float>(rightMinH) / bounds.getWidth();
                            if (relPosition < minRelPosition) {
                                splitter.setRelativePosition (minRelPosition);
                            }
                            else if (relPosition > maxRelPosition) {
                                splitter.setRelativePosition (maxRelPosition);
                            }
                        }

                        int top = childBounds.getY() + splitter.getRelativePosition() * bounds.getHeight();
                        updateGeometry (node, childBounds.withTop (top), last, i);
                        splitterBounds.setY (top-1);
                        splitterBounds.setHeight (3);
                        childBounds.setBottom (top-1);
                    }
                    splitter.setItemBounds (splitterBounds);
                    splitter.setBounds (splitterBounds);
                    splitter.setBoundsAreFinal (true);
                    
                    i++;
                    last = i;
                }
            }
        }
        
        // layout rest right of splitter, if any
        updateGeometry (node, childBounds, last, node.getNumChildren());
    }
    else {
        // give the chance to respect aspectRatio constraint - depends on given size
        juce::Rectangle<int> childBounds (bounds);
        LayoutItem item (node);
        LayoutItem parent (node.getParent());
        bool changedWidth, changedHeight;
        item.constrainBounds (childBounds, changedWidth, changedHeight, parent.isVertical());
    }
    return needsGrowing;
}

int LayoutItem::updateGeometry (juce::ValueTree& node, juce::Rectangle<int> bounds, int start, int end)
{
    LayoutItem layout (node);
    int needsGrowing = 0;
    
    // recursion check
    if (node.getProperty (volatileIsUpdating, false)) {
        return 0;
    }
    node.setProperty (volatileIsUpdating, true, nullptr);
    
    float cummulatedX, cummulatedY;
    LayoutItem::getStretch (node, cummulatedX, cummulatedY, start, end);
    float availableWidth  = bounds.getWidth();
    float availableHeight = bounds.getHeight();
    const Orientation orientation = layout.getOrientation();
    
    if (layout.isVertical()) {
        for (int i=start; i<juce::jmin (node.getNumChildren(), end); ++i) {
            juce::ValueTree child (node.getChild (i));
            LayoutItem item (child);
            int overlay = item.isOverlay();
            if (overlay < 1) {
                float sx, sy;
                LayoutItem::getStretch (child, sx, sy);

                float h = bounds.getHeight() * sy / cummulatedY;
                juce::Rectangle<int> childBounds (bounds.getX(), bounds.getY(), bounds.getWidth(), h);
                bool changedWidth, changedHeight;
                item.constrainBounds (childBounds, changedWidth, changedHeight, true);
                item.setItemBounds (childBounds);
                if (changedHeight) {
                    item.setBoundsAreFinal (true);
                    availableHeight -= childBounds.getHeight();
                    cummulatedY -= sy;
                }
                else {
                    item.setBoundsAreFinal (false);
                }
                if (changedWidth) {
                    availableWidth = std::max (bounds.getWidth(), childBounds.getWidth());
                }
            }
        }
        needsGrowing = std::max (static_cast<int>(-availableHeight), 0);
        
        float y = bounds.getY();
        if (orientation == BottomUp) {
            y = bounds.getY() + bounds.getHeight();
        }
        for (int i=start; i<juce::jmin (node.getNumChildren(), end); ++i) {
            juce::ValueTree child = node.getChild (i);
            LayoutItem item (child);
            
            if (item.isOverlay() == 0) {
                if (item.getBoundsAreFinal()) {
                    float h = item.getItemBounds().getHeight();
                    if (orientation == BottomUp) {
                        y -= h;
                    }
                    item.setItemBounds (bounds.getX(), y, availableWidth, h);
                    if (child.getType() == itemTypeSubLayout) {
                        LayoutItem::updateGeometry (child, item.getPaddedItemBounds());
                        if (juce::Component* c = item.getComponent()) {
                            // component in a layout is a GroupComponent, so don't pad component but contents
                            c->setBounds (item.getItemBounds());
                        }
                    }
                    else if (juce::Component* c = item.getComponent()) {
                        c->setBounds (item.getPaddedItemBounds());
                    }
                    
                    if (orientation == TopDown) {
                        y += h;
                    }
                }
                else {
                    float sx, sy;
                    LayoutItem::getStretch (child, sx, sy);
                    
                    float h = availableHeight * sy /cummulatedY;
                    if (orientation == BottomUp) {
                        y -= h;
                    }
                    item.setItemBounds (bounds.getX(), y, availableWidth, h );
                    if (child.getType() == itemTypeSubLayout) {
                        LayoutItem sub (child);
                        sub.updateGeometry (child, item.getPaddedItemBounds());
                        if (juce::Component* c = item.getComponent()) {
                            // component in a layout is a GroupComponent, so don't pad component but contents
                            c->setBounds (item.getItemBounds());
                        }
                    }
                    else if (juce::Component* c = item.getComponent()) {
                        c->setBounds (item.getPaddedItemBounds());
                    }
                    item.callListenersCallback (item.getPaddedItemBounds());
                    if (orientation == TopDown) {
                        y += h;
                    }
                }
            }
            else {
                // overlay other item
                juce::Rectangle<int> overlayTarget;
                if (item.isOverlay() == 1 && i > start) {
                    LayoutItem previous (node.getChild(i-1));
                    overlayTarget = previous.getItemBounds();
                }
                else if (item.isOverlay() == 2) {
                    overlayTarget = bounds;
                }
                juce::Rectangle<int> overlayBounds (0, 0, item.getOverlayWidth() * overlayTarget.getWidth(), item.getOverlayHeight() * overlayTarget.getHeight());
                bool changedWidth, changedHeight;
                item.constrainBounds (overlayBounds, changedWidth, changedHeight, true);
                juce::Justification j (item.getOverlayJustification());
                item.setItemBounds (j.appliedToRectangle (overlayBounds, overlayTarget));
                if (child.getType() == itemTypeSubLayout) {
                    LayoutItem::updateGeometry (child, item.getPaddedItemBounds());
                    if (juce::Component* c = item.getComponent()) {
                        // component in a layout is a GroupComponent, so don't pad component but contents
                        c->setBounds (item.getItemBounds());
                    }
                }
                else if (juce::Component* c = item.getComponent()) {
                    c->setBounds (item.getPaddedItemBounds());
                }
                item.callListenersCallback (item.getPaddedItemBounds());
                
            }
        }
    } else if (layout.isHorizontal()) {
        for (int i=start; i<juce::jmin (node.getNumChildren(), end); ++i) {
            juce::ValueTree child (node.getChild (i));
            LayoutItem item (child);
            int overlay = item.isOverlay();
            if (overlay < 1) {
                float sx, sy;
                LayoutItem::getStretch (child, sx, sy);

                float w = bounds.getWidth() * sx / cummulatedX;
                juce::Rectangle<int> childBounds (bounds.getX(), bounds.getY(), w, bounds.getHeight());
                bool changedWidth, changedHeight;
                item.constrainBounds (childBounds, changedWidth, changedHeight, false);
                item.setItemBounds (childBounds);
                if (changedWidth) {
                    item.setBoundsAreFinal (true);
                    availableWidth -= childBounds.getWidth();
                    cummulatedX -= sx;
                }
                else {
                    item.setBoundsAreFinal (false);
                }
                if (changedHeight) {
                    availableHeight = std::max (bounds.getHeight(), childBounds.getHeight());
                }
            }
        }
        needsGrowing = std::max (static_cast<int>(-availableWidth), 0);

        float x = bounds.getX();
        if (orientation == RightToLeft) {
            x = bounds.getX() + bounds.getWidth();
        }
        for (int i=start; i<juce::jmin (node.getNumChildren(), end); ++i) {
            juce::ValueTree child = node.getChild (i);
            LayoutItem item (child);
            if (item.isOverlay() < 1) {
                if (item.getBoundsAreFinal()) {
                    float w = item.getItemBounds().getWidth();
                    if (orientation == RightToLeft) {
                        x -= w;
                    }
                    item.setItemBounds (x, bounds.getY(), w, availableHeight);
                    juce::Rectangle<int> childBounds (x, bounds.getY(), w, availableHeight);
                    if (child.getType() == itemTypeSubLayout) {
                        LayoutItem::updateGeometry (child, item.getPaddedItemBounds());
                        if (juce::Component* c = item.getComponent()) {
                            // component in a layout is a GroupComponent, so don't pad component but contents
                            c->setBounds (item.getItemBounds());
                        }
                    }
                    else if (juce::Component* c = item.getComponent()) {
                        c->setBounds (item.getPaddedItemBounds());
                    }
                    
                    if (orientation == LeftToRight) {
                        x += w;
                    }
                }
                else {
                    float sx, sy;
                    LayoutItem::getStretch (child, sx, sy);

                    float w = availableWidth * sx /cummulatedX;
                    if (orientation == RightToLeft) {
                        x -= w;
                    }
                    item.setItemBounds (x, bounds.getY(), w, availableHeight);
                    if (child.getType() == itemTypeSubLayout) {
                        LayoutItem::updateGeometry (child, item.getPaddedItemBounds());
                        if (juce::Component* c = item.getComponent()) {
                            // component in a layout is a GroupComponent, so don't pad component but contents
                            c->setBounds (item.getItemBounds());
                        }
                    }
                    else if (juce::Component* c = item.getComponent()) {
                        c->setBounds (item.getPaddedItemBounds());
                    }
                    item.callListenersCallback (item.getPaddedItemBounds());
                    if (orientation == LeftToRight) {
                        x += w;
                    }
                }
            }
            else {
                // overlay other item
                juce::Rectangle<int> overlayTarget;
                if (item.isOverlay() == 1 && i > start) {
                    LayoutItem previous (node.getChild(i-1));
                    overlayTarget = previous.getItemBounds();
                }
                else if (item.isOverlay() == 2) {
                    overlayTarget = bounds;
                }
                juce::Rectangle<int> overlayBounds (0, 0, item.getOverlayWidth() * overlayTarget.getWidth(), item.getOverlayHeight() * overlayTarget.getHeight());
                bool changedWidth, changedHeight;
                item.constrainBounds (overlayBounds, changedWidth, changedHeight, true);
                juce::Justification j (item.getOverlayJustification());
                item.setItemBounds (j.appliedToRectangle (overlayBounds, overlayTarget));
                if (child.getType() == itemTypeSubLayout) {
                    LayoutItem sub (child);
                    sub.updateGeometry (child, item.getPaddedItemBounds());
                    if (juce::Component* c = item.getComponent()) {
                        // component in a layout is a GroupComponent, so don't pad component but contents
                        c->setBounds (item.getItemBounds());
                    }
                }
                else if (juce::Component* c = item.getComponent()) {
                    c->setBounds (item.getPaddedItemBounds());
                }
                item.callListenersCallback (item.getPaddedItemBounds());
                
            }
        }
    }
    
    node.setProperty (volatileIsUpdating, false, nullptr);
    return needsGrowing;
}

void LayoutItem::getStretch (const juce::ValueTree& node, float& w, float& h, int start, int end)
{
    if (node.getType() != itemTypeSubLayout) {
        w = node.getProperty (propStretchX, 1.0);
        h = node.getProperty (propStretchY, 1.0);
        return;
    }

    LayoutItem layout (node);
    bool horizontal = layout.isHorizontal();
    bool vertical   = layout.isVertical();
    
    float stretchX = node.getProperty (propStretchX, -1.0);
    float stretchY = node.getProperty (propStretchY, -1.0);
    
    w = std::max (stretchX, 0.0f);
    h = std::max (stretchY, 0.0f);
    
    if (w > 0.0001 && h > 0.0001) {
        return;
    }

    for (int i=start; i<std::min (end, node.getNumChildren()); ++i) {
        juce::ValueTree child = node.getChild (i);
        LayoutItem item (child);
        if (item.isOverlay() < 1) {
            float x, y;
            LayoutItem::getStretch (child, x, y);
            if (horizontal) {
                if (stretchX <= 0) w += x;
                if (stretchY <= 0) h = std::max (h, y);
            }
            else if (vertical) {
                if (stretchX <= 0) w = std::max (w, x);
                if (stretchY <= 0) h += y;
            }
            else {
                if (stretchX <= 0) w += x;
                if (stretchY <= 0) h += y;
            }
        }
    }

    // no stretch in children defaults to 1.0
    if (w <= 0.0) w = 1.0;
    if (h <= 0.0) h = 1.0;
}


void LayoutItem::getSizeLimits (const juce::ValueTree& node, int& minW, int& maxW, int& minH, int& maxH, int start, int end)
{
    bool canConsumeWidth = false;
    bool canConsumeHeight = false;

    LayoutItem item (node);
    
    if (end < 0) {
        end = node.getNumChildren();
    }

    if (node.getType() == itemTypeSubLayout) {
        if (item.isVertical()) {
            for (int i=start; i < std::min (end, node.getNumChildren()); ++i) {
                LayoutItem item (node.getChild (i));
                if (item.isOverlay() < 1) {
                    if (item.getMinimumWidth() >= 0) minW = (minW < 0) ? item.getMinimumWidth() : juce::jmax(minW, item.getMinimumWidth());
                    if (item.getMaximumWidth() >= 0) maxW = (maxW < 0) ? item.getMaximumWidth() : juce::jmin(maxW, item.getMaximumWidth());
                    if (item.getMinimumHeight() >= 0) minH = (minH < 0) ? item.getMinimumHeight() : minH + item.getMinimumHeight();
                    if (item.getMaximumHeight() >= 0) {
                        maxH = (maxH < 0) ? item.getMaximumHeight() : maxH + item.getMaximumHeight();
                    }
                    else {
                        canConsumeHeight = true;
                    }
                }
            }
        }
        else if (item.isHorizontal()) {
            for (int i=start; i < std::min (end, node.getNumChildren()); ++i) {
                LayoutItem item (node.getChild (i));
                if (item.isOverlay() < 1) {
                    if (item.getMinimumWidth() >= 0) minW = (minW < 0) ? item.getMinimumWidth() : minW + item.getMinimumWidth();
                    if (item.getMaximumWidth() >= 0) {
                        maxW = (maxW < 0) ? item.getMaximumWidth() : maxW + item.getMaximumWidth();
                    }
                    else {
                        canConsumeWidth = true;
                    }
                    if (item.getMinimumHeight() >= 0) minH = (minH < 0) ? item.getMinimumHeight() : juce::jmax(minH, item.getMinimumHeight());
                    if (item.getMaximumHeight() >= 0) maxH = (maxH < 0) ? item.getMaximumHeight() : juce::jmin(maxH, item.getMaximumHeight());
                }
            }
        }
        if (canConsumeWidth)  maxW = -1;
        if (canConsumeHeight) maxH = -1;
    }
    else {
        int cMinW = item.getMinimumWidth();
        int cMaxW = item.getMaximumWidth();
        int cMinH = item.getMinimumHeight();
        int cMaxH = item.getMaximumHeight();
        if (cMinW >= 0) { if (minW < 0) minW = cMinW; else minW = std::max (minW, cMinW); }
        if (cMaxW >= 0) { if (maxW < 0) maxW = cMaxW; else maxW = std::min (maxW, cMaxW); }
        if (cMinH >= 0) { if (minH < 0) minH = cMinH; else minH = std::max (minH, cMinH); }
        if (cMaxH >= 0) { if (maxH < 0) maxH = cMaxH; else maxH = std::min (maxH, cMaxH); }
    }
}

// =============================================================================


const juce::Component* LayoutItem::SharedLayoutData::getComponent() const {
    if (ownedComponent) {
        return ownedComponent;
    }
    if (componentPtr) {
        return componentPtr;
    }
    return nullptr;
}

juce::Component* LayoutItem::SharedLayoutData::getComponent() {
    if (ownedComponent) {
        return ownedComponent;
    }
    if (componentPtr) {
        return componentPtr;
    }
    return nullptr;
}

void LayoutItem::SharedLayoutData::setComponent (juce::Component* c, bool owned)
{
    if (owned) {
        if (ownedComponent == c) {
            return;
        }
        componentPtr = nullptr;
        ownedComponent = c;
    }
    else {
        if (componentPtr == c) {
            return;
        }
        ownedComponent = nullptr;
        componentPtr = c;
    }
}

bool LayoutItem::SharedLayoutData::hasComponent () const {
    return componentPtr || ownedComponent;
}

void LayoutItem::SharedLayoutData::addLayoutListener (LayoutItem::Listener* l) {
    layoutItemListeners.add (l);
}
void LayoutItem::SharedLayoutData::removeLayoutListener (LayoutItem::Listener* l) {
    layoutItemListeners.remove (l);
}
void LayoutItem::SharedLayoutData::removeAllListeners () {
    layoutItemListeners.clear();
}

void LayoutItem::SharedLayoutData::callListenersCallback (juce::ValueTree item, juce::Rectangle<int> newBounds)
{
    layoutItemListeners.call(&LayoutItem::Listener::layoutBoundsChanged, item, newBounds);
}

void LayoutItem::SharedLayoutData::callListenersCallback (juce::ValueTree item, float relativePosition, bool final)
{
    layoutItemListeners.call(&LayoutItem::Listener::layoutSplitterMoved, item, relativePosition, final);
}

// =============================================================================
void LayoutItem::addListener (LayoutItemListener* const newListener)
{
    SharedLayoutData* data = getOrCreateData (state);
    data->addLayoutListener (newListener);
}

void LayoutItem::removeListener (LayoutItemListener* const listener)
{
    if (state.hasProperty (volatileSharedLayoutData)) {
        SharedLayoutData* data = getOrCreateData (state);
        data->removeLayoutListener (listener);
    }
}

void LayoutItem::callListenersCallback (juce::Rectangle<int> newBounds)
{
    SharedLayoutData* data = getOrCreateData (state);
    data->callListenersCallback (state, newBounds);
}

void LayoutItem::callListenersCallback (float relativePosition, bool final)
{
    SharedLayoutData* data = getOrCreateData (state);
    data->callListenersCallback (state, relativePosition, final);
}

//==============================================================================

const juce::Identifier LayoutSplitter::propRelativePosition       ("relativePosition");
const juce::Identifier LayoutSplitter::propRelativeMinPosition    ("relativeMinPosition");
const juce::Identifier LayoutSplitter::propRelativeMaxPosition    ("relativeMaxPosition");


LayoutSplitter::LayoutSplitter (juce::ValueTree& node)
:  LayoutItem(node)
{
}

LayoutSplitter::~LayoutSplitter()
{
}


float LayoutSplitter::getRelativePosition() const
{
    return state.getProperty (propRelativePosition, 0.5);
}

void LayoutSplitter::setRelativePosition (float position, juce::UndoManager* undo)
{
    state.setProperty (propRelativePosition, position, undo);
}

void LayoutSplitter::setMinimumRelativePosition (const float min, juce::UndoManager* undo)
{
    state.setProperty (propRelativeMinPosition, min, undo);
}

void LayoutSplitter::setMaximumRelativePosition (const float max, juce::UndoManager* undo)
{
    state.setProperty (propRelativeMaxPosition, max, undo);
}

float LayoutSplitter::getMinimumRelativePosition() const
{
    return state.getProperty (propRelativeMinPosition, 0.0);
}

float LayoutSplitter::getMaximumRelativePosition() const
{
    return state.getProperty (propRelativeMaxPosition, 1.0);
}

bool LayoutSplitter::isHorizontal () const
{
    LayoutItem parent (state.getParent());
    return parent.isHorizontal();
}

void LayoutSplitter::setBounds (juce::Rectangle<int> b)
{
    if (juce::Component* splitterComponent = getComponent()) {
        splitterComponent->setBounds (b);
    }
}

LayoutSplitter::Component::Component (Layout* layout)
: layoutPtr (layout)
{
}

void LayoutSplitter::Component::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::grey);
}

void LayoutSplitter::Component::mouseDrag (const juce::MouseEvent &event)
{
    if (layoutPtr) {
        juce::ValueTree item = layoutPtr->getLayoutItem (this);
        if (item.isValid()) {
            juce::ValueTree parentNode (item.getParent());
            LayoutSplitter  splitter (item);
            LayoutItem      parent   (parentNode);
            
            if (juce::Component* parentComponent = getParentComponent()) {
                float pos;
                bool horizontal = true;
                juce::Rectangle<int> layoutBounds (parentComponent->getLocalBounds());
                
                horizontal = parent.isHorizontal();
                if (!parent.getItemBounds().isEmpty()) {
                    layoutBounds = parent.getItemBounds();
                }
                if (horizontal) {
                    pos = (event.getEventRelativeTo(parentComponent).position.getX() - layoutBounds.getX()) / layoutBounds.getWidth();
                }
                else {
                    pos = (event.getEventRelativeTo(parentComponent).position.getY() - layoutBounds.getY()) / layoutBounds.getHeight();
                }
                splitter.setRelativePosition (juce::jmax (splitter.getMinimumRelativePosition(), juce::jmin (splitter.getMaximumRelativePosition(), pos)));
                
                layoutPtr->updateGeometry();
                splitter.callListenersCallback (splitter.getRelativePosition(), false);
            }
        }
    }
}

void LayoutSplitter::Component::mouseUp (const juce::MouseEvent& event)
{
    if (layoutPtr) {
        juce::ValueTree item = layoutPtr->getLayoutItem (this);
        if (item.isValid()) {
            LayoutSplitter  splitter (item);
            if (splitter.isValid()) {
                splitter.callListenersCallback (splitter.getRelativePosition(), true);
            }
        }
    }
}
