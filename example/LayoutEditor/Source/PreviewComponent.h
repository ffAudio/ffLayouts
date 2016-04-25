/*
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
            mainComponent->addAndMakeVisible (newComponent);
            testComponents.add (newComponent);
        }
        else if (tree.hasProperty ("componentID")) {
            String componentID = tree.getProperty ("componentID");
            Label* newComponent = new Label ();
            newComponent->setComponentID (componentID);
            newComponent->setText ("ID:" + componentID, dontSendNotification);
            newComponent->setJustificationType (Justification::centred);
            newComponent->setColour (Label::backgroundColourId, Colour::fromRGB (rand() % 128 + 127, rand() % 128 + 127, rand() % 128 + 127));
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
