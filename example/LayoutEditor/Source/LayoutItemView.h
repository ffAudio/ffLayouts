/*
  ==============================================================================

    LayoutItemView.h
    Created: 24 Apr 2016 10:47:36pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef LAYOUTITEMVIEW_H_INCLUDED
#define LAYOUTITEMVIEW_H_INCLUDED

class LayoutXMLEditor;

class LayoutItemView : public TreeViewItem
{
public:
    LayoutItemView (ValueTree node, LayoutXMLEditor* editor);
    
    void paintItem (Graphics &g, int width, int height) override;
    
    bool mightContainSubItems () override;
    
    void itemSelectionChanged (bool isNowSelected) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutItemView)
    
    ValueTree state;
    
    Component::SafePointer<LayoutXMLEditor> editor;
};



#endif  // LAYOUTITEMVIEW_H_INCLUDED
