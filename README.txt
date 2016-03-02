/*******************************************************************************
    juce module: juce_ak_layout
    author:      Daniel Walz

    started:     Feb, 2016
    
*******************************************************************************/


This project adds a simple to use layout class to align components of the JUCE
framework (www.juce.com) next to each others.

The highlights:
Only one member variable necessary
Self contained with SafePointers to the components to minimize risk of dangling pointers
Stretch factors to distribute the available space proportionally
Restrict minimum and maximum sizes of items
Add sublayouts nested horizontally and vertically in each directions
Add a labeled component in one line of code
Add spacers between components

Have fun using it...
Daniel
