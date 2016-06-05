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
 
    LayoutXMLEditor.cpp
    Created: 23 Apr 2016 6:53:29pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PreviewComponent.h"
#include "LayoutTreeView.h"
#include "LayoutTreeViewItem.h"
#include "LayoutXMLEditor.h"
#include "LayoutEditorApplication.h"

//==============================================================================
LayoutXMLEditor::LayoutXMLEditor() : needsSaving (false)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    //"DBG (ComponentBuilder::DrawableTypeHandler<DrawableText>::type);

    codeTokeniser = new XmlTokeniser;
    codeDocument  = new CodeDocument;
    codeEditor    = new CodeEditorComponent (*codeDocument, codeTokeniser);
    addAndMakeVisible (codeEditor);
    codeDocument->addListener (this);

    localisedStringsEditor = new TextEditor;
    localisedStringsEditor->setMultiLine (true);
    addAndMakeVisible (localisedStringsEditor);

    layoutTree    = new LayoutTreeView;
    addAndMakeVisible (layoutTree);
    layoutTree->setDefaultOpenness (true);

    nodeProperties = new PropertyPanel;
    addAndMakeVisible (nodeProperties);

    Label* text = new Label;
    text->setText (TRANS ("Properties"), dontSendNotification);
    addAndMakeVisible (text);

    insertButtons = new Toolbar;
    insertButtons->addDefaultItems (*this);
    addAndMakeVisible (insertButtons);

    layout = new Layout (LayoutItem::LeftToRight, this);
    LayoutItem left = LayoutItem::makeSubLayout (layout->state, LayoutItem::TopDown);
    LayoutItem::makeChildComponent (left.state, codeEditor);
    LayoutItem::makeChildSplitter  (left.state, 0.7f);
    LayoutItem::makeChildComponent (left.state, localisedStringsEditor);
    LayoutItem::makeChildSplitter  (layout->state, 0.7f);
    LayoutItem right = LayoutItem::makeSubLayout (layout->state, LayoutItem::TopDown);
    LayoutItem::makeChildComponent (right.state, insertButtons).setFixedHeight (36);
    LayoutItem::makeChildComponent (right.state, layoutTree);
    LayoutItem::makeChildSplitter  (right.state, 0.7f);
    LayoutItem::makeChildComponent (right.state, text, true).setFixedHeight (24);
    LayoutItem::makeChildComponent (right.state, nodeProperties);

    documentContent.addListener (this);

    setSize (700, 700);
    layout->realize();
    layout->updateGeometry();
}

LayoutXMLEditor::~LayoutXMLEditor()
{
    documentContent.removeListener (this);
    codeDocument->removeListener (this);
    if (previewWindow) {
        deleteAndZero (previewWindow);
    }
    layoutTree->deleteRootItem();
}

ApplicationCommandTarget* LayoutXMLEditor::getNextCommandTarget ()
{
    return nullptr;
}

void LayoutXMLEditor::getAllCommands (Array< CommandID > &commands)
{
    commands.add (CMDLayoutEditor_New);
    commands.add (CMDLayoutEditor_Open);
    commands.add (CMDLayoutEditor_Save);
    commands.add (CMDLayoutEditor_SaveAs);
    commands.add (StandardApplicationCommandIDs::del);
    commands.add (CMDLayoutEditor_Run);
    commands.add (CMDLayoutEditor_Refresh);
    commands.add (CMDLayoutEditor_InsertLayout);
    commands.add (CMDLayoutEditor_InsertComponent);
    commands.add (CMDLayoutEditor_InsertSplitter);
    commands.add (CMDLayoutEditor_InsertSpacer);
}

