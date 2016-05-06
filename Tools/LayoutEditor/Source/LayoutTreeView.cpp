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

}

LayoutTreeView::~LayoutTreeView()
{
}

bool LayoutTreeView::isInterestedInDragSource (const SourceDetails &dragSourceDetails)
{
    return true;
}
