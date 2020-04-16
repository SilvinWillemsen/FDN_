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
    sliderValuesAtStartDrag.resize (Global::numOctaveBands, Global::RT);

    //// EQ Sliders and Labels ////
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
        sliders[i]->setValue (Global::RT);
        addAndMakeVisible (sliders[i]);
        sliders[i]->addListener (this);
        sliders[i]->setTextBoxIsEditable (false);
        
        labels.add (new Label (name, name));
        addAndMakeVisible (labels[i]);
        labels[i]->attachToComponent (sliders[i], false);
    }

    //// Dry/Wet ////
    sliders.add (new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliders[sliders.size() - 1]->setName ("dryGain");

    sliders[sliders.size() - 1]->setRange (0.0, 1.0, 0.001);
    sliders[sliders.size() - 1]->setValue (Global::dryWetInit);
    
    sliders[sliders.size() - 1]->addListener (this);
    addAndMakeVisible (sliders[sliders.size() - 1]);
    
    labels.add (new Label ("dryGain", "Dry/Wet"));
    addAndMakeVisible (labels[labels.size() - 1]);
    labels[labels.size() - 1]->setJustificationType(Justification::centred);
    labels[labels.size() - 1]->attachToComponent (sliders[sliders.size() - 1], false);
    
    //// Input gain ////
    sliders.add (new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliders[sliders.size() - 1]->setName ("inputGain");
    
    sliders[sliders.size() - 1]->setRange (0.0, 1.0, 0.001);
    sliders[sliders.size() - 1]->setValue (Global::initInputGain);
    
    sliders[sliders.size() - 1]->addListener (this);
    addAndMakeVisible (sliders[sliders.size() - 1]);
    
    labels.add (new Label ("inputGain", "Input Gain"));
    addAndMakeVisible (labels[labels.size() - 1]);
    labels[labels.size() - 1]->setJustificationType(Justification::centred);
    labels[labels.size() - 1]->attachToComponent (sliders[sliders.size() - 1], false);
    
    impulseBtn = std::make_unique<TextButton> ("Impulse");
    impulseBtn->addListener (this);
    addAndMakeVisible (impulseBtn.get());
    
    smoothVals = std::make_unique<TextButton> ("Smooth");
    smoothVals->addListener (this);
    addAndMakeVisible (smoothVals.get());
    defaultButtonColour = smoothVals->getLookAndFeel().findColour (TextButton::buttonColourId);
    
    allSliders = std::make_unique<TextButton> ("All Sliders");
    allSliders->addListener (this);
    addAndMakeVisible (allSliders.get());

    //// Presets ////
    presets = std::make_unique<ComboBox> ("Presets");
    presets->addItem ("-- no preset --", noPreset);
    presets->setItemEnabled (1, false);
    presets->addItem ("Small Room", smallRoom);
    presets->addItem ("Medium Room", mediumRoom);
    presets->addItem ("Large Room", largeRoom);
    presets->addItem ("Concert Hall", concertHall);
    presets->addItem ("Church", church);
    presets->addListener (this);
    addAndMakeVisible (presets.get());
    presets->setSelectedId (5);
    
    presetsLabel = std::make_unique<Label> ("presets", "Presets:");
    Font font (18.0);
    presetsLabel->setFont (font);
    addAndMakeVisible (presetsLabel.get());
    presetsLabel->setJustificationType(Justification::left);
    presetsLabel->setColour (Label::textColourId, Colours::white);
    
    if (Global::useAdvancedWindow)
    {
        advancedSettings = std::make_unique<TextButton>("Advanced");
        advancedSettings->addListener (this);
        addAndMakeVisible (advancedSettings.get());
        advancedSettingsWindow = std::make_unique<AdvancedSettingsWindow> (impulseBtn);
        
        fdnOrder = advancedSettingsWindow->getFDNOrderBox();
        scatMats = advancedSettingsWindow->getScatMatsBox();
        delayLines = advancedSettingsWindow->getDelayLinesBox();
        preDefBtn = advancedSettingsWindow->getPreDefBtn();
        applyRangeBtn = advancedSettingsWindow->getApplyRangeBtn();

        fdnOrder->addListener (this);
        scatMats->addListener (this);
        delayLines->addListener (this);
        preDefBtn->addListener (this);
        applyRangeBtn->addListener (this);

    } else {
        initialiseAdvancedSettings();
    }

    //// Fix coefficients ////
    fixCoeffs = std::make_unique<TextButton> ("Fix coeffs");
    fixCoeffs->addListener (this);
    addAndMakeVisible (fixCoeffs.get());
    
    //// Response ////
    for (int i = 0; i < Global::numOctaveBands; ++i)
    {
        std::cout << sliders[i]->getValue() << std::endl;
    }
    minDLenIdx = processor.getFDN()->getMinDLenIdx();
    maxDLenIdx = processor.getFDN()->getMaxDLenIdx();
    response = std::make_unique<Response> (processor.getFDN()->getMinDLen(), processor.getFDN()->getMaxDLen(), processor.getSampleRate());
    comboBoxChanged (presets.get());
    addAndMakeVisible (response.get());

    processor.getFDN()->recalculateCoeffs();
    calculateImpulseResponse();
    calculateEQ();
    
    // listen to the response IR button to refresh the coefficients asynchronously
    response->addChangeListener (this);
    
    //// CPU usage Audio ////
    cpuUsageAudio = std::make_unique<Label> ("cpuUsage", "CPU: ");
    addAndMakeVisible (cpuUsageAudio.get());
    cpuUsageAudio->setJustificationType (Justification::left);
    cpuUsageAudio->setColour (Label::textColourId, Colours::white);
    cpuUsageAudio->setColour (Label::backgroundColourId, defaultButtonColour);
    cpuUsageAudio->setColour (Label::outlineColourId, Colours::white);
    
    //// CPU usage Graphics ////
    cpuUsageGraphics = std::make_unique<Label> ("cpuUsage", "CPU: ");
    addAndMakeVisible (cpuUsageGraphics.get());
    cpuUsageGraphics->setJustificationType (Justification::left);
    cpuUsageGraphics->setColour (Label::textColourId, Colours::white);
    cpuUsageGraphics->setColour (Label::backgroundColourId, defaultButtonColour);
    cpuUsageGraphics->setColour (Label::outlineColourId, Colours::white);
    
    //// Timer and Size ////
    startTimerHz (Global::updatePerSecond);
    setSize (800, 700);
}

Fdn_AudioProcessorEditor::~Fdn_AudioProcessorEditor()
{
    stopTimer();
}

void Fdn_AudioProcessorEditor::initialiseAdvancedSettings()
{
    //// FDN order ////
    fdnOrder = std::make_shared<ComboBox> ("fdnOrder");
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
    fdnOrder->addListener (this);
    
    fdnOrderLabel = std::make_unique<Label> ("fdnOrderLabel", "FDN Order");
    addAndMakeVisible (fdnOrderLabel.get());
    fdnOrderLabel->setJustificationType(Justification::left);
    fdnOrderLabel->setColour (Label::textColourId, Colours::white);
    
    //// Scattering Matrices ////
    scatMats = std::make_shared<ComboBox> ("scatMats");
    scatMats->addItem ("Hadamard", hadamard);
    scatMats->addItem ("Householder", householder);
    scatMats->addItem ("Identity", identity);
    scatMats->addItem ("Random", randomMat);
    
    scatMats->setSelectedId (Global::initMatType);
    scatMats->addListener (this);
    addAndMakeVisible (scatMats.get());
    
    scatMatsLabel = std::make_unique<Label> ("scatMatsLabel", "Scattering Mat.");
    addAndMakeVisible (scatMatsLabel.get());
    scatMatsLabel->setJustificationType(Justification::left);
    scatMatsLabel->setColour (Label::textColourId, Colours::white);
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
                calculateImpulseResponse();
            }
        }
        
        // always do this for smooth transition to IR
        double avgVal = 0;
        for (int i = 0; i < Global::numOctaveBands; ++i)
            avgVal += sliders[i]->getValue();
        avgVal /= Global::numOctaveBands;
        
        response->setIRseconds (Global::limit (round (avgVal), 1.0, 15.0) + 0.5);
        
        //// there is no 'else' here so that instablility can be calculated
        calculateEQ();
        
        response->setInitialised();

        paintResponse = false;
        cpuUsageGraphicsValue = 0.001 * (Time::getCurrentTime().toMilliseconds() - time) / (1.0 / static_cast<double>(Global::updatePerSecond));
    }
}

