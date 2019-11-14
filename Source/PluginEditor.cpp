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
        sliders.add (new Slider (name));
        sliders[i]->setRange (Global::RTmin, Global::RTmax, 0.01f);
//        sliders[i]->setSkewFactorFromMidPoint (1);
        sliders[i]->setValue (Global::RT);
        addAndMakeVisible (sliders[i]);
        sliders[i]->addListener (this);
        
        labels.add (new Label (name, name));
        addAndMakeVisible (labels[i]);
        labels[i]->attachToComponent (sliders[i], true);
        
    }
    sliders.add (new Slider ("dryGain"));
    sliders[sliders.size() - 1]->setRange (0.0f, 1.0f, 0.001f);
    sliders[sliders.size() - 1]->setValue (0.55f);
    addAndMakeVisible (sliders[sliders.size() - 1]);
    
    labels.add (new Label ("dryGain", "dryGain"));
    addAndMakeVisible (labels[labels.size() - 1]);
    labels[labels.size() - 1]->attachToComponent (sliders[sliders.size() - 1], true);
    
    int height = (sliders.size() + 1) * Global::sliderHeight;
    if (height > 700)
        height = 700;
    
    calculateBtn = std::make_unique<TextButton> ("Calculate");
    calculateBtn->addListener (this);
    addAndMakeVisible (calculateBtn.get());
    setSize (400, height);
}

Fdn_AudioProcessorEditor::~Fdn_AudioProcessorEditor()
{
}

//==============================================================================
void Fdn_AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

//    g.setColour (Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void Fdn_AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle<int> totArea = getLocalBounds();
    calculateBtn->setBounds(totArea.removeFromBottom (Global::sliderHeight));
    totArea.removeFromLeft (120);
    sliders[sliders.size() - 1]->setBounds (totArea.removeFromTop (Global::sliderHeight));
    for (int i = 0; i < sliders.size() - 1; ++i)
    {
        sliders[i]->setBounds (totArea.removeFromTop (Global::sliderHeight));
    }
    
}

void Fdn_AudioProcessorEditor::sliderDragStarted (Slider* slider)
{
    for (int i = 0; i < sliders.size() - 1; ++i)
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
    if (curSlider == nullptr)
        return;
    
    processor.getFDN()->setRT (curSliderIdx, curSlider->getValue());
    
    for (int j = 0; j < sliders.size() - 1; ++j)
    {
//        auto test = sliderValuesAtStartDrag[j] - pow(sliderCoeff, abs(j-curSliderIdx));
//        std::cout << j << ": " << test << std::endl;
        if (curSliderIdx != j)
        {
            sliders[j]->setValue (sliderValuesAtStartDrag[j] + (curSlider->getValue() - sliderValuesAtStartDrag[curSliderIdx]) * pow(sliderCoeff, abs(j-curSliderIdx)));
            processor.getFDN()->setRT (j, sliders[j]->getValue());
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
