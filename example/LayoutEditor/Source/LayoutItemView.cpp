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

    LayoutItemView.cpp
    Created: 24 Apr 2016 10:47:36pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LayoutXMLEditor.h"
#include "LayoutItemView.h"


LayoutItemView::LayoutItemView (ValueTree _state, LayoutXMLEditor* _editor)
  : state (_state), editor (_editor)
{
    setState (state, editor);
}

void LayoutItemView::setState (ValueTree _state, LayoutXMLEditor* _editor)
{
    OpennessRestorer restorer (*this);
    state  = _state;
    editor = _editor;
    LayoutItem item (state);
    clearSubItems();
    if (item.isSubLayout()) {
        for (int i=0; i<state.getNumChildren(); ++i) {
            addSubItem (new LayoutItemView (state.getChild (i), editor));
        }
    }
}


String LayoutItemView::getUniqueName () const
{
    String name = state.getType().toString();
    if (state.getType() == LayoutItem::itemTypeSubLayout) {
        name += state.getProperty (LayoutItem::propOrientation).toString();
    }
    if (state.hasProperty (LayoutItem::propComponentID)) {
        name += state.getProperty (LayoutItem::propComponentID).toString();
    }
    if (state.hasProperty (LayoutItem::propComponentName)) {
        name += state.getProperty (LayoutItem::propComponentName).toString();
    }
    
    
    return name;
}

void LayoutItemView::paintItem (Graphics &g, int width, int height)
{
    LayoutItem item (state);
    if (item.isSubLayout()) {
        g.drawText (LayoutItem::getNameFromOrientation (item.getOrientation()).toString(), 0, 0, width, height, Justification::left);
    }
    else if (item.isSplitterItem()) {
        g.drawText (TRANS ("Splitter"), 0, 0, width, height, Justification::left);
    }
    else if (item.isSpacerItem()) {
        g.drawText (TRANS ("Spacer"), 0, 0, width, height, Justification::left);
    }
    else if (item.isComponentItem()) {
        String componentID = item.getComponentID();
        if (!componentID.isEmpty()) {
            g.drawText (String ("ID:") + componentID, 0, 0, width, height, Justification::left);
        }
        else {
            String labelText   = state.getProperty ("labelText", "");
            Graphics::ScopedSaveState save (g);
            Font f = g.getCurrentFont();
            f.setItalic (true);
            g.setFont (f);
            g.drawText (String ("\"") + labelText + String ("\""), 0, 0, width, height, Justification::left);
        }
    }
}

bool LayoutItemView::mightContainSubItems ()
{
    LayoutItem item (state);
    return item.isSubLayout();
}

void LayoutItemView::itemSelectionChanged (bool isNowSelected)
{
    if (editor && isNowSelected) {
        editor->updatePropertiesView (state);
    }
}