void Fdn_AudioProcessorEditor::timerCallback()
{
    time = Time::getCurrentTime().toMilliseconds();
    
    if (processor.getFDN()->isInitialised())
    {
        if (!changingFDNorder)
        {
            paintResponse = true;
        } else {
            response->setDLens (processor.getFDN()->getMinDLen(), processor.getFDN()->getMaxDLen());
            changingFDNorder = false;
        }
    }
    if (!coeffsFixed)
        repaint();
    else
        cpuUsageGraphicsValue = 0.001 * (Time::getCurrentTime().toMilliseconds() - time) / (1.0 / static_cast<double>(Global::updatePerSecond));
    
    cpuUsageAudio->setText ("Audio CPU: " + String (static_cast<int>(processor.getCPU() * 1000.0) / 10.0) + "%", dontSendNotification);
    cpuUsageGraphics->setText ("Graphics CPU: " + String (static_cast<int>(cpuUsageGraphicsValue * 1000.0) / 10.0) + "%", dontSendNotification);
    
    
}



void Fdn_AudioProcessorEditor::calculateImpulseResponse()
{
    if (!processor.getFDN()->isInitialised())
        return;
    
    minDLenIdx = processor.getFDN()->getMinDLenIdx();
    maxDLenIdx = processor.getFDN()->getMaxDLenIdx();
    for (int i = 0; i <= Global::numOctaveBands; ++i)
        response->getIRComb()->setFilter (i, processor.getFDN()->getCoefficients (maxDLenIdx, i));
    response->calculateIR();
}