void LayoutXMLEditor::getCommandInfo (CommandID commandID, ApplicationCommandInfo &result)
{
    switch (commandID) {
        case CMDLayoutEditor_New:
            result.setInfo ("New Layout", "Creates a new layout XML definition", "File", 0);
            result.defaultKeypresses.add (KeyPress ('n', ModifierKeys::commandModifier, 0));
            break;
        case CMDLayoutEditor_Open:
            result.setInfo ("Open Layout", "Opens a layout XML definition", "File", 0);
            result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier, 0));
            break;
        case CMDLayoutEditor_Save:
            result.setInfo ("Save Layout", "Save the current layout XML definition", "File", 0);
            result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier, 0));
            break;
        case CMDLayoutEditor_SaveAs:
            result.setInfo ("Save Layout as...", "Save the current layout XML definition under a new name", "File", 0);
            result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
            break;
        case StandardApplicationCommandIDs::del:
            result.setInfo ("Delete", "Delete the selected node", "Edit", 0);
            result.defaultKeypresses.add (KeyPress (KeyPress::deleteKey, 0, 0));
            break;
        case CMDLayoutEditor_Run:
            result.setInfo ("Open Layout", "Open a window showing the layout", "Run", 0);
            result.defaultKeypresses.add (KeyPress ('r', ModifierKeys::commandModifier, 0));
            break;
        case CMDLayoutEditor_Refresh:
            result.setInfo ("Refresh layout", "Recreate layout for XML", "Run", 0);
            result.defaultKeypresses.add (KeyPress ('b', ModifierKeys::commandModifier, 0));
            break;
        case CMDLayoutEditor_InsertLayout:
            result.setInfo ("Insert Layout", "Inserts a sub layout at selected position", "Insert", 0);
            result.defaultKeypresses.add (KeyPress ('l', ModifierKeys::commandModifier, 0));
            break;
        case CMDLayoutEditor_InsertComponent:
            result.setInfo ("Insert Component", "Inserts a component at selected position", "Insert", 0);
            break;
        case CMDLayoutEditor_InsertSplitter:
            result.setInfo ("Insert Splitter", "Inserts a splitter component at selected position", "Insert", 0);
            break;
        case CMDLayoutEditor_InsertSpacer:
            result.setInfo ("Insert Spacer", "Inserts a spacer element at selected position", "Insert", 0);
            break;

        default:
            break;
    }
}

