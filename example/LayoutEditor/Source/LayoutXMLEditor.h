/*
  ==============================================================================

    LayoutXMLEditor.h
    Created: 23 Apr 2016 6:53:29pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef LAYOUTXMLEDITOR_H_INCLUDED
#define LAYOUTXMLEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class DebugComponent;

//==============================================================================
/*
*/
class LayoutXMLEditor    : public Component, public ApplicationCommandTarget
{
public:
    LayoutXMLEditor();
    ~LayoutXMLEditor();

    ApplicationCommandTarget* getNextCommandTarget () override;
    void getAllCommands (Array< CommandID > &commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo &result) override;
    bool perform (const InvocationInfo &info) override;
    
    void resized() override;
    
    enum {
        CMDLayoutEditor_New = 0x1101,
        CMDLayoutEditor_Open,
        CMDLayoutEditor_Save,
        CMDLayoutEditor_SaveAs,
        CMDLayoutEditor_Run,
        CMDLayoutEditor_Refresh,
    };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutXMLEditor)
    
    File                               openedFile;
    
    ScopedPointer<CodeDocument>        codeDocument;
    ScopedPointer<CodeEditorComponent> codeEditor;
    ScopedPointer<CodeTokeniser>       codeTokeniser;
    
    SafePointer<DebugComponent>        previewWindow;
    
};


#endif  // LAYOUTXMLEDITOR_H_INCLUDED
