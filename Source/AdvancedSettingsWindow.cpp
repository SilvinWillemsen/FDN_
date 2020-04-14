/*
  ==============================================================================

    AdvancedSettingsWindow.cpp
    Created: 14 Apr 2020 11:32:27am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AdvancedSettingsWindow.h"

//==============================================================================
AdvancedSettingsWindow::AdvancedSettingsWindow()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setSize (400, 200);
}

AdvancedSettingsWindow::~AdvancedSettingsWindow()
{
}

void AdvancedSettingsWindow::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("Advanced Settings Window", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void AdvancedSettingsWindow::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void AdvancedSettingsWindow::buttonClicked (Button* button)
{
    
}
