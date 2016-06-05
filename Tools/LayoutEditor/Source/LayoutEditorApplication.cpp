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

    LayoutEditorApplication.cpp
    Created: 24 Apr 2016 12:38:31pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LayoutTreeView.h"
#include "LayoutXMLEditor.h"
#include "EditorMenuBar.h"
#include "LayoutEditorApplication.h"


    
const String LayoutEditorApplication::getApplicationName()        { return ProjectInfo::projectName; }
const String LayoutEditorApplication::getApplicationVersion()     { return ProjectInfo::versionString; }
bool LayoutEditorApplication::moreThanOneInstanceAllowed()        { return true; }

//==============================================================================
void LayoutEditorApplication::initialise (const String& commandLine)
{
    commandManager = new ApplicationCommandManager();
    menuBar = new EditorMenuBar;
    menuBar->setApplicationCommandManagerToWatch (commandManager);
    
    mainWindow = new MainWindow (getApplicationName());
    mainWindow->setResizable (true, false);
    commandManager->registerAllCommandsForTarget (this);
    commandManager->registerAllCommandsForTarget (mainWindow->getApplicationCommandTarget());
    
    mainWindow->addKeyListener (commandManager->getKeyMappings());
    
#ifdef __APPLE__
    MenuBarModel::setMacMainMenu (menuBar);
#else
    mainWindow->setMenuBar (menuBar);
#endif

}

void LayoutEditorApplication::shutdown()
{
    // Add your application's shutdown code here..
#ifdef __APPLE__
    MenuBarModel::setMacMainMenu (nullptr);
#endif
    
    mainWindow = nullptr; // (deletes our window)
}

ApplicationCommandManager* LayoutEditorApplication::getCommandManager()
{
    return commandManager;
}

LayoutEditorApplication* LayoutEditorApplication::getApp() {
    return dynamic_cast<LayoutEditorApplication*> (JUCEApplication::getInstance());
}

//==============================================================================
void LayoutEditorApplication::systemRequestedQuit()
{
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.

    if (mainWindow && mainWindow->editorNeedsSaving()) {
        int result = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon, TRANS ("Save layout before Quit..."), TRANS ("Save layout before Quit?\nYes to save, no to discard changes and Cancel to continue editing"));

        if (result == 0) {
            // cancel quit
            return;
        }
        else if (result == 1) {
            // call save
            commandManager->invokeDirectly (LayoutXMLEditor::CMDLayoutEditor_Save, false);
        }
    }
    
    quit();
}

void LayoutEditorApplication::anotherInstanceStarted (const String& commandLine)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
}

//==============================================================================

LayoutEditorApplication::MainWindow::MainWindow (String name)  : DocumentWindow (name,
                                                                                 Colours::lightgrey,
                                                                                 DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
    editorComponent = new LayoutXMLEditor();
    setContentNonOwned (editorComponent, true);
    
    centreWithSize (getWidth(), getHeight());
    setVisible (true);
}

void LayoutEditorApplication::MainWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

bool LayoutEditorApplication::MainWindow::editorNeedsSaving() const
{
    if (editorComponent) {
        return editorComponent->getNeedsSaving();
    }
    return false;
}

ApplicationCommandTarget* LayoutEditorApplication::MainWindow::getApplicationCommandTarget ()
{
    return editorComponent;
}

