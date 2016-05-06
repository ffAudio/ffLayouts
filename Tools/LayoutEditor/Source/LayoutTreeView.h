/*
  ==============================================================================

    LayoutTreeView.h
    Created: 5 May 2016 10:17:41pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef LAYOUTTREEVIEW_H_INCLUDED
#define LAYOUTTREEVIEW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class LayoutTreeView    : public TreeView
{
public:
    LayoutTreeView ();
    ~LayoutTreeView();

    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutTreeView)

    ValueTree state;
};


#endif  // LAYOUTTREEVIEW_H_INCLUDED
