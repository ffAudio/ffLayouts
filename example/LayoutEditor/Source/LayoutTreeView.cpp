/*
  ==============================================================================

    LayoutTreeView.cpp
    Created: 5 May 2016 10:17:41pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LayoutTreeViewItem.h"
#include "LayoutTreeView.h"

//==============================================================================
LayoutTreeView::LayoutTreeView ()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

LayoutTreeView::~LayoutTreeView()
{
}

bool LayoutTreeView::isInterestedInDragSource (const SourceDetails &dragSourceDetails)
{
    return true;
}

void LayoutTreeView::itemDropped (const SourceDetails &dragSourceDetails)
{
    DBG ("Drop View: " + dragSourceDetails.description.toString());
    LayoutTreeViewItem* target = dynamic_cast<LayoutTreeViewItem*> (getItemAt (dragSourceDetails.localPosition.getY()));
    LayoutTreeViewItem* toMove = dynamic_cast<LayoutTreeViewItem*> (getSelectedItem (0));
    if (toMove != nullptr && target != nullptr) {
        ValueTree targetNode = target->state;
        ValueTree moveNode   = toMove->state;
        ValueTree moveParent = moveNode.getParent();
        if (moveParent.isValid()) {
            int index = 0;
            ValueTree targetLayout = targetNode;
            if (targetNode.getType() != LayoutItem::itemTypeSubLayout) {
                targetLayout = targetNode.getParent();
                index = targetLayout.indexOf (targetNode);
            }
            if (moveNode.isAChildOf (targetNode)) {
                // moved into itself
                return;
            }
            moveParent.removeChild (moveNode, nullptr);
            targetLayout.addChild (moveNode, index, nullptr);
        }
    }
    repaint();
}

