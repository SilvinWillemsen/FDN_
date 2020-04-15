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
    delayLines->addItem ("Random (pre-defined)", randomPredef);
    delayLines->addItem ("Random", randomDlen);
    delayLines->addItem ("Primes", primes);
    delayLines->addItem ("Uniform", uniform);
    
    delayLines->setSelectedId (Global::initDelayLineSetting);
    addAndMakeVisible (delayLines.get());
    delayLines->addListener (this);
    
    delayLinesLabel = std::make_unique<Label> ("delayLinesLabel", "Delay Lines Type");
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

    if (delayLines->getSelectedId() != randomDlen)
    {
        rangeSliderMin->setEnabled (false);
        rangeSliderMax->setEnabled (false);
        applyRangeBtn->setEnabled (false);
    } else {
        checkSlidersAgainstAppliedVals();
    }
    
    //// Exit Button ////
    exitBtn = std::make_unique<TextButton> ("Exit");
    exitBtn->addListener (this);
    addAndMakeVisible (exitBtn.get());
    
    setSize (400, 300);
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
    Rectangle<int> totArea = getLocalBounds();
    Rectangle<int> lowerButtonArea = totArea.removeFromBottom (40 + 2.0 * Global::margin);
    Rectangle<int> labelArea = totArea.removeFromLeft (0.33 * getWidth());

    //// Labels ////
    labelArea.reduce (Global::margin, Global::margin);
    fdnOrderLabel->setBounds (labelArea.removeFromTop (40));
    labelArea.removeFromTop(Global::margin);
    scatMatsLabel->setBounds (labelArea.removeFromTop (40));
    labelArea.removeFromTop(Global::margin);
    delayLinesLabel->setBounds (labelArea.removeFromTop (40));
    labelArea.removeFromTop(Global::margin);
    rangeLabel->setBounds (labelArea.removeFromTop (20));
    labelArea.removeFromTop(Global::margin);
    applyRangeBtn->setBounds (labelArea.removeFromTop (20));

    
    //// Controls ////
    totArea.reduce (Global::margin, Global::margin);
    fdnOrder->setBounds (totArea.removeFromTop (40));
    totArea.removeFromTop(Global::margin);
    scatMats->setBounds (totArea.removeFromTop (40));
    totArea.removeFromTop (Global::margin);
    delayLines->setBounds (totArea.removeFromTop (40));
    totArea.removeFromTop (Global::margin);
    rangeSliderMin->setBounds (totArea.removeFromTop (20));
    totArea.removeFromTop (Global::margin);
    rangeSliderMax->setBounds (totArea.removeFromTop (20));
    
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
        lastMaxRange = rangeSliderMin->getValue();
        applyRangeBtn->setEnabled (false);
    }
    else if (button == exitBtn.get())
    {
        DialogWindow* dw = this->findParentComponentOfClass<DialogWindow>();
        dw->exitModalState (0);
    }
}

void AdvancedSettingsWindow::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == delayLines.get())
    {
        if (delayLines->getSelectedId() == randomDlen)
        {
            rangeSliderMin->setEnabled (true);
            rangeSliderMax->setEnabled (true);
            checkSlidersAgainstAppliedVals();
        } else {
            rangeSliderMin->setEnabled (false);
            rangeSliderMax->setEnabled (false);
        }
    }
    
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
    applyRangeBtn->setEnabled (false);
    else
    applyRangeBtn->setEnabled (true);
}