bool LayoutXMLEditor::perform (const InvocationInfo &info)
{
    switch (info.commandID) {
        case CMDLayoutEditor_New:
            if (!codeDocument->getAllContent().isEmpty() &&
                !AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
                                               TRANS ("Clear editor"),
                                               TRANS ("Clear editor contents and start new?"))) {
                return true;
            }
            else {
                Layout layout (LayoutItem::LeftToRight);
                documentContent = layout.state;
                String templateText = documentContent.toXmlString();
                openedFile = File();

                codeDocument->replaceAllContent (templateText);

                updateFromCodeDocument();
                needsSaving = false;
            }
            break;
        case CMDLayoutEditor_Open:
            {
                WildcardFileFilter wildcardFilter ("*", String::empty, "Layout file");
                FileBrowserComponent browser (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                              File::nonexistent,
                                              &wildcardFilter,
                                              nullptr);
                FileChooserDialogBox dialogBox ("Open layout file",
                                                "Please choose a layout file...",
                                                browser,
                                                false,
                                                Colours::lightgrey);
                if (dialogBox.show())
                {
                    DBG (String ("Open: ") + browser.getSelectedFile (0).getFileName());
                    FileInputStream input (browser.getSelectedFile (0));
                    if (input.openedOk()) {
                        openedFile = browser.getSelectedFile (0);
                        codeDocument->loadFromStream (input);
                        codeDocument->clearUndoHistory();

                        updateFromCodeDocument();
                    }
                    needsSaving = false;
                    return true;
                }
            }
            break;
        case CMDLayoutEditor_Save:
            {
                FileOutputStream output (openedFile);
                if (output.openedOk()) {
                    output.setPosition (0);
                    output.truncate();
                    codeDocument->writeToStream (output);
                    needsSaving = false;
                    DBG (String ("Saved: ") + openedFile.getFileName());
                    return true;
                }
            }
        case CMDLayoutEditor_SaveAs:
            {
                WildcardFileFilter wildcardFilter ("*", String::empty, "Layout file");
                FileBrowserComponent browser (FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles,
                                              File::nonexistent,
                                              &wildcardFilter,
                                              nullptr);
                FileChooserDialogBox dialogBox ("Save layout file",
                                                "Please choose a layout file...",
                                                browser,
                                                false,
                                                Colours::lightgrey);
                if (dialogBox.show())
                {
                    DBG (String ("Save as: ") + browser.getSelectedFile (0).getFileName());
                    FileOutputStream output (browser.getSelectedFile (0));
                    if (output.openedOk()) {
                        output.setPosition (0);
                        output.truncate();
                        codeDocument->writeToStream (output);
                        needsSaving = false;
                    }
                    return true;
                }
            }
            break;
        case CMDLayoutEditor_Run:
            if (!previewWindow) {
                previewWindow = new PreviewComponent (TRANS ("Layout Preview"), Colours::darkgrey, DocumentWindow::allButtons);
                previewWindow->setSize (500, 350);
                previewWindow->setVisible (true);
            }
            previewWindow->setLayoutFromString (codeDocument->getAllContent());
            break;
        case CMDLayoutEditor_Refresh:
            codeDocument->replaceAllContent (documentContent.toXmlString());
            if (previewWindow) {
                previewWindow->setLayoutFromString (codeDocument->getAllContent());
            }
            collectLabelText (documentContent);
            break;
            
        // Insert methods
        case CMDLayoutEditor_InsertLayout:
            {
                if (LayoutTreeViewItem* item = dynamic_cast<LayoutTreeViewItem*> (layoutTree->getSelectedItem(0)))
                {
                    LayoutItem li (item->state);
                    if (li.isSubLayout()) {
                        LayoutItem::makeSubLayout (item->state, LayoutItem::LeftToRight);
                    }
                    else {
                        ValueTree parent = item->state.getParent();
                        if (parent.isValid()) {
                            int index = parent.indexOf (item->state);
                            LayoutItem::makeSubLayout (parent, LayoutItem::LeftToRight, index+1);
                        }
                    }
                }
            }
            break;
        case CMDLayoutEditor_InsertComponent:
            {
                if (LayoutTreeViewItem* item = dynamic_cast<LayoutTreeViewItem*> (layoutTree->getSelectedItem(0)))
                {
                    ValueTree node (LayoutItem::itemTypeComponent);
                    node.setProperty (LayoutItem::propComponentID, TRANS ("unknown"), nullptr);
                    LayoutItem li (item->state);
                    if (li.isSubLayout()) {
                        item->state.addChild (node, -1, nullptr);
                    }
                    else {
                        ValueTree parent = item->state.getParent();
                        if (parent.isValid()) {
                            int index = parent.indexOf (item->state);
                            parent.addChild (node, index+1, nullptr);
                        }
                    }
                }
            }
            break;
        case CMDLayoutEditor_InsertSplitter:
            {
                if (LayoutTreeViewItem* item = dynamic_cast<LayoutTreeViewItem*> (layoutTree->getSelectedItem(0)))
                {
                    LayoutItem li (item->state);
                    if (li.isSubLayout()) {
                        LayoutItem::makeChildSplitter (item->state, 0.5f);
                    }
                    else {
                        ValueTree parent = item->state.getParent();
                        if (parent.isValid()) {
                            int index = parent.indexOf (item->state);
                            LayoutItem::makeChildSplitter (parent, 0.5f, index+1);
                        }
                    }
                }
            }
            break;
        case CMDLayoutEditor_InsertSpacer:
            {
                if (LayoutTreeViewItem* item = dynamic_cast<LayoutTreeViewItem*> (layoutTree->getSelectedItem(0)))
                {
                    LayoutItem li (item->state);
                    if (li.isSubLayout()) {
                        LayoutItem::makeChildSpacer (item->state);
                    }
                    else {
                        ValueTree parent = item->state.getParent();
                        if (parent.isValid()) {
                            int index = parent.indexOf (item->state);
                            LayoutItem::makeChildSpacer (parent, 1.0, 1.0, index+1);
                        }
                    }
                }
            }
            break;
        case StandardApplicationCommandIDs::del:
            if (LayoutTreeViewItem* item = dynamic_cast<LayoutTreeViewItem*> (layoutTree->getSelectedItem(0))) {
                ValueTree parent = item->state.getParent();
                if (parent.isValid()) {
                    parent.removeChild (item->state, nullptr);
                }
            }
        default:
            break;
    }

    return true;
}

