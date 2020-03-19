/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Global.h"
//==============================================================================
Fdn_AudioProcessorEditor::Fdn_AudioProcessorEditor (Fdn_AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    sliderValuesAtStartDrag.resize (Global::numOctaveBands, Global::RT);

    for (int i = 0; i < Global::numOctaveBands; ++i)
    {
        String name = "RT" + String(Global::fc1[i]);
        if (Global::horSliders)
            sliders.add (new Slider (name));
        else
        {
            sliders.add (new Slider (Slider::LinearBarVertical, Slider::TextBoxBelow));
            sliders[i]->setName (name);
        }
        sliders[i]->setRange (Global::RTmin, Global::RTmax, 0.01f);
//        sliders[i]->setSkewFactorFromMidPoint (1);
        sliders[i]->setValue (Global::RT);
        addAndMakeVisible (sliders[i]);
        sliders[i]->addListener (this);
        
        labels.add (new Label (name, name));
        addAndMakeVisible (labels[i]);
        labels[i]->attachToComponent (sliders[i], false);
        
    }
    if (Global::horSliders)
        sliders.add (new Slider ("dryGain"));
    else
    {
        sliders.add (new Slider (Slider::LinearBarVertical, Slider::TextBoxBelow));
        sliders[sliders.size() - 1]->setName ("dryGain");
    }
    sliders[sliders.size() - 1]->setRange (0.0, 1.0, 0.001);
    sliders[sliders.size() - 1]->setValue (Global::dryGainInit);
    
    sliders[sliders.size() - 1]->addListener (this);
    addAndMakeVisible (sliders[sliders.size() - 1]);
    
    labels.add (new Label ("dryGain", "Dry Gain"));
    addAndMakeVisible (labels[labels.size() - 1]);
    labels[labels.size() - 1]->attachToComponent (sliders[sliders.size() - 1], false);
    
    if (Global::horSliders)
        sliders.add (new Slider ("inputGain"));
    else
    {
        sliders.add (new Slider (Slider::LinearBarVertical, Slider::TextBoxBelow));
        sliders[sliders.size() - 1]->setName ("inputGain");
    }
    
    sliders[sliders.size() - 1]->setRange (0.0, 1.0, 0.001);
    sliders[sliders.size() - 1]->setValue (Global::initInputGain);
    
    sliders[sliders.size() - 1]->addListener (this);
    addAndMakeVisible (sliders[sliders.size() - 1]);
    
    labels.add (new Label ("inputGain", "Input Gain"));
    addAndMakeVisible (labels[labels.size() - 1]);
    labels[labels.size() - 1]->attachToComponent (sliders[sliders.size() - 1], false);
    
    calculateBtn = std::make_unique<TextButton> ("Calculate");
    calculateBtn->addListener (this);
    addAndMakeVisible (calculateBtn.get());
    
    response = std::make_unique<Response> (processor.getSampleRate());
    addAndMakeVisible (response.get());
//    double R = 0.99;
//    response->setCoefficients ({1, 0, 0, 1, -2.0 * R * cos(double_Pi * 1000.0 / 22050.0), R * R});
//    response->setCoefficients ({0.5, 0.5, 0, 1, 0, 0});
    startTimerHz (15);
    setSize (800, 700);
}

Fdn_AudioProcessorEditor::~Fdn_AudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void Fdn_AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    response->setDataToZero();
	
    for (int i = 0; i < Global::numOctaveBands + 1; ++i)	
    {
        std::vector<double> coeffs = processor.getFDN()->getCoefficients (0, i);
	
       //std::vector<double> coeffs = {0.8775, 0.7515, 0, 1, 0.6292, 0};
	   //std::vector<double> coeffs = { 1.0178, 0.7291, 0.1023, 1,0.7547, 0.1595 };
        response->calculateResponse (coeffs);
		
    }
    response->linearGainToDB();
    
}

void Fdn_AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle<int> totArea = getLocalBounds();
    
    if (Global::horSliders)
    {
        int sliderHeight = 40;
        response->setBounds (totArea.removeFromRight (getWidth() * 0.5));
        calculateBtn->setBounds (totArea.removeFromBottom (sliderHeight));
        totArea.removeFromLeft (100);
        sliders[sliders.size() - 1]->setBounds (totArea.removeFromTop (sliderHeight));
        sliders[sliders.size() - 2]->setBounds (totArea.removeFromTop (sliderHeight));
        
        for (int i = 0; i < sliders.size() - 2; ++i)
        {
            sliders[i]->setBounds (totArea.removeFromTop (sliderHeight));
        }
    } else {
        int margin = 5;
        int sliderWidth = floor ((getWidth() - (14 * margin)) / 13.0);
        Rectangle<int> responseArea = totArea.removeFromTop (getHeight() * 0.5);
        responseArea.reduce(10, 10);
        response->setBounds (responseArea);
        totArea.removeFromTop (15);
        calculateBtn->setBounds (totArea.removeFromRight (sliderWidth));
        totArea.removeFromBottom (20);
        sliders[sliders.size() - 1]->setBounds (totArea.removeFromLeft (sliderWidth));
        sliders[sliders.size() - 2]->setBounds (totArea.removeFromLeft(sliderWidth));
        
        for (int i = 0; i < sliders.size() - 2; ++i)
        {
            totArea.removeFromLeft (margin);
            sliders[i]->setBounds (totArea.removeFromLeft (sliderWidth));
        }
    }
    
}

void Fdn_AudioProcessorEditor::sliderDragStarted (Slider* slider)
{
    for (int i = 0; i < sliders.size() - 2; ++i)
    {
        sliderValuesAtStartDrag[i] = sliders[i]->getValue();
        if (slider == sliders[i])
        {
            curSlider = sliders[i];
            curSliderIdx = i;
        }
    }
}

void Fdn_AudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == sliders[sliders.size() - 2])
    {
        processor.setDryGain (slider->getValue());
        return;
    }
    
    if (slider == sliders[sliders.size() - 1])
    {
        processor.setInputGain (slider->getValue());
        return;
    }
    
    if (curSlider == nullptr)
        return;
    
    processor.getFDN()->setRT (curSliderIdx, curSlider->getValue());
    
    if (Global::sliderDependency)
    {
        for (int j = 0; j < sliders.size() - 2; ++j)
        {
            if (curSliderIdx != j)
            {
                sliders[j]->setValue (sliderValuesAtStartDrag[j] + (curSlider->getValue() - sliderValuesAtStartDrag[curSliderIdx]) * pow(sliderCoeff, abs(j-curSliderIdx)));
                processor.getFDN()->setRT (j, sliders[j]->getValue());
            }
        }
    }
}

void Fdn_AudioProcessorEditor::buttonClicked (Button* button)
{
    if (button == calculateBtn.get())
    {
        processor.setRecalculateFlag();
    }
}

void Fdn_AudioProcessorEditor::timerCallback()
{
    repaint();
}
