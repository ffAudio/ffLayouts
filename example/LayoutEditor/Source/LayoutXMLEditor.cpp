/*
  ==============================================================================

    LayoutXMLEditor.cpp
    Created: 23 Apr 2016 6:53:29pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PreviewComponent.h"
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

    setSize (500, 300);
}

LayoutXMLEditor::~LayoutXMLEditor()
{
    if (previewWindow) {
        delete previewWindow;
    }
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
    commands.add (CMDLayoutEditor_Run);
    commands.add (CMDLayoutEditor_Refresh);
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
            result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier, 0));
            break;
        case CMDLayoutEditor_SaveAs:
            result.setInfo ("Save Layout as...", "Save the current layout XML definition under a new name", "File", 0);
            result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
            break;
        case CMDLayoutEditor_Run:
            result.setInfo ("Open Layout", "Open a window showing the layout", "Run", 0);
            result.defaultKeypresses.add (KeyPress ('r', ModifierKeys::commandModifier, 0));
            break;
        case CMDLayoutEditor_Refresh:
            result.setInfo ("Refresh layout", "Recreate layout for XML", "Run", 0);
            result.defaultKeypresses.add (KeyPress ('b', ModifierKeys::commandModifier, 0));
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
                String templateText = layout.state.toXmlString();
                openedFile = File();
                
                codeDocument->replaceAllContent (templateText);
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
                previewWindow = new DebugComponent (TRANS ("Layout Preview"), Colours::darkgrey, DocumentWindow::allButtons);
                previewWindow->setSize (500, 350);
                previewWindow->setVisible (true);
            }
            previewWindow->setLayoutFromString (codeDocument->getAllContent());
            break;
        default:
            break;
    }
    
    return true;
}


void LayoutXMLEditor::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    if (codeEditor) {
        codeEditor->setBounds (getLocalBounds());
    }
}