bool LayoutXMLEditor::getNeedsSaving () const
{
    return needsSaving;
}

void LayoutXMLEditor::updateTreeView ()
{
    if (LayoutTreeViewItem* itemView = dynamic_cast<LayoutTreeViewItem*> (layoutTree->getRootItem())) {
        itemView->setState (documentContent, this);
    }
    else {
        layoutTree->deleteRootItem();
        layoutTree->setRootItem (new LayoutTreeViewItem (documentContent, this));
    }
}


// properties view

void LayoutXMLEditor::getPropertiesForItem (const Identifier type, Array<Identifier>& props)
{
    if (type == LayoutItem::itemTypeSubLayout) {
        props.add (LayoutItem::propOrientation);
        props.add (LayoutItem::propMinWidth);
        props.add (LayoutItem::propMaxWidth);
        props.add (LayoutItem::propMinHeight);
        props.add (LayoutItem::propMaxHeight);
        props.add (LayoutItem::propAspectRatio);
        props.add (LayoutItem::propPaddingLeft);
        props.add (LayoutItem::propPaddingRight);
        props.add (LayoutItem::propPaddingTop);
        props.add (LayoutItem::propPaddingBottom);
        props.add (LayoutItem::propStretchX);
        props.add (LayoutItem::propStretchY);
        props.add (LayoutItem::propGroupName);
        props.add (LayoutItem::propGroupText);
        props.add (LayoutItem::propGroupJustification);
        props.add (LayoutItem::propOverlay);
        props.add (LayoutItem::propOverlayWidth);
        props.add (LayoutItem::propOverlayHeight);
        props.add (LayoutItem::propOverlayJustification);
    }
    else if (type == LayoutItem::itemTypeComponent) {
        props.add (LayoutItem::propComponentName);
        props.add (LayoutItem::propComponentID);
        props.add (LayoutItem::propLabelText);
        props.add (LayoutItem::propLabelFontSize);
        props.add (LayoutItem::propLabelJustification);
        props.add (LayoutItem::propMinWidth);
        props.add (LayoutItem::propMaxWidth);
        props.add (LayoutItem::propMinHeight);
        props.add (LayoutItem::propMaxHeight);
        props.add (LayoutItem::propAspectRatio);
        props.add (LayoutItem::propPaddingLeft);
        props.add (LayoutItem::propPaddingRight);
        props.add (LayoutItem::propPaddingTop);
        props.add (LayoutItem::propPaddingBottom);
        props.add (LayoutItem::propStretchX);
        props.add (LayoutItem::propStretchY);
        props.add (LayoutItem::propOverlay);
        props.add (LayoutItem::propOverlayWidth);
        props.add (LayoutItem::propOverlayHeight);
        props.add (LayoutItem::propOverlayJustification);
    }
    else if (type == LayoutItem::itemTypeSplitter) {
        props.add (LayoutSplitter::propComponentName);
        props.add (LayoutSplitter::propComponentID);
        props.add (LayoutSplitter::propRelativePosition);
        props.add (LayoutSplitter::propRelativeMinPosition);
        props.add (LayoutSplitter::propRelativeMaxPosition);
        props.add (LayoutItem::propMinWidth);
        props.add (LayoutItem::propMinWidth);
        props.add (LayoutItem::propMinWidth);
        props.add (LayoutItem::propMaxWidth);
        props.add (LayoutItem::propMinHeight);
        props.add (LayoutItem::propMaxHeight);
        props.add (LayoutItem::propAspectRatio);
        props.add (LayoutItem::propPaddingLeft);
        props.add (LayoutItem::propPaddingRight);
        props.add (LayoutItem::propPaddingTop);
        props.add (LayoutItem::propPaddingBottom);
        props.add (LayoutItem::propStretchX);
        props.add (LayoutItem::propStretchY);
    }
    else if (type == LayoutItem::itemTypeSpacer) {
        props.add (LayoutItem::propMinWidth);
        props.add (LayoutItem::propMaxWidth);
        props.add (LayoutItem::propMinHeight);
        props.add (LayoutItem::propMaxHeight);
        props.add (LayoutItem::propAspectRatio);
        props.add (LayoutItem::propPaddingLeft);
        props.add (LayoutItem::propPaddingRight);
        props.add (LayoutItem::propPaddingTop);
        props.add (LayoutItem::propPaddingBottom);
        props.add (LayoutItem::propStretchX);
        props.add (LayoutItem::propStretchY);
    }
}

