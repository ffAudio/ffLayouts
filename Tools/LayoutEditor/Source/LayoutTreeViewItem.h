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

    LayoutTreeViewItem.h
    Created: 24 Apr 2016 10:47:36pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef LAYOUTTREEVIEWITEM_H_INCLUDED
#define LAYOUTTREEVIEWITEM_H_INCLUDED

class LayoutXMLEditor;

class LayoutTreeViewItem : public TreeViewItem//, public DragAndDropTarget
{
public:
    LayoutTreeViewItem (ValueTree node, LayoutXMLEditor* editor);
    
    void setState (ValueTree, LayoutXMLEditor* editor);
    
    String getUniqueName () const override;
    
    void paintItem (Graphics &g, int width, int height) override;
    
    bool mightContainSubItems () override;
    
    void itemSelectionChanged (bool isNowSelected) override;

    void itemClicked (const MouseEvent& event) override;

    var getDragSourceDescription () override;

    bool isInterestedInDragSource (const DragAndDropTarget::SourceDetails& dragSourceDetails) override;

    void itemDropped (const DragAndDropTarget::SourceDetails& dragSourceDetails, int insertIndex) override;

    ValueTree state;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutTreeViewItem)
    
    Component::SafePointer<LayoutXMLEditor> editor;
};



#endif  // LAYOUTTREEVIEWITEM_H_INCLUDED
