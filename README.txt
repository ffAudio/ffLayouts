/*******************************************************************************
    juce module: ff_layout
    author:      Daniel Walz

    started:     Feb, 2016
    
*******************************************************************************/


This project adds a simple to use layout class to align components of the JUCE
framework (www.juce.com) next to each others.

Find the api-documentation here: https://ffAudio.github.io/ffLayouts/

The highlights:
In the Tools folder is a LayoutEditor. You can create layouts as XML definitions
and show them with placeholders to see how the space would be distributed.

To use the layout in a JUCE component it is easiest to add a Layout member to
the component, load the layout definition via ValueTree, e.g. from an XML file
from BinaryData.
Call in the resized() callback layout->updateGeometry() and all child components
are layouted according to the XML definition.

Other features:
Only one member variable necessary
Self contained with SafePointers to the components to minimize risk of dangling pointers
Stretch factors to distribute the available space proportionally
Restrict minimum and maximum sizes of items
Add sublayouts nested horizontally and vertically in each directions
Add labels and GroupComponents from XML definition
Movable splitters to split layouts interactively
Add spacers between components

Have fun using it...
Daniel
