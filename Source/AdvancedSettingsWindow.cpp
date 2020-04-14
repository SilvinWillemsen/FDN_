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
    delayLines->addItem ("Random (pre-defined)", 1);
    delayLines->addItem ("Random", 2);
    delayLines->addItem ("Primes", 3);
    delayLines->addItem ("Uniform", 4);
    
    delayLines->setSelectedId (Global::initDelayLineSetting);
    addAndMakeVisible (delayLines.get());
    
    delayLinesLabel = std::make_unique<Label> ("delayLinesLabel", "Delay Lines Type");
    addAndMakeVisible (delayLinesLabel.get());
    delayLinesLabel->setJustificationType(Justification::left);
    delayLinesLabel->setColour (Label::textColourId, Colours::white);
    
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
    
    //// Controls ////
    totArea.reduce(Global::margin, Global::margin);
    fdnOrder->setBounds (totArea.removeFromTop (40));
    totArea.removeFromTop(Global::margin);
    scatMats->setBounds (totArea.removeFromTop (40));
    totArea.removeFromTop(Global::margin);
    delayLines->setBounds (totArea.removeFromTop (40));
    
    //// Buttons ////
    lowerButtonArea.reduce(Global::margin, Global::margin);
    impulseBtn->setBounds (lowerButtonArea.removeFromRight (100));
    lowerButtonArea.removeFromRight (Global::margin);
}

void AdvancedSettingsWindow::buttonClicked (Button* button)
{
    if (button == impulseBtn.get())
    {
        impulseBtnPtr->triggerClick();
    }
}

void AdvancedSettingsWindow::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    
}
