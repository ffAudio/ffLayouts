/*
 ==============================================================================
 
 Copyright (c) 2016 - 2017, Daniel Walz
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
 
 BEGIN_JUCE_MODULE_DECLARATION
 
 ID:            ff_layout
 vendor:        foleysfinest.com
 version:       0.9.2
 name:          Dynamic layout
 description:   Enables to layout gui components defined in a xml document
 dependencies:  juce_core, juce_data_structures, juce_gui_basics, juce_gui_extra
 website:       https://github.com/audiokinematics/juceLayouts
 license:       BSD 3-clause
 
 END_JUCE_MODULE_DECLARATION
 
 ==============================================================================
 */

#pragma once

/**
 \mainpage
 <h1>Layout for juce</h1>
 
 This JUCE module provides a simple to use layout mechanism. The items to be laid out are LayoutItems.
 Each LayoutItem can be either a sub layout, a spacer to leave space or a component item which refers to a
 juce::Component. When Layout::updateGeometry is called, the calculated bounds of the item is then
 transfered to the component via setBounds.
 @see LayoutItem
 
 <p>
 The major benefit is, that the layout can be defined as xml file and load on creation of the
 GUI elements. So the GUI can be rearranged without changing the implementation. Different XML
 files dependent on screen dimensions are also feasible.
 
 <p>
 The basic usage is to add a Layout instance to your parent component where you want to lay the children in.
 This component doesn't need to be the top most component, it also works for sub components.
 Use the Constructor providing a this pointer to the component, so the overall bounds can be used.
 @see Layout
 
 <p>
 If you use automatically created and owned components like labels and splitters, you should call
 Layout::realize() at the end of the constructor i.e. after loading and the components are added as
 children to owningComponent.
 
 <p>
 Add a call to updateGeometry() in your component's resized() method.
 @see Layout::updateGeometry()
 
 */


#include "ff_layoutCore.h"
#include "ff_layoutItem.h"
