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

    EditorMenuBar.cpp
    Created: 24 Apr 2016 12:33:53pm
    Author:  Daniel Walz

  ==============================================================================
*/


#include "../JuceLibraryCode/JuceHeader.h"
#include "LayoutEditorApplication.h"
#include "LayoutTreeViewItem.h"
#include "LayoutXMLEditor.h"
#include "EditorMenuBar.h"


EditorMenuBar::EditorMenuBar() {}

StringArray EditorMenuBar::getMenuBarNames ()
{
    StringArray names;
    names.add (TRANS ("File"));
    names.add (TRANS ("Edit"));
    names.add (TRANS ("Insert"));
    names.add (TRANS ("Preview"));
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
        menu.addCommandItem (cm, StandardApplicationCommandIDs::del);
    }
    else if (topLevelMenuIndex == 2) {
        // insert menu
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_InsertLayout);
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_InsertComponent);
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_InsertSplitter);
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_InsertSpacer);
    }
    else if (topLevelMenuIndex == 3) {
        // Run menu
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_Run);
        menu.addCommandItem (cm, LayoutXMLEditor::CMDLayoutEditor_Refresh);
    }
    return menu;
    
}

void EditorMenuBar::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{
}

