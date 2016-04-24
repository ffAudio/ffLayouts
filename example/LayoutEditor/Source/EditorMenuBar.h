/*
  ==============================================================================

    EditorMenuBar.h
    Created: 23 Apr 2016 8:57:56pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef EDITORMENUBAR_H_INCLUDED
#define EDITORMENUBAR_H_INCLUDED


class EditorMenuBar : public MenuBarModel
{
public:
    EditorMenuBar();

    StringArray getMenuBarNames () override;
    
    PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName) override;
    
    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorMenuBar)
    
    
};


#endif  // EDITORMENUBAR_H_INCLUDED
