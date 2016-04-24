/*
  ==============================================================================

    LayoutEditorApplication.h
    Created: 24 Apr 2016 12:38:31pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef LAYOUTEDITORAPPLICATION_H_INCLUDED
#define LAYOUTEDITORAPPLICATION_H_INCLUDED

class LayoutXMLEditor;

class LayoutEditorApplication  : public JUCEApplication
{
public:
    //==============================================================================
    LayoutEditorApplication() {}
    
    const String getApplicationName() override;
    const String getApplicationVersion() override;
    bool moreThanOneInstanceAllowed() override;
    
    //==============================================================================
    void initialise (const String& commandLine) override;
    
    void shutdown() override;
    
    ApplicationCommandManager* getCommandManager();
    
    static LayoutEditorApplication* getApp();
    
    //==============================================================================
    void systemRequestedQuit() override;
    
    void anotherInstanceStarted (const String& commandLine) override;
    
    //==============================================================================
    /*
     This class implements the desktop window that contains an instance of
     our MainContentComponent class.
     */
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow (String name);
        
        void closeButtonPressed() override;
        
        /* Note: Be careful if you override any DocumentWindow methods - the base
         class uses a lot of them, so by overriding you might break its functionality.
         It's best to do all your work in your content component instead, but if
         you really have to override any DocumentWindow methods, make sure your
         subclass also calls the superclass's method.
         */
        
        ApplicationCommandTarget* getApplicationCommandTarget ();

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
        
        ScopedPointer<LayoutXMLEditor>           editorComponent;
    };
    
private:
    ScopedPointer<ApplicationCommandManager> commandManager;
    ScopedPointer<MenuBarModel>              menuBar;
    ScopedPointer<MainWindow>                mainWindow;

};

#endif  // LAYOUTEDITORAPPLICATION_H_INCLUDED