void LayoutXMLEditor::updatePropertiesView (ValueTree state)
{
    nodeProperties->clear();
    Array<PropertyComponent*> properties;
    Array<Identifier> propNames;
    getPropertiesForItem (state.getType(), propNames);
    for (int i=0; i<propNames.size(); ++i) {
        Identifier propertyName = propNames.getUnchecked (i);
        if (propertyName == LayoutItem::propOrientation) {
            StringArray o;
            Array<var>  v;
            for (int i=0; i<5; ++i) {
                Identifier orientation = LayoutItem::getNameFromOrientation (LayoutItem::Orientation (i));
                o.add (orientation.toString());
                v.add (orientation.toString());
            }
            PropertyComponent* c = new ChoicePropertyComponent (state.getPropertyAsValue (propertyName, nullptr), propertyName.toString(), o, v);
            properties.add (c);
        }
        else if (propertyName == LayoutItem::propGroupJustification ||
                 propertyName == LayoutItem::propLabelJustification ||
                 propertyName == LayoutItem::propOverlayJustification )
        {
            StringArray o;
            Array<var>  v;
            o.add (TRANS ("top left"));
            v.add (Justification::topLeft);
            o.add (TRANS ("top"));
            v.add (Justification::top);
            o.add (TRANS ("top right"));
            v.add (Justification::topRight);
            o.add (TRANS ("left"));
            v.add (Justification::left);
            o.add (TRANS ("centred"));
            v.add (Justification::centred);
            o.add (TRANS ("right"));
            v.add (Justification::right);
            o.add (TRANS ("bottom left"));
            v.add (Justification::bottomLeft);
            o.add (TRANS ("bottom"));
            v.add (Justification::bottom);
            o.add (TRANS ("bottom right"));
            v.add (Justification::bottomRight);
            PropertyComponent* c = new ChoicePropertyComponent (state.getPropertyAsValue (propertyName, nullptr), propertyName.toString(), o, v);
            properties.add (c);
        }
        else if (propertyName == LayoutItem::propOverlay)
        {
            StringArray o;
            Array<var>  v;
            o.add (TRANS ("No overlay"));
            v.add (0);
            o.add (TRANS ("Previous"));
            v.add (1);
            o.add (TRANS ("Parent"));
            v.add (2);
            PropertyComponent* c = new ChoicePropertyComponent (state.getPropertyAsValue (propertyName, nullptr), propertyName.toString(), o, v);
            properties.add (c);
        }
        else if (propertyName == LayoutItem::propStretchX ||
                 propertyName == LayoutItem::propStretchY )
        {
            PropertyComponent* c = new SliderPropertyComponent (state.getPropertyAsValue (propertyName, nullptr), propertyName.toString(), -1.0f, 99.0f, 0.1f);
            properties.add (c);
        }
        else if (propertyName == LayoutItem::propOverlayWidth ||
                 propertyName == LayoutItem::propOverlayHeight )
        {
            PropertyComponent* c = new SliderPropertyComponent (state.getPropertyAsValue (propertyName, nullptr), propertyName.toString(), 0.0f, 2.0f, 0.01f);
            properties.add (c);
        }
        else if (propertyName == LayoutItem::propAspectRatio )
        {
            PropertyComponent* c = new SliderPropertyComponent (state.getPropertyAsValue (propertyName, nullptr), propertyName.toString(), 0.01f, 50.0f, 0.01f);
            properties.add (c);
        }
        else if (propertyName == LayoutItem::propMinWidth ||
                 propertyName == LayoutItem::propMaxWidth ||
                 propertyName == LayoutItem::propMinHeight ||
                 propertyName == LayoutItem::propMaxHeight ||
                 propertyName == LayoutItem::propPaddingLeft ||
                 propertyName == LayoutItem::propPaddingTop ||
                 propertyName == LayoutItem::propPaddingRight ||
                 propertyName == LayoutItem::propPaddingBottom )
        {
            PropertyComponent* c = new SliderPropertyComponent (state.getPropertyAsValue (propertyName, nullptr), propertyName.toString(), -1.0f, 2400.0f, 1.0f);
            properties.add (c);
        }
        else {
            PropertyComponent* c = new TextPropertyComponent (state.getPropertyAsValue (propertyName, nullptr), propertyName.toString(), 255, false);
            properties.add (c);
        }
    }
    nodeProperties->addProperties (properties);
}

