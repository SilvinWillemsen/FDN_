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
                                    public ComboBox::Listener,
                                    public Slider::Listener

{
public:
    AdvancedSettingsWindow (std::shared_ptr<TextButton> impulseBtnPtr);
    ~AdvancedSettingsWindow();

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked (Button* button) override;
    
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;

    void sliderValueChanged (Slider* slider) override;
    void sliderDragStarted (Slider* slider) override;
    
    std::shared_ptr<ComboBox> getFDNOrderBox() { return fdnOrder; };
    std::shared_ptr<ComboBox> getScatMatsBox() { return scatMats; };
    std::shared_ptr<ComboBox> getDelayLinesBox() { return delayLines; };
    std::shared_ptr<TextButton> getPreDefBtn() { return preDefBtn; };
    std::shared_ptr<TextButton> getApplyRangeBtn() { return applyRangeBtn; };
//    std::shared_ptr<TextButton> getExitBtn() { return exitBtn; };

    int getRangeSliderMin() { return rangeSliderMin->getValue(); };
    int getRangeSliderMax() { return rangeSliderMax->getValue(); };
    
    void setPreDefEnabled (bool enabled);

    void checkSlidersAgainstAppliedVals();
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
    
    std::unique_ptr<Label> rangeLabel;
    std::shared_ptr<Slider> rangeSliderMin;
    std::shared_ptr<Slider> rangeSliderMax;
    std::unique_ptr<Label> minLabel;
    std::unique_ptr<Label> maxLabel;

    std::shared_ptr<TextButton> preDefBtn;
    bool preDefOn = Global::initPreDefOn;
    
    std::shared_ptr<TextButton> applyRangeBtn;

    std::shared_ptr<TextButton> exitBtn;
    Slider* curSlider = nullptr;

    int lastMinRange = Global::minDelayLength;
    int lastMaxRange = Global::maxDelayLength;

    Colour defaultButtonColour;

};
