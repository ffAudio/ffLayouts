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
#include "LayoutItemView.h"
#include "LayoutXMLEditor.h"

//==============================================================================
LayoutXMLEditor::LayoutXMLEditor()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    codeTokeniser = new XmlTokeniser;
    codeDocument  = new CodeDocument;
    codeEditor    = new CodeEditorComponent (*codeDocument, codeTokeniser);
    addAndMakeVisible (codeEditor);
    
    layoutTree    = new TreeView;
    addAndMakeVisible (layoutTree);
    
    nodeProperties = new PropertyPanel;
    addAndMakeVisible (nodeProperties);

    Label* text = new Label;
    text->setText (TRANS ("Properties"), dontSendNotification);
    addAndMakeVisible (text);
    
    layout = new Layout (LayoutItem::LeftToRight, this);
    LayoutItem::makeChildComponent (layout->state, codeEditor);
    LayoutItem::makeChildSplitter  (layout->state, 0.7f);
    LayoutItem right = LayoutItem::makeSubLayout (layout->state, LayoutItem::TopDown);
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
                
                XmlDocument doc (templateText);
                ScopedPointer<XmlElement> element = doc.getDocumentElement();
                if (element) {
                    layoutTree->deleteRootItem();
                    documentContent = ValueTree::fromXml (*element);
                    layoutTree->setRootItem (new LayoutItemView (documentContent, this));
                }
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
                        
                        XmlDocument doc (codeDocument->getAllContent());
                        ScopedPointer<XmlElement> element = doc.getDocumentElement();
                        if (element) {
                            layoutTree->deleteRootItem();
                            documentContent = ValueTree::fromXml (*element);
                            layoutTree->setRootItem (new LayoutItemView (documentContent, this));
                        }
                    }
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
            break;
            
        // Insert methods
        case CMDLayoutEditor_InsertLayout:
            {
                if (LayoutItemView* item = dynamic_cast<LayoutItemView*> (layoutTree->getSelectedItem(0)))
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
                if (LayoutItemView* item = dynamic_cast<LayoutItemView*> (layoutTree->getSelectedItem(0)))
                {
                    ValueTree node (LayoutItem::itemTypeComponent);
                    node.setProperty (LayoutItem::propComponentID, TRANS ("unknown"), nullptr);
                    LayoutItem li (item->state);
                    if (li.isSubLayout()) {
                        item->state.addChild (node, -1, nullptr);
                    }
                    else if (item->state.getParent().isValid()) {
                        int index = item->state.getParent().indexOf (item->state);
                        item->state.addChild (node, index+1, nullptr);
                    }
                }
            }
            break;
        case CMDLayoutEditor_InsertSplitter:
            {
                if (LayoutItemView* item = dynamic_cast<LayoutItemView*> (layoutTree->getSelectedItem(0)))
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
                if (LayoutItemView* item = dynamic_cast<LayoutItemView*> (layoutTree->getSelectedItem(0)))
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
            if (LayoutItemView* item = dynamic_cast<LayoutItemView*> (layoutTree->getSelectedItem(0))) {
                ValueTree parent = item->state.getParent();
                if (parent.isValid()) {
                    parent.removeChild (item->state, nullptr);
                    TreeViewItem* parentView = item->getParentItem();
                    if (parentView) {
                        for (int i=0; i<parentView->getNumSubItems(); ++i ) {
                            if (parentView->getSubItem (i) == item) {
                                parentView->removeSubItem (i);
                                break;
                            }
                        }
                    }
                }
            }
            
        default:
            break;
    }
    
    return true;
}

void LayoutXMLEditor::updateTreeView ()
{
    if (LayoutItemView* itemView = dynamic_cast<LayoutItemView*> (layoutTree->getRootItem())) {
        itemView->setState (documentContent, this);
    }
    else {
        layoutTree->deleteRootItem();
        layoutTree->setRootItem (new LayoutItemView (documentContent, this));
    }
}

void LayoutXMLEditor::updatePropertiesView (ValueTree state)
{
    nodeProperties->clear();
    Array<PropertyComponent*> properties;
    for (int i=0; i<state.getNumProperties(); ++i) {
        String propertyName = state.getPropertyName (i).toString();
        PropertyComponent* c = new TextPropertyComponent (state.getPropertyAsValue (propertyName, nullptr), propertyName, 255, false);
        properties.add (c);
    }
    nodeProperties->addProperties (properties);
}

void LayoutXMLEditor::resized()
{
    if (layout) {
        layout->updateGeometry();
    }
}

void LayoutXMLEditor::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    codeDocument->replaceAllContent (documentContent.toXmlString());
    if (previewWindow) {
        previewWindow->setLayoutFromString (codeDocument->getAllContent());
    }
}
void LayoutXMLEditor::valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded)
{
    updateTreeView();
    codeDocument->replaceAllContent (documentContent.toXmlString());
    if (previewWindow) {
        previewWindow->setLayoutFromString (codeDocument->getAllContent());
    }
}
void LayoutXMLEditor::valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved)
{
    updateTreeView();
    codeDocument->replaceAllContent (documentContent.toXmlString());
    if (previewWindow) {
        previewWindow->setLayoutFromString (codeDocument->getAllContent());
    }
}
void LayoutXMLEditor::valueTreeChildOrderChanged (ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex)
{
    updateTreeView();
    
}
void LayoutXMLEditor::valueTreeParentChanged (ValueTree &treeWhoseParentHasChanged)
{
    updateTreeView();
    
}

