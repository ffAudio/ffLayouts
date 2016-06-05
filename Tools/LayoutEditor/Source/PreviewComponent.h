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

    PreviewComponent.h
    Created: 15 Mar 2016 4:20:15pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef PREVIEWCOMPONENT_H_INCLUDED
#define PREVIEWCOMPONENT_H_INCLUDED


//==============================================================================
/*
*/
class PreviewComponent    : public DocumentWindow
{
public:
    PreviewComponent(const String &  	name,
                     Colour backgroundColour,
                     int  	requiredButtons,
                     bool  	addToDesktop = true )
      : DocumentWindow (name, backgroundColour, requiredButtons, addToDesktop),
        showBounds (true)
    {
        setUsingNativeTitleBar (true);
        setColour (backgroundColourId, Colours::darkslateblue);
        
        mainComponent = new Component();
        mainComponent->setSize (500, 380);
        setContentNonOwned (mainComponent, true);
        centreWithSize (getWidth(), getHeight());
        mainComponent->setVisible (true);
        setResizable (true, false);
    }

    ~PreviewComponent()
    {
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (findColour (backgroundColourId));
    }

    // creates a label for each layout item that references a component
    void createDummyComponents (ValueTree& tree)
    {
        if (tree.hasProperty ("componentName")) {
            String componentName = tree.getProperty ("componentName");
            Label* newComponent = new Label (componentName);
            if (tree.hasProperty ("componentID")) {
                String componentID = tree.getProperty ("componentID");
                newComponent->setComponentID (componentID);
                newComponent->setText ("ID:" + componentID + " Name:" + componentName, dontSendNotification);
            }
            else {
                newComponent->setText ("Name:" + componentName, dontSendNotification);
            }
            int hashCode = componentName.hashCode();
            newComponent->setColour (Label::backgroundColourId, Colour::fromRGB (hashCode % 128 + 127, (hashCode / 255) % 128 + 127, (hashCode / 1024) % 128 + 127));
            newComponent->setJustificationType (Justification::centred);
            mainComponent->addAndMakeVisible (newComponent);
            testComponents.add (newComponent);
        }
        else if (tree.hasProperty ("componentID")) {
            String componentID = tree.getProperty ("componentID");
            Label* newComponent = new Label ();
            newComponent->setComponentID (componentID);
            newComponent->setText ("ID:" + componentID, dontSendNotification);
            newComponent->setJustificationType (Justification::centred);
            int hashCode = componentID.hashCode();
            newComponent->setColour (Label::backgroundColourId, Colour::fromRGB (hashCode % 128 + 127, (hashCode / 255) % 128 + 127, (hashCode / 1024) % 128 + 127));
            mainComponent->addAndMakeVisible (newComponent);
            testComponents.add (newComponent);
        }
        for (int i=0; i<tree.getNumChildren(); ++i) {
            ValueTree child = tree.getChild (i);
            createDummyComponents (child);
        }
    }
    
    // load a layout file and apply it
    void loadLayout (File file)
    {
        FileInputStream input (file);
        // load a xml layout and create a dummy component for each componentID or componentName found
        String layoutCode = input.readEntireStreamAsString ();
        
        // read the ValueTree and create dummy components
        juce::ScopedPointer<juce::XmlElement> mainElement = juce::XmlDocument::parse (layoutCode);
        if (mainElement) {
            juce::ValueTree myLoadedTree = juce::ValueTree::fromXml (*mainElement);

            // for this debugger create for each component of the xml a label to display as placeholder
            createDummyComponents (myLoadedTree);
        }
        
        layout = new Layout (layoutCode, mainComponent);
    }
    
    void setLayoutFromString (String code)
    {
        // read the ValueTree and create dummy components
        juce::ScopedPointer<juce::XmlElement> mainElement = juce::XmlDocument::parse (code);
        if (mainElement) {
            juce::ValueTree myLoadedTree = juce::ValueTree::fromXml (*mainElement);
            
            // for this debugger create for each component of the xml a label to display as placeholder
            createDummyComponents (myLoadedTree);
        
            layout = new Layout (code, mainComponent);
            layout->updateGeometry();
        }
    }
    
    void closeButtonPressed () override
    {
        delete this;
    }
    
    // set showBounds to true to paint the bounds of individual layouts
    void paintOverChildren (Graphics& g) override
    {
        if (showBounds && layout) layout->paintBounds (g);
    }

    void resized() override
    {
        if (mainComponent) mainComponent->setBounds (getLocalBounds());
        if (layout)        layout->updateGeometry();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreviewComponent)
    
    bool showBounds;
    ScopedPointer<Layout>    layout;
    ScopedPointer<Component> mainComponent;

    OwnedArray<Component> testComponents;
};


#endif  // PREVIEWCOMPONENT_H_INCLUDED
