/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AdvancedSettingsWindow.h"
#include "Response.h"

//==============================================================================
/**
*/
class Fdn_AudioProcessorEditor  : public AudioProcessorEditor,
                                  public Slider::Listener,
                                  public Button::Listener,
                                  public Timer,
                                  public ComboBox::Listener,
                                  public ChangeListener
{
public:
    Fdn_AudioProcessorEditor (Fdn_AudioProcessor&);
    ~Fdn_AudioProcessorEditor();

    void initialiseAdvancedSettings();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void calculateImpulseResponse();
    void calculateEQ();
    
    void sliderValueChanged (Slider* slider) override;
    void sliderDragStarted (Slider* slider) override;
    
    void buttonClicked (Button* button) override;
    
    void timerCallback() override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    void openAdvancedSettings();
    
private:
    
    Fdn_AudioProcessor& processor;
    std::unique_ptr<Response> response;
    
    std::unique_ptr<AdvancedSettingsWindow> advancedSettingsWindow;
    
    OwnedArray<Slider> sliders;
    OwnedArray<Label> labels;
    std::shared_ptr<TextButton> impulseBtn;
    std::unique_ptr<TextButton> smoothVals;
    std::unique_ptr<TextButton> allSliders;
    
    std::unique_ptr<ComboBox> presets;
    std::unique_ptr<Label> presetsLabel;
    
    //// Advanced Settings ////
    std::unique_ptr<TextButton> advancedSettings;

    std::shared_ptr<ComboBox> fdnOrder;
    std::unique_ptr<Label> fdnOrderLabel;
    
    std::shared_ptr<ComboBox> scatMats;
    std::unique_ptr<Label> scatMatsLabel;
    
    std::shared_ptr<ComboBox> delayLines;
    std::shared_ptr<TextButton> applyRangeBtn;
    
//    std::shared_ptr<TextButton> exitBtn;


    bool changingFDNorder = false;
    
    Colour defaultButtonColour;
    bool smoothValsBool = false;
    bool allSlidersBool = false;

    std::vector<double> sliderValuesAtStartDrag;
    float sliderCoeff = Global::sliderCoeff;
    Slider* curSlider = nullptr;
    int curSliderIdx = -1;
    
    bool paintResponse = false;
    int minDLenIdx, maxDLenIdx;
    
    std::unique_ptr<TextButton> fixCoeffs;
    bool coeffsFixed = false;
    
    //// CPU usage ////
    std::unique_ptr<Label> cpuUsageAudio;
    std::unique_ptr<Label> cpuUsageGraphics;
    
    long time;
    double cpuUsageGraphicsValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Fdn_AudioProcessorEditor)
};
