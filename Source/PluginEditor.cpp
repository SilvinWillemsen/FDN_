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
        sliders[i]->setTextBoxIsEditable (false);
        
        labels.add (new Label (name, name));
        addAndMakeVisible (labels[i]);
        labels[i]->attachToComponent (sliders[i], false);
        
    }
    if (Global::horSliders)
        sliders.add (new Slider ("dryGain"));
    else
    {
        sliders.add (new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
        sliders[sliders.size() - 1]->setName ("dryGain");
    }
    sliders[sliders.size() - 1]->setRange (0.0, 1.0, 0.001);
    sliders[sliders.size() - 1]->setValue (Global::dryWetInit);
    
    sliders[sliders.size() - 1]->addListener (this);
    addAndMakeVisible (sliders[sliders.size() - 1]);
    
    labels.add (new Label ("dryGain", "Dry/Wet"));
    addAndMakeVisible (labels[labels.size() - 1]);
    labels[labels.size() - 1]->setJustificationType(Justification::centred);
    labels[labels.size() - 1]->attachToComponent (sliders[sliders.size() - 1], false);
    
    if (Global::horSliders)
        sliders.add (new Slider ("inputGain"));
    else
    {
        sliders.add (new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
        sliders[sliders.size() - 1]->setName ("inputGain");
    }
    
    sliders[sliders.size() - 1]->setRange (0.0, 1.0, 0.001);
    sliders[sliders.size() - 1]->setValue (Global::initInputGain);
    
    sliders[sliders.size() - 1]->addListener (this);
    addAndMakeVisible (sliders[sliders.size() - 1]);
    
    labels.add (new Label ("inputGain", "Input Gain"));
    addAndMakeVisible (labels[labels.size() - 1]);
    labels[labels.size() - 1]->setJustificationType(Justification::centred);
    labels[labels.size() - 1]->attachToComponent (sliders[sliders.size() - 1], false);
    
    calculateBtn = std::make_unique<TextButton> ("Test");
    calculateBtn->addListener (this);
    addAndMakeVisible (calculateBtn.get());
    
    smoothVals = std::make_unique<TextButton> ("Smooth");
    smoothVals->addListener (this);
    addAndMakeVisible (smoothVals.get());
    defaultButtonColour = smoothVals->getLookAndFeel().findColour (TextButton::buttonColourId);
    
    allSliders = std::make_unique<TextButton> ("All Sliders");
    allSliders->addListener (this);
    addAndMakeVisible (allSliders.get());

    std::cout << "Max dLen = " << processor.getFDN()->getMaxDLen() << std::endl;
    std::cout << "Avg dLen = " << processor.getFDN()->getAvgDLen() << std::endl;
    std::cout << "Min dLen = " << processor.getFDN()->getMinDLen() << std::endl;

    minDLenIdx = processor.getFDN()->getMinDLenIdx();
    maxDLenIdx = processor.getFDN()->getMaxDLenIdx();
    response = std::make_unique<Response> (processor.getFDN()->getMinDLen(), processor.getFDN()->getMaxDLen(), processor.getSampleRate());
    addAndMakeVisible (response.get());

    //// Presets ////
    presets = std::make_unique<ComboBox> ("Presets");
    presets->addItem ("-- no preset --", noPreset);
    presets->setItemEnabled (1, false);
    presets->addItem ("Small Room", smallRoom);
    presets->addItem ("Medium Room", mediumRoom);
    presets->addItem ("Large Room", largeRoom);
    presets->addItem ("Concert Hall", concertHall);
    presets->addItem ("Church", church);
    presets->setSelectedId (3);
    addAndMakeVisible (presets.get());
    presets->addListener (this);

    presetsLabel = std::make_unique<Label> ("presets", "Presets:");
    Font font (18.0);
    presetsLabel->setFont (font);
    addAndMakeVisible (presetsLabel.get());
    presetsLabel->setJustificationType(Justification::left);
    presetsLabel->setColour (Label::textColourId, Colours::white);
    
    //// FDN order ////
    fdnOrder = std::make_unique<ComboBox> ("fdnOrder");
    fdnOrder->addItem ("8", 1);
    fdnOrder->addItem ("16", 2);
    fdnOrder->addItem ("32", 3);
    addAndMakeVisible(fdnOrder.get());
    
    switch (Global::initFDNorder)
    {
        case 8:
            fdnOrder->setSelectedId (1);
            break;
        case 16:
            fdnOrder->setSelectedId (2);
            break;
        case 32:
            fdnOrder->setSelectedId (3);
            break;
        default:
            break;
    }
    addAndMakeVisible (fdnOrder.get());
    fdnOrder->addListener (this);

    
    //// Scattering Matrices ////
    scatMats = std::make_unique<ComboBox> ("scatMats");
    scatMats->addItem ("Hadamard", hadamard);
    scatMats->addItem ("Householder", householder);
    scatMats->setSelectedId (Global::initMatType);
//    if (Global::initFDNorder != 16)
//    {
//        scatMats->setItemEnabled (householder, false);
//        scatMats->setSelectedId (hadamard);
//    }
    scatMats->addListener (this);
    addAndMakeVisible(scatMats.get());
    
    startTimerHz (1.0 / Global::updatePerSecondRatio);
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
    
    if (paintResponse)
    {
        response->setDataToZero();
        if (response->isShowingIR())
        {
            if (response->getIRComb() != nullptr)
            {
//                double maxVal = 0;
//                for (int i = 0; i < Global::numOctaveBands; ++i)
//                    if (sliders[i]->getValue() > maxVal)
//                        maxVal = sliders[i]->getValue();
                double avgVal = 0;
                for (int i = 0; i < Global::numOctaveBands; ++i)
                    avgVal += sliders[i]->getValue();
                avgVal /= Global::numOctaveBands;
                
                for (int i = 0; i < Global::numOctaveBands + 1; ++i)
                    response->getIRComb()->setFilter (i, processor.getFDN()->getCoefficients(maxDLenIdx, i));
                response->calculateIR();
                response->setIRseconds (Global::limit (round (avgVal), 1.0, 15.0) + 0.5);
            }
        }
        //// there is no 'else' here so that instablility can be calculated {
        for (int i = 0; i < Global::numOctaveBands + 1; ++i)
        {
            std::vector<double> coeffs = processor.getFDN()->getCoefficients (minDLenIdx, i);
            response->calculateResponse (coeffs);
            
        }
        response->linearGainToDB();
        ////}
        paintResponse = false;
    }
    
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
        int margin = 12;
        Rectangle<int> sidePanel = totArea.removeFromRight (120);
        sidePanel.reduce (margin, margin);

        //// Preset Area ////
        Rectangle<int> presetArea = totArea.removeFromTop (40);
        presetArea.removeFromLeft (margin * 0.5);
        presetArea.removeFromTop (margin);
        presetsLabel->setBounds (presetArea.removeFromLeft (70));
//        presetArea.removeFromLeft (20);
        presets->setBounds (presetArea);
        
        //// Side panel ////
        
        // Top Half //
        sidePanel.removeFromTop (margin);
        int knobHeight = sidePanel.getHeight() * 0.2 - margin;
        sliders[sliders.size() - 1]->setBounds (sidePanel.removeFromTop (knobHeight));
        sidePanel.removeFromTop (margin * 3.0);
        sliders[sliders.size() - 2]->setBounds (sidePanel.removeFromTop (knobHeight));
        sidePanel.removeFromTop (margin * 3.0);
        
        // Bottom Half //
        calculateBtn->setBounds (sidePanel.removeFromBottom (40));
        sidePanel.removeFromBottom (margin);
        smoothVals->setBounds (sidePanel.removeFromBottom (40));
        sidePanel.removeFromBottom (margin);
        allSliders->setBounds (sidePanel.removeFromBottom (40));
        sidePanel.removeFromBottom (margin);
        scatMats->setBounds (sidePanel.removeFromBottom (40));
        sidePanel.removeFromBottom (margin);
        fdnOrder->setBounds (sidePanel.removeFromBottom (40));
        sidePanel.removeFromBottom (margin);

        //// Response area ////
        Rectangle<int> responseArea = totArea.removeFromTop (getHeight() * 0.5);
        responseArea.reduce(0, margin);
        responseArea.removeFromLeft (margin);
        response->setBounds (responseArea);
        
        //// Sliders ////
        totArea.removeFromTop (20);
        totArea.removeFromBottom (margin);

        int sliderMargin = 10;
        totArea.removeFromLeft (margin);
        int sliderWidth = ceil ((totArea.getWidth() - (9.0 * sliderMargin)) / 10.0);
        
        for (int i = 0; i < sliders.size() - 2; ++i)
        {
            sliders[i]->setBounds (totArea.removeFromLeft (sliderWidth));
            totArea.removeFromLeft (sliderMargin);
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
        processor.setDryGain (1.0-slider->getValue());
        return;
    }
    
    if (slider == sliders[sliders.size() - 1])
    {
        processor.setInputGain (slider->getValue());
        return;
    }
    
    if (curSlider == nullptr)
        return;
    
    if (curSliderIdx != -1)
    {
        presets->setSelectedId (1);
        processor.getFDN()->setRT (curSliderIdx, curSlider->getValue());
    } else {
        return;
    }
        
    // Affect all sliders smoothly
    if (smoothValsBool)
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
    // Set all sliders to the current slider value
    else if (allSlidersBool)
    {
        for (int j = 0; j < sliders.size() - 2; ++j)
        {
            if (curSliderIdx != j)
            {
                sliders[j]->setValue (curSlider->getValue());
                processor.getFDN()->setRT (j, sliders[j]->getValue());
            }
        }
    }
}

void Fdn_AudioProcessorEditor::buttonClicked (Button* button)
{
    if (button == calculateBtn.get())
    {
        processor.setZeroCoeffsFlag();
    }
    else if (button == smoothVals.get())
    {
        smoothValsBool = !smoothValsBool;
        if (!smoothValsBool)
            smoothVals->setColour (TextButton::buttonColourId, defaultButtonColour);
        else
        {
            smoothVals->setColour (TextButton::buttonColourId, Colours::darkgreen);
            // if allsliders button is on, disable
            if (allSlidersBool)
                buttonClicked (allSliders.get());
        }
        
    }
    else if (button == allSliders.get())
    {
        allSlidersBool = !allSlidersBool;
        if (!allSlidersBool)
            allSliders->setColour (TextButton::buttonColourId, defaultButtonColour);
        else
        {
            allSliders->setColour (TextButton::buttonColourId, Colours::darkgreen);
            // if smooth values button is on, disable
            if (smoothValsBool)
                buttonClicked (smoothVals.get());
        }
    }
}

void Fdn_AudioProcessorEditor::timerCallback()
{
    
    if (!changingFDNorder)
        paintResponse = true;
    else if (processor.getFDN()->isInitialised())
    {
        minDLenIdx = processor.getFDN()->getMinDLenIdx();
        maxDLenIdx = processor.getFDN()->getMaxDLenIdx();
        response->setDLens (processor.getFDN()->getMinDLen(), processor.getFDN()->getMaxDLen());
        changingFDNorder = false;
    }
    repaint();
}

void Fdn_AudioProcessorEditor::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == presets.get() && presets->getSelectedId() != 1)
    {
        curSliderIdx = -1;
        for (int i = 0; i < Global::numOctaveBands; ++i)
        {
            sliders[i]->setValue (Global::presetValues[presets->getSelectedId() - 2][i]);
            processor.getFDN()->setRT (i, sliders[i]->getValue());
        }
    }
    else if (comboBoxThatHasChanged == fdnOrder.get())
    {
        int orderToSwitchTo;
        switch (fdnOrder->getSelectedId())
        {
            case 1:
                orderToSwitchTo = 8;
                break;
            case 2:
                orderToSwitchTo = 16;
                break;
            case 3:
                orderToSwitchTo = 32;
                break;
        }
        
        if (processor.getFDN()->getFDNorder() == orderToSwitchTo)
        {
            return;
        }
        
        changingFDNorder = true;
//        if (orderToSwitchTo != 16)
//        {
//            scatMats->setSelectedId (hadamard);
//            scatMats->setItemEnabled (householder, false);
//        } else {
//            scatMats->setItemEnabled (householder, true);
//        }
        
        processor.changeFDNorder (orderToSwitchTo, static_cast<MatrixType> (scatMats->getSelectedId()));

        std::cout << "FDN order changed" << std::endl;
    }
    else if (comboBoxThatHasChanged == scatMats.get())
    {
        processor.changeMatType (static_cast<MatrixType> (scatMats->getSelectedId()));
    }
}