void Fdn_AudioProcessorEditor::calculateEQ()
{
    minDLenIdx = processor.getFDN()->getMinDLenIdx();
    maxDLenIdx = processor.getFDN()->getMaxDLenIdx();
    for (int i = 0; i < Global::numOctaveBands + 1; ++i)
    {
        std::vector<double> coeffs = processor.getFDN()->getCoefficients (minDLenIdx, i);
        response->calculateResponse (coeffs);
        
    }
    response->linearGainToDB();
}

void Fdn_AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle<int> totArea = getLocalBounds();
    
    Rectangle<int> sidePanel = totArea.removeFromRight (120);
    sidePanel.reduce (Global::margin, Global::margin);

    //// Preset Area ////
    Rectangle<int> presetArea = totArea.removeFromTop (40);
    presetArea.removeFromLeft (Global::margin * 0.5);
    presetArea.removeFromTop (Global::margin);
    presetsLabel->setBounds (presetArea.removeFromLeft (70));
    cpuUsageGraphics->setBounds (presetArea.removeFromRight (140));
    presetArea.removeFromRight (Global::margin);
    cpuUsageAudio->setBounds (presetArea.removeFromRight (140));
    presetArea.removeFromRight (Global::margin);
    presets->setBounds (presetArea);
    
    //// Side panel ////
    
    // Top Half //
    fixCoeffs->setBounds (sidePanel.removeFromTop (40 - Global::margin));
    sidePanel.removeFromTop (Global::margin * 4.0);
    int knobHeight = sidePanel.getHeight() * 0.2 - Global::margin;
    sliders[sliders.size() - 1]->setBounds (sidePanel.removeFromTop (knobHeight));
    sidePanel.removeFromTop (Global::margin * 3.0);
    sliders[sliders.size() - 2]->setBounds (sidePanel.removeFromTop (knobHeight));
    sidePanel.removeFromTop (Global::margin * 3.0);
    
    // Bottom Half //
    impulseBtn->setBounds (sidePanel.removeFromBottom (40));
    sidePanel.removeFromBottom (Global::margin);
    smoothVals->setBounds (sidePanel.removeFromBottom (40));
    sidePanel.removeFromBottom (Global::margin);
    allSliders->setBounds (sidePanel.removeFromBottom (40));
   
    if (Global::useAdvancedWindow)
    {
        sidePanel.removeFromBottom (Global::margin);
        advancedSettings->setBounds (sidePanel.removeFromBottom (40));
    } else {
        sidePanel.removeFromBottom (Global::margin);
        scatMats->setBounds (sidePanel.removeFromBottom (40));
        sidePanel.removeFromBottom (Global::margin * 0.5);
        scatMatsLabel->setBounds (sidePanel.removeFromBottom (20).withX (sidePanel.getX() - 5));
        sidePanel.removeFromBottom (Global::margin);
        fdnOrder->setBounds (sidePanel.removeFromBottom (40));
        sidePanel.removeFromBottom (Global::margin * 0.5);
        fdnOrderLabel->setBounds (sidePanel.removeFromBottom (20).withX (sidePanel.getX() - 5));
    }
    
    //// Response area ////
    Rectangle<int> responseArea = totArea.removeFromTop (getHeight() * 0.5);
    responseArea.reduce(0, Global::margin);
    responseArea.removeFromLeft (Global::margin);
    response->setBounds (responseArea);
    
    //// Sliders ////
    totArea.removeFromTop (20);
    totArea.removeFromBottom (Global::margin);

    int sliderMargin = 10;
    totArea.removeFromLeft (Global::margin);
    int sliderWidth = ceil ((totArea.getWidth() - (9.0 * sliderMargin)) / 10.0);
    
    for (int i = 0; i < sliders.size() - 2; ++i)
    {
        sliders[i]->setBounds (totArea.removeFromLeft (sliderWidth));
        totArea.removeFromLeft (sliderMargin);
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
    if (button == impulseBtn.get())
    {
        processor.setZeroCoeffsFlag();
    }
    else if (button == advancedSettings.get())
    {
        openAdvancedSettings();
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
    else if (button == fixCoeffs.get())
    {
        coeffsFixed = !coeffsFixed;
        if (coeffsFixed)
        {
            fixCoeffs->setButtonText("Unfix coeffs");
            presets->setEnabled (false);
            for (int i = 0; i < Global::numOctaveBands; ++i)
            {
                sliders[i]->setEnabled (false);
            }
        } else {
            fixCoeffs->setButtonText("Fix coeffs");
            presets->setEnabled (true);
            for (int i = 0; i < Global::numOctaveBands; ++i)
            {
                sliders[i]->setEnabled (true);
            }
        }
        processor.fixCoefficients (coeffsFixed);
    }
    else if (button == applyRangeBtn.get() || button == preDefBtn.get())
    {
        if (button == preDefBtn.get())
            preDefOn = !preDefOn; // using two separate preDefOn booleans (this and the one in advancedSettingsWindow) as one comes before the other in an order that I'm unsure about
        if (preDefOn)
            processor.changeDelayLineSetting (static_cast<DelayLineSetting> (delayLines->getSelectedId() + 3), Global::minDelayLength, Global::maxDelayLength);
        else
            processor.changeDelayLineSetting (static_cast<DelayLineSetting> (delayLines->getSelectedId()), advancedSettingsWindow->getRangeSliderMin(), advancedSettingsWindow->getRangeSliderMax());
    }
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
				orderToSwitchTo = 2;
                break;
			case 2:
				orderToSwitchTo = 4;
                break;
            case 3:
                orderToSwitchTo = 8;
                break;
            case 4:
                orderToSwitchTo = 16;
                break;
            case 5:
                orderToSwitchTo = 32;
                break;
            case 6:
                orderToSwitchTo = 64;
                break;
        }
        
        if (processor.getFDN()->getFDNorder() == orderToSwitchTo)
        {
            return;
        }
        
        changingFDNorder = true;
        processor.getFDN()->setInitialised (false);
        processor.changeFDNorder (orderToSwitchTo, static_cast<MatrixType> (scatMats->getSelectedId()));

        std::cout << "FDN order changed with maxDLenIdx = " << maxDLenIdx << std::endl;
    }
    else if (comboBoxThatHasChanged == scatMats.get())
    {
        processor.changeMatType (static_cast<MatrixType> (scatMats->getSelectedId()));
    }
    else if (comboBoxThatHasChanged == delayLines.get())
    {
        if (preDefOn)
            processor.changeDelayLineSetting (static_cast<DelayLineSetting> (delayLines->getSelectedId() + 3), Global::minDelayLength, Global::maxDelayLength);
        else
            processor.changeDelayLineSetting (static_cast<DelayLineSetting> (delayLines->getSelectedId()), advancedSettingsWindow->getRangeSliderMin(), advancedSettingsWindow->getRangeSliderMax());
        

    }
}

void Fdn_AudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* source)
{
    // if a message is received it is ALWAYS the IR button clicked (to show IR)
    // now asynchronously calculate the impulse response
    calculateImpulseResponse();
};

void Fdn_AudioProcessorEditor::openAdvancedSettings()
{
    DialogWindow::LaunchOptions dlg;
    int dlgModal = -1;
    addAndMakeVisible (advancedSettingsWindow.get());

    dlg.dialogTitle = "Advanced Settings";
    dlg.content.set (advancedSettingsWindow.get(), false);
    dlgModal = dlg.runModal();
}
