/*
  ==============================================================================

    LayoutXMLEditor.h
    Created: 23 Apr 2016 6:53:29pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef LAYOUTXMLEDITOR_H_INCLUDED
#define LAYOUTXMLEDITOR_H_INCLUDED


class PreviewComponent;
class LayoutItemView;

//==============================================================================
/*
*/
class LayoutXMLEditor : public Component,
                        public ApplicationCommandTarget,
                        public ValueTree::Listener
{
public:
    LayoutXMLEditor();
    ~LayoutXMLEditor();

    ApplicationCommandTarget* getNextCommandTarget () override;
    void getAllCommands (Array< CommandID > &commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo &result) override;
    bool perform (const InvocationInfo &info) override;

    void updateTreeView ();
    
    void updatePropertiesView (ValueTree state);

    void resized() override;

    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;
    void valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded) override;
    void valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
    void valueTreeChildOrderChanged (ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
    void valueTreeParentChanged (ValueTree &treeWhoseParentHasChanged) override;
    
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
    
    ValueTree                          documentContent;
    
    ScopedPointer<CodeDocument>        codeDocument;
    ScopedPointer<CodeEditorComponent> codeEditor;
    ScopedPointer<CodeTokeniser>       codeTokeniser;
    
    ScopedPointer<TreeView>            layoutTree;
    ScopedPointer<PropertyPanel>       nodeProperties;
    
    ScopedPointer<Layout>              layout;
    
    SafePointer<PreviewComponent>      previewWindow;
    
};


#endif  // LAYOUTXMLEDITOR_H_INCLUDED
