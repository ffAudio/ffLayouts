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

    LayoutXMLEditor.h
    Created: 23 Apr 2016 6:53:29pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef LAYOUTXMLEDITOR_H_INCLUDED
#define LAYOUTXMLEDITOR_H_INCLUDED


class PreviewComponent;
class LayoutTreeView;

//==============================================================================
/*
*/
class LayoutXMLEditor : public Component,
                        public ApplicationCommandTarget,
                        public ValueTree::Listener,
                        public CodeDocument::Listener,
                        private ToolbarItemFactory
{
public:
    LayoutXMLEditor();
    ~LayoutXMLEditor();

    ApplicationCommandTarget* getNextCommandTarget () override;
    void getAllCommands (Array< CommandID > &commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo &result) override;
    bool perform (const InvocationInfo &info) override;

    bool getNeedsSaving () const;

    void updateTreeView ();
    
    // properties view

    void updatePropertiesView (ValueTree state);

    void collectLabelText (ValueTree node);
    
    void getPropertiesForItem (const Identifier type, Array<Identifier>& props);

    void resized() override;

    void codeDocumentTextInserted (const String &newText, int insertIndex) override;
    void codeDocumentTextDeleted (int startIndex, int endIndex) override;

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
        CMDLayoutEditor_InsertLayout,
        CMDLayoutEditor_InsertComponent,
        CMDLayoutEditor_InsertSplitter,
        CMDLayoutEditor_InsertSpacer
    };

    // toolbar factory
    void getAllToolbarItemIds (Array<int> &ids) override;
    void getDefaultItemSet (Array<int> &ids) override;
    ToolbarItemComponent* createItem (int itemId) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutXMLEditor)

    void updateFromCodeDocument();

    File                               openedFile;

    bool                               needsSaving;

    ValueTree                          documentContent;
    
    ScopedPointer<CodeDocument>        codeDocument;
    ScopedPointer<CodeEditorComponent> codeEditor;
    ScopedPointer<CodeTokeniser>       codeTokeniser;
    
    ScopedPointer<Toolbar>             insertButtons;
    ScopedPointer<LayoutTreeView>      layoutTree;
    ScopedPointer<PropertyPanel>       nodeProperties;

    ScopedPointer<TextEditor>          localisedStringsEditor;
    
    ScopedPointer<Layout>              layout;
    
    SafePointer<PreviewComponent>      previewWindow;

};


#endif  // LAYOUTXMLEDITOR_H_INCLUDED
