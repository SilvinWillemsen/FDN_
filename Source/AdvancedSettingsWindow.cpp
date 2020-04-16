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
AdvancedSettingsWindow::AdvancedSettingsWindow (std::shared_ptr<TextButton> impulseBtnPtr) : impulseBtnPtr (impulseBtnPtr)
{
    //// Impulse Button ////
    impulseBtn = std::make_unique<TextButton> ("Impulse");
    impulseBtn->addListener (this);
    addAndMakeVisible (impulseBtn.get());
    
    //// FDN order ////
    fdnOrder = std::make_unique<ComboBox> ("fdnOrder");
    fdnOrder->addItem("2", 1);
    fdnOrder->addItem("4", 2);
    fdnOrder->addItem ("8", 3);
    fdnOrder->addItem ("16", 4);
    fdnOrder->addItem ("32", 5);
    fdnOrder->addItem ("64", 6);
    addAndMakeVisible(fdnOrder.get());
    
    switch (Global::initFDNorder)
    {
        case 2:
            fdnOrder->setSelectedId (1);
            break;
        case 4:
            fdnOrder->setSelectedId (2);
            break;
        case 8:
            fdnOrder->setSelectedId (3);
            break;
        case 16:
            fdnOrder->setSelectedId (4);
            break;
        case 32:
            fdnOrder->setSelectedId (5);
            break;
        case 64:
            fdnOrder->setSelectedId (6);
            break;
        default:
            break;
    }
    addAndMakeVisible (fdnOrder.get());
    
    fdnOrderLabel = std::make_unique<Label> ("fdnOrderLabel", "FDN Order");
    addAndMakeVisible (fdnOrderLabel.get());
    fdnOrderLabel->setJustificationType(Justification::left);
    fdnOrderLabel->setColour (Label::textColourId, Colours::white);
    
    //// Scattering Matrices ////
    scatMats = std::make_unique<ComboBox> ("scatMats");
    scatMats->addItem ("Hadamard", hadamard);
    scatMats->addItem ("Householder", householder);
    scatMats->addItem ("Identity", identity);
    scatMats->addItem ("Random", randomMat);
    
    scatMats->setSelectedId (Global::initMatType);
    addAndMakeVisible (scatMats.get());
    
    scatMatsLabel = std::make_unique<Label> ("scatMatsLabel", "Scattering Matrix");
    addAndMakeVisible (scatMatsLabel.get());
    scatMatsLabel->setJustificationType(Justification::left);
    scatMatsLabel->setColour (Label::textColourId, Colours::white);
    
    //// Delay lines ////
    delayLines = std::make_unique<ComboBox> ("delayLines");
    delayLines->addItem ("Random", gaussianDlen);
    delayLines->addItem ("Primes", primes);
    delayLines->addItem ("Uniform", uniform);
    
    delayLines->setSelectedId (Global::initDelayLineSetting - (preDefOn ? 3 : 0));
    addAndMakeVisible (delayLines.get());
    delayLines->addListener (this);
    
    delayLinesLabel = std::make_unique<Label> ("delayLinesLabel", "Delay-line Settings");
    Font font (18.0f);
    delayLinesLabel->setFont (font);
    addAndMakeVisible (delayLinesLabel.get());
    delayLinesLabel->setJustificationType(Justification::left);
    delayLinesLabel->setColour (Label::textColourId, Colours::white);
    
    //// Range ////
    rangeLabel = std::make_unique<Label> ("rangeLabel", "Range delay lines");
    addAndMakeVisible (rangeLabel.get());
    rangeLabel->setJustificationType (Justification::left);
    rangeLabel->setColour (Label::textColourId, Colours::white);
    
    //// Apply range Button ////
    applyRangeBtn = std::make_unique<TextButton> ("Apply range");
    applyRangeBtn->addListener (this);
    addAndMakeVisible (applyRangeBtn.get());
    
    //// Sliders for min and max range random ////
    rangeSliderMin = std::make_shared<Slider>();
    rangeSliderMin->setRange (Global::minRange, Global::maxRange - Global::minRangeDiff, 1);
    rangeSliderMin->setValue (Global::minDelayLength);
    addAndMakeVisible (rangeSliderMin.get());
    rangeSliderMin->addListener (this);

    rangeSliderMax = std::make_shared<Slider>();
    rangeSliderMax->setRange (Global::minRange + Global::minRangeDiff, Global::maxRange, 1);
    rangeSliderMax->setValue (Global::maxDelayLength);
    addAndMakeVisible (rangeSliderMax.get());
    rangeSliderMax->addListener (this);
    
    //// Labels ////
    minLabel = std::make_unique<Label> ("minLabel", "Min:");
    addAndMakeVisible (minLabel.get());
    minLabel->setJustificationType (Justification::left);
    minLabel->setColour (Label::textColourId, Colours::white);
    
    maxLabel = std::make_unique<Label> ("maxLabel", "Max:");
    addAndMakeVisible (maxLabel.get());
    maxLabel->setJustificationType (Justification::left);
    maxLabel->setColour (Label::textColourId, Colours::white);
    
    //// Pre-defined Button ////
    preDefBtn = std::make_shared<TextButton> ("Pre-defined (1500-4500)");
    preDefBtn->addListener (this);
    addAndMakeVisible (preDefBtn.get());
    defaultButtonColour = preDefBtn->getLookAndFeel().findColour (TextButton::buttonColourId);
    
    //// Exit Button ////
    exitBtn = std::make_unique<TextButton> ("Exit");
    exitBtn->addListener (this);
    addAndMakeVisible (exitBtn.get());
    
    setPreDefEnabled (preDefOn);
    
    setSize (400, 400);
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

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}
void AdvancedSettingsWindow::resized()
{
    Rectangle<int> totalArea = getLocalBounds();
    Rectangle<int> lowerButtonArea = totalArea.removeFromBottom (40 + 2.0 * Global::margin);
    Rectangle<int> topArea = totalArea.removeFromTop(2.0 * (2.0 * Global::margin + 40));
    Rectangle<int> delayLineArea = totalArea;

    //// Labels top area ////
    Rectangle<int> topLabelArea = topArea.removeFromLeft (0.33 * getWidth());
    topLabelArea.reduce (Global::margin, Global::margin);
    fdnOrderLabel->setBounds (topLabelArea.removeFromTop (40));
    topLabelArea.removeFromTop(Global::margin);
    scatMatsLabel->setBounds (topLabelArea.removeFromTop (40));
    
    //// Controls top area ////
    topArea.reduce (Global::margin, Global::margin);
    fdnOrder->setBounds (topArea.removeFromTop (40));
    topArea.removeFromTop(Global::margin);
    scatMats->setBounds (topArea.removeFromTop (40));
    

    //// Delay line area ////
    delayLineArea.removeFromTop (Global::margin);
    Rectangle<int> delayTitleArea = delayLineArea.removeFromTop (40);
    delayTitleArea.reduce (Global::margin, 0);
    delayLinesLabel->setBounds (delayTitleArea.removeFromLeft (0.4 * delayTitleArea.getWidth()));
    delayTitleArea.removeFromLeft(Global::margin);
    delayTitleArea.reduce (0, Global::margin * 0.5);
    preDefBtn->setBounds (delayTitleArea);
    
    Rectangle<int> delayLineLabelArea = delayLineArea.removeFromLeft (0.33 * getWidth());
    delayLineLabelArea.reduce (Global::margin, Global::margin);
    delayLineArea.reduce (Global::margin, Global::margin);
    
    applyRangeBtn->setBounds (delayLineLabelArea.removeFromTop (40));
    delayLineLabelArea.removeFromTop (Global::margin);
    minLabel->setBounds (delayLineLabelArea.removeFromTop (20));
    delayLineLabelArea.removeFromTop (Global::margin);
    maxLabel->setBounds (delayLineLabelArea.removeFromTop (20));

    delayLines->setBounds (delayLineArea.removeFromTop (40));
    delayLineArea.removeFromTop (Global::margin);
    rangeSliderMin->setBounds (delayLineArea.removeFromTop (20));
    delayLineArea.removeFromTop (Global::margin);
    rangeSliderMax->setBounds (delayLineArea.removeFromTop (20));
    
    //// Buttons ////
    lowerButtonArea.reduce (Global::margin, Global::margin);
    impulseBtn->setBounds (lowerButtonArea.removeFromRight (100));
    lowerButtonArea.removeFromRight (Global::margin);
    exitBtn->setBounds (lowerButtonArea.removeFromRight (100));
}

