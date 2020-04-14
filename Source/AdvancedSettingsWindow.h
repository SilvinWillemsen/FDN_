/*
  ==============================================================================

    AdvancedSettingsWindow.h
    Created: 14 Apr 2020 11:32:27am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Global.h"
//==============================================================================
/*
*/
class AdvancedSettingsWindow    : public Component,
                                  public Button::Listener
{
public:
    AdvancedSettingsWindow();
    ~AdvancedSettingsWindow();

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked (Button* button);
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdvancedSettingsWindow)
    
};
