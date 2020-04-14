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
class AdvancedSettingsWindow    :   public Component,
                                    public Button::Listener,
                                    public ComboBox::Listener

{
public:
    AdvancedSettingsWindow (std::shared_ptr<TextButton> impulseBtnPtr);
    ~AdvancedSettingsWindow();

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked (Button* button) override;
    
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;

    std::shared_ptr<ComboBox> getFDNOrderBox() { return fdnOrder; };
    std::shared_ptr<ComboBox> getScatMatsBox() { return scatMats; };
    std::shared_ptr<ComboBox> getDelayLinesBox() { return delayLines; };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdvancedSettingsWindow)
    std::shared_ptr<ComboBox> fdnOrder;
    std::unique_ptr<Label> fdnOrderLabel;
    
    std::shared_ptr<ComboBox> scatMats;
    std::unique_ptr<Label> scatMatsLabel;
    
    std::shared_ptr<ComboBox> delayLines;
    std::unique_ptr<Label> delayLinesLabel;
    
    std::unique_ptr<TextButton> impulseBtn;
    std::shared_ptr<TextButton> impulseBtnPtr;

};
