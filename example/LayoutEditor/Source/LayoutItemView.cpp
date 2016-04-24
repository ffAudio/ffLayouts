/*
  ==============================================================================

    LayoutItemView.cpp
    Created: 24 Apr 2016 10:47:36pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LayoutXMLEditor.h"
#include "LayoutItemView.h"


LayoutItemView::LayoutItemView (ValueTree node, LayoutXMLEditor* _editor)
: state (node), editor (_editor)
{
    LayoutItem item (state);
    if (item.isSubLayout()) {
        for (int i=0; i<state.getNumChildren(); ++i) {
            addSubItem (new LayoutItemView (state.getChild (i), editor));
        }
    }
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