void LayoutXMLEditor::collectLabelText (ValueTree node)
{
    if (node.hasProperty (LayoutItem::propLabelText)) {
        String labelText = node.getProperty (LayoutItem::propLabelText).toString();
        localisedStringsEditor->moveCaretToStartOfLine (false);
        localisedStringsEditor->moveCaretToEndOfLine (true);
        if (!localisedStringsEditor->getHighlightedText().isEmpty()) {
            localisedStringsEditor->moveCaretToEndOfLine (false);
            localisedStringsEditor->insertTextAtCaret ("\n");
        }
        localisedStringsEditor->insertTextAtCaret ("\"" + labelText + "\"");
        localisedStringsEditor->insertTextAtCaret (" = ");
        localisedStringsEditor->insertTextAtCaret ("\"" + labelText + "\"");
    }
    if (node.hasProperty (LayoutItem::propGroupText)) {
        String labelText = node.getProperty (LayoutItem::propGroupText).toString();
        localisedStringsEditor->moveCaretToStartOfLine (false);
        localisedStringsEditor->moveCaretToEndOfLine (true);
        if (!localisedStringsEditor->getHighlightedText().isEmpty()) {
            localisedStringsEditor->moveCaretToEndOfLine (false);
            localisedStringsEditor->insertTextAtCaret ("\n");
        }
        localisedStringsEditor->insertTextAtCaret ("\"" + labelText + "\"");
        localisedStringsEditor->insertTextAtCaret (" = ");
        localisedStringsEditor->insertTextAtCaret ("\"" + labelText + "\"");
    }
    for (int i=0; i<node.getNumChildren(); ++i) {
        collectLabelText (node.getChild (i));
    }
}

void LayoutXMLEditor::resized()
{
    if (layout) {
        layout->updateGeometry();
    }
}

void LayoutXMLEditor::codeDocumentTextInserted (const String &newText, int insertIndex)
{
    //updateFromCodeDocument();
}

void LayoutXMLEditor::codeDocumentTextDeleted (int startIndex, int endIndex)
{
    //updateFromCodeDocument();
}

void LayoutXMLEditor::updateFromCodeDocument ()
{
    XmlDocument doc (codeDocument->getAllContent());
    ScopedPointer<XmlElement> element = doc.getDocumentElement();
    if (element) {
        layoutTree->deleteRootItem();
        documentContent = ValueTree::fromXml (*element);
        layoutTree->setRootItem (new LayoutTreeViewItem (documentContent, this));
        if (previewWindow) {
            previewWindow->setLayoutFromString (codeDocument->getAllContent());
        }
    }
    needsSaving = true;
}


