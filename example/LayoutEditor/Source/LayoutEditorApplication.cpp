/*
  ==============================================================================

    LayoutEditorApplication.cpp
    Created: 24 Apr 2016 12:38:31pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
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

ApplicationCommandTarget* LayoutEditorApplication::MainWindow::getApplicationCommandTarget ()
{
    return editorComponent;
}

