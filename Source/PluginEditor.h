/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Response.h"

//==============================================================================
/**
*/
class Fdn_AudioProcessorEditor  : public AudioProcessorEditor,
                                  public Slider::Listener,
                                  public Button::Listener,
                                  public Timer
{
public:
    Fdn_AudioProcessorEditor (Fdn_AudioProcessor&);
    ~Fdn_AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider* slider) override;
    void sliderDragStarted (Slider* slider) override;
    
    void buttonClicked (Button* button) override;
    
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Fdn_AudioProcessor& processor;
    std::unique_ptr<Response> response;
    
    OwnedArray<Slider> sliders;
    OwnedArray<Label> labels;
    std::unique_ptr<TextButton> calculateBtn;
    std::unique_ptr<TextButton> smoothVals;
    std::unique_ptr<TextButton> allSliders;

    Colour defaultButtonColour;
    bool smoothValsBool = false;
    bool allSlidersBool = false;

    std::vector<double> sliderValuesAtStartDrag;
    float sliderCoeff = Global::sliderCoeff;
    Slider* curSlider = nullptr;
    int curSliderIdx;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Fdn_AudioProcessorEditor)
};