void LayoutXMLEditor::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    codeDocument->replaceAllContent (documentContent.toXmlString());
    layoutTree->repaint();
    if (previewWindow) {
        previewWindow->setLayoutFromString (codeDocument->getAllContent());
    }
    needsSaving = true;
}
void LayoutXMLEditor::valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded)
{
    updateTreeView();
    codeDocument->replaceAllContent (documentContent.toXmlString());
    if (previewWindow) {
        previewWindow->setLayoutFromString (codeDocument->getAllContent());
    }
    needsSaving = true;
}
void LayoutXMLEditor::valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved)
{
    updateTreeView();
    codeDocument->replaceAllContent (documentContent.toXmlString());
    if (previewWindow) {
        previewWindow->setLayoutFromString (codeDocument->getAllContent());
    }
    needsSaving = true;
}
void LayoutXMLEditor::valueTreeChildOrderChanged (ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex)
{
    updateTreeView();
    needsSaving = true;
}
void LayoutXMLEditor::valueTreeParentChanged (ValueTree &treeWhoseParentHasChanged)
{
    updateTreeView();
    needsSaving = true;
}

// toolbar factory
void LayoutXMLEditor::getAllToolbarItemIds (Array<int> &ids)
{
    ids.add (CMDLayoutEditor_InsertLayout);
    ids.add (CMDLayoutEditor_InsertComponent);
    ids.add (CMDLayoutEditor_InsertSplitter);
    ids.add (CMDLayoutEditor_InsertSpacer);
    ids.add (StandardApplicationCommandIDs::del);
}

void LayoutXMLEditor::getDefaultItemSet (Array<int> &ids)
{
    ids.add (CMDLayoutEditor_InsertLayout);
    ids.add (CMDLayoutEditor_InsertComponent);
    ids.add (CMDLayoutEditor_InsertSplitter);
    ids.add (CMDLayoutEditor_InsertSpacer);
    ids.add (StandardApplicationCommandIDs::del);
}

ToolbarItemComponent* LayoutXMLEditor::createItem (int itemId)
{
    ApplicationCommandManager* cm = LayoutEditorApplication::getApp()->getCommandManager();
    switch (itemId) {
        case CMDLayoutEditor_InsertLayout:
            {
                DrawableText* text = new DrawableText();
                text->setText (TRANS ("Layout"));
                DrawableText* textOff = new DrawableText();
                textOff->setText (TRANS ("Layout"));
                ToolbarButton* button = new ToolbarButton (itemId, TRANS ("Layout"), text, textOff);
                button->setCommandToTrigger (cm, itemId, true);
                return button;
            }
            break;
        case CMDLayoutEditor_InsertComponent:
            {
                DrawableText* text = new DrawableText();
                text->setText (TRANS ("Component"));
                DrawableText* textOff = new DrawableText();
                textOff->setText (TRANS ("Component"));
                ToolbarButton* button = new ToolbarButton (itemId, TRANS ("Component"), text, textOff);
                button->setCommandToTrigger (cm, itemId, true);
                return button;
            }
            break;
        case CMDLayoutEditor_InsertSplitter:
            {
                DrawableText* text = new DrawableText();
                text->setText (TRANS ("Splitter"));
                DrawableText* textOff = new DrawableText();
                textOff->setText (TRANS ("Splitter"));
                ToolbarButton* button = new ToolbarButton (itemId, TRANS ("Splitter"), text, textOff);
                button->setCommandToTrigger (cm, itemId, true);
                return button;
            }
            break;
        case CMDLayoutEditor_InsertSpacer:
            {
                DrawableText* text = new DrawableText();
                text->setText (TRANS ("Spacer"));
                DrawableText* textOff = new DrawableText();
                textOff->setText (TRANS ("Spacer"));
                ToolbarButton* button = new ToolbarButton (itemId, TRANS ("Spacer"), text, textOff);
                button->setCommandToTrigger (cm, itemId, true);
                return button;
            }
            break;
        case StandardApplicationCommandIDs::del:
            {
                DrawableText* text = new DrawableText();
                text->setText (TRANS ("Delete"));
                DrawableText* textOff = new DrawableText();
                textOff->setText (TRANS ("Delete"));
                ToolbarButton* button = new ToolbarButton (itemId, TRANS ("Delete"), text, textOff);
                button->setCommandToTrigger (cm, itemId, true);
                return button;
            }
            break;

        default:
            break;
    }
    return nullptr;
}

