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
    class MainWindow    : public DocumentWindow, public DragAndDropContainer
    {
    public:
        MainWindow (String name);
        
        void closeButtonPressed() override;

        bool editorNeedsSaving() const;
        
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