void AdvancedSettingsWindow::buttonClicked (Button* button)
{
    if (button == impulseBtn.get())
    {
        impulseBtnPtr->triggerClick();
    }
    else if (button == applyRangeBtn.get())
    {
        lastMinRange = rangeSliderMin->getValue();
        lastMaxRange = rangeSliderMax->getValue();
        applyRangeBtn->setButtonText("Randomise");
    }
    else if (button == preDefBtn.get())
    {
        preDefOn = !preDefOn;
        setPreDefEnabled (preDefOn);
    }
    else if (button == exitBtn.get())
    {
        DialogWindow* dw = this->findParentComponentOfClass<DialogWindow>();
        dw->exitModalState (0);
    }
    
}

void AdvancedSettingsWindow::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    
}

void AdvancedSettingsWindow::sliderDragStarted (Slider* slider)
{
    curSlider = slider;
}

void AdvancedSettingsWindow::sliderValueChanged (Slider* slider)
{
    if (slider == rangeSliderMin.get() && slider == curSlider)
    {
        if (rangeSliderMin->getValue() + Global::minRangeDiff >= rangeSliderMax->getValue())
            rangeSliderMax->setValue (rangeSliderMin->getValue() + Global::minRangeDiff);
    }
    if (slider == rangeSliderMax.get() && slider == curSlider)
    {
        if (rangeSliderMax->getValue() - Global::minRangeDiff <= rangeSliderMin->getValue())
            rangeSliderMin->setValue (rangeSliderMax->getValue() - Global::minRangeDiff);
    }
    checkSlidersAgainstAppliedVals();
}

void AdvancedSettingsWindow::checkSlidersAgainstAppliedVals()
{
    if (rangeSliderMin->getValue() == lastMinRange && rangeSliderMax->getValue() == lastMaxRange)
        applyRangeBtn->setButtonText ("Randomise");
    else
        applyRangeBtn->setButtonText ("Apply range");
}

void AdvancedSettingsWindow::setPreDefEnabled (bool enabled)
{
    rangeSliderMin->setEnabled (!enabled);
    rangeSliderMax->setEnabled (!enabled);
    applyRangeBtn->setEnabled (!enabled);
    preDefBtn->setColour (TextButton::buttonColourId, preDefOn ? Colours::darkgreen : defaultButtonColour);
}
