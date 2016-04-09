/*
  ==============================================================================

    debugComponent.h
    Created: 15 Mar 2016 4:20:15pm
    Author:  Daniel Walz

  ==============================================================================
*/

#ifndef DEBUGCOMPONENT_H_INCLUDED
#define DEBUGCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class debugComponent    : public Component
{
public:
    debugComponent() : showBounds (true)
    {
        WildcardFileFilter wildcardFilter ("*", String::empty, "Layout file");
        FileBrowserComponent browser (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                      File::nonexistent,
                                      &wildcardFilter,
                                      nullptr);
        FileChooserDialogBox dialogBox ("Open layout file",
                                        "Please choose a layout file...",
                                        browser,
                                        false,
                                        Colours::lightgrey);
        if (dialogBox.show())
        {
            loadLayout (browser.getSelectedFile (0));
        }
        
        if (getWidth() * getHeight() < 1) {
            // if a size constraint is set in the xml, the component has already a valid size
            setSize (100, 100);
        }
    }

    ~debugComponent()
    {
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
            newComponent->setJustificationType (Justification::centred);
            addAndMakeVisible (newComponent);
            testComponents.add (newComponent);
        }
        else if (tree.hasProperty ("componentID")) {
            String componentID = tree.getProperty ("componentID");
            Label* newComponent = new Label ();
            newComponent->setComponentID (componentID);
            newComponent->setText ("ID:" + componentID, dontSendNotification);
            newComponent->setJustificationType (Justification::centred);
            newComponent->setColour (Label::backgroundColourId, Colour::fromRGB (rand() % 128 + 127, rand() % 128 + 127, rand() % 128 + 127));
            addAndMakeVisible (newComponent);
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
        
        layout = new Layout (layoutCode, this);
    }
    
    // set showBounds to true to paint the bounds of individual layouts
    void paintOverChildren (Graphics& g) override
    {
        if (showBounds && layout) layout->paintBounds (g);
    }

    void resized() override
    {
        if (layout) layout->updateGeometry();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (debugComponent)
    
    bool showBounds;
    ScopedPointer<Layout>                       layout;

    OwnedArray<Component> testComponents;
};


#endif  // DEBUGCOMPONENT_H_INCLUDED
