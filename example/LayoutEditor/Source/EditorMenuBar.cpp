/*
  ==============================================================================

    EditorMenuBar.cpp
    Created: 24 Apr 2016 12:33:53pm
    Author:  Daniel Walz

  ==============================================================================
*/


#include "../JuceLibraryCode/JuceHeader.h"
#include "LayoutEditorApplication.h"
#include "LayoutItemView.h"
#include "LayoutXMLEditor.h"
#include "EditorMenuBar.h"


EditorMenuBar::EditorMenuBar() {}

StringArray EditorMenuBar::getMenuBarNames ()
{
    StringArray names;
    names.add (TRANS ("File"));
    names.add (TRANS ("Edit"));
    names.add (TRANS ("Run"));
    return names;
}

PopupMenu EditorMenuBar::getMenuForIndex (int topLevelMenuIndex, const String& menuName)
{
    PopupMenu menu;
    ApplicationCommandManager* cm = LayoutEditorApplication::getApp()->getCommandManager();
    
    if (topLevelMenuIndex == 0) {
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_New);
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_Open);
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_Save);
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_SaveAs);
#ifndef __APPLE__
        menu.addSeparator();
        menu.addCommandItem (cm, StandardApplicationCommandIDs::quit);
#endif
    }
    else if (topLevelMenuIndex == 1) {
        
    }
    else if (topLevelMenuIndex == 2) {
        // Run menu
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_Run);
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_Refresh);
    }
    return menu;
    
}

void EditorMenuBar::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{
}

