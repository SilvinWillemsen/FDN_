/*
  ==============================================================================

    Response.cpp
    Created: 15 Nov 2019 9:14:32am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Response.h"

//==============================================================================
Response::Response (double dLen, double fs) : fs (fs), dLen (dLen)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    linearData.resize (Global::fftOrder, 0);
    dBData.resize (Global::fftOrder, 0);
	RTData.resize (Global::fftOrder, 0);
    IRdata.resize (Global::IRseconds * Global::IRsamplesPerSecond, 0);

    noiseBurst.resize (dLen, 0);
    
	//targetRT.resize(Global::fftOrder, 0);
    Random r;
    for (int i = 0; i < dLen; ++i)
    {
        noiseBurst[i] = 2.0 * r.nextDouble() - 1;
    }
    drawToggles.resize(2, true);
    
    if (Global::showRTGainButtons)
    {
        for (int i = 0; i < 2; ++i)
        {
            buttons.add (new TextButton());
            buttons[i]->setButtonText (i == 0 ? "Gain" : "Reverb Time");
            buttons[i]->setColour(TextButton::buttonColourId, Colours::white);
            buttons[i]->setColour(TextButton::textColourOnId, i == 0 ? Colours::red : Colours::black);
            buttons[i]->setColour(TextButton::textColourOffId, i == 0 ? Colours::red : Colours::black);

            buttons[i]->addListener (this);
            addAndMakeVisible (buttons[i]);
        }
    }
    
    gainLabel = std::make_unique<Label> ("Gain", "Gain (dB)");
    gainLabel->setColour(Label::textColourId, Colours::red);
    gainLabel->setColour(Label::backgroundColourId, Colours::white.withAlpha(0.0f));
    gainLabel->setJustificationType(Justification::centred);
    addAndMakeVisible (gainLabel.get());
    
    RTLabel = std::make_unique<Label> ("RT", "RT (s)");
    RTLabel->setColour(Label::textColourId, Colours::black);
    RTLabel->setColour(Label::backgroundColourId, Colours::white.withAlpha(0.0f));
    RTLabel->setJustificationType(Justification::centred);
    addAndMakeVisible (RTLabel.get());
    
    ampLabel = std::make_unique<Label> ("Amp", "Amplitude");
    ampLabel->setColour(Label::textColourId, Colours::black);
    ampLabel->setColour(Label::backgroundColourId, Colours::white.withAlpha(0.0f));
    ampLabel->setJustificationType(Justification::centred);
    addAndMakeVisible (ampLabel.get());
    
    if (Global::showLogSlider)
    {
        //// For changing the logbase of the plot
        logBaseSlider = std::make_unique<Slider>(Slider::LinearHorizontal, Slider::TextBoxLeft);
        logBaseSlider->setName ("logBase");
    
        logBaseSlider->setRange (1.001, 10000.0, 0.001);
        logBaseSlider->setSkewFactorFromMidPoint (100.0);
        logBaseSlider->setValue (Global::logBase);
        logBaseSlider->setColour (Slider::textBoxTextColourId, Colours::black);
        logBaseSlider->addListener (this);
        addAndMakeVisible (logBaseSlider.get());
    
        logLabel = std::make_unique<Label> ("logBase", "Log Base");
        addAndMakeVisible (logLabel.get());
        logLabel->setColour (Label::backgroundColourId, Colours::white);
        logLabel->setColour (Label::textColourId, Colours::black);
        logLabel->attachToComponent (logBaseSlider.get(), true);

        logButton = std::make_unique<TextButton> ("Grid Type");
        logButton->addListener (this);
        addAndMakeVisible (logButton.get());
        
    } else {
        IRButton = std::make_unique<TextButton> (Global::initShowIR ? "Show EQ" : "Show IR");
        IRButton->addListener (this);
        addAndMakeVisible (IRButton.get());
    }

    initialiseIRComb();
    
}

Response::~Response()
{
}

void Response::paint (Graphics& g)
{
    
//    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(Colours::white);
    if (unstable)
    {
        g.setColour (unstableColour);
        g.fillRect (getLocalBounds());
    }

    if (showingIR)
    {
        g.setColour (Colours::black);
        g.strokePath (generateResponsePath (IRdata, 200), PathStrokeType (1.0f));
       
        //// Draw gridlines ////
        g.setColour (Colours::lightgrey);
        int xLoc = 0;
        for (int i = 1; i < Global::IRseconds; ++i)
        {
            xLoc = (i / Global::IRseconds) * plotWidth + Global::axisMargin;
            g.drawLine (xLoc, 0, xLoc, getHeight() - Global::axisMargin, 1.0f);
        }
    } else {
        //// Draw gridlines ////
        g.setColour (Colours::lightgrey);
        for (int n = 0; n < gridLineCoords.size(); ++n)
        {
            g.drawLine (gridLineCoords[n] * plotWidth + Global::axisMargin, 0, gridLineCoords[n] * plotWidth + Global::axisMargin, getHeight() - Global::axisMargin, 1.0f);
        }
        
        //// Draw dB data ////
        if (drawToggles[0])
        {
            int visualScaling = 10;
            g.setColour (Colours::red);
            g.strokePath (generateResponsePath (dBData, visualScaling), PathStrokeType(2.0f));
            g.setColour (Colours::red.withAlpha (0.3f));
            for (int i = -2; i >= -8; i = i - 2)
            {
                g.drawLine (Global::axisMargin, -i * visualScaling + zeroDbHeight, getWidth(), -i * visualScaling + zeroDbHeight);
                g.drawText (String(i), 0, -i * visualScaling + zeroDbHeight - 10, Global::axisMargin - 5.0, 20, Justification::centredRight);
            }
        }
        g.setColour(Colours::black);
        g.drawText(String(0), 0, zeroDbHeight - 10, Global::axisMargin - 5.0, 20, Justification::centredRight);
        
        //// Draw Reverb Time data ////
        if (drawToggles[1])
        {
            int visualScaling = 10;

            g.setColour(Colours::black);
            g.strokePath (generateResponsePath (RTData, visualScaling), PathStrokeType(2.0f));
            g.setColour(Colours::black.withAlpha (0.3f));
            for (int i = 2; i <= 19; i = i + 2)
            {
                g.drawLine (Global::axisMargin, -i * visualScaling + zeroDbHeight, getWidth(), -i * visualScaling + zeroDbHeight);
                g.drawText (String(i), 0, -i * visualScaling + zeroDbHeight - 10, Global::axisMargin - 5.0, 20, Justification::centredRight);
            }
        }
    }
    
    g.setColour (unstable ? unstableColour : Colours::white);
    g.fillRect(Global::axisMargin, getHeight() - Global::axisMargin, getWidth() - Global::axisMargin, getHeight() - Global::axisMargin);
    
    g.setColour (Colours::black);
    // Draw axis lines
    g.drawLine (Global::axisMargin, 0, Global::axisMargin, getHeight() - Global::axisMargin, 1.0f);
    g.drawLine (Global::axisMargin, getHeight() - Global::axisMargin, getWidth(), getHeight() - Global::axisMargin, 1.0f);
    
    if (showingIR)
    {
        int xLoc = 0;
        g.setColour (Colours::darkgrey);
        for (int i = 1; i <= Global::IRseconds; ++i)
        {
            xLoc = (i / Global::IRseconds) * plotWidth + Global::axisMargin;
            g.drawText(String(i), xLoc - 20, getHeight() - Global::axisMargin, 40, 20, Justification::centred);
        }
        g.drawText ("Time (s)", (getWidth() + Global::axisMargin) * 0.5 - 40, getHeight() - 0.5 * Global::axisMargin - 10, 80, 30, Justification::centred);
    } else {
        // Draw zero DB / RT line
        g.drawLine (Global::axisMargin, zeroDbHeight, getWidth(), zeroDbHeight, 1.0);
        
        g.setColour (Colours::darkgrey);
        int j = 1;
        for (int n = 0; n < gridLineCoords.size(); ++n)
        {
            if (drawBandLines)
                g.drawText(String(125.0 * pow(2.0, n-2)), gridLineCoords[n] * plotWidth + Global::axisMargin - 20, getHeight() - Global::axisMargin, 40, 20, Justification::centred);
            else
            {
                int lim = 0; // varibale for deciding how many lines are shown in the non-band-lines case
                g.drawText(String(pow(10, j) * ((n+1) % 10)), gridLineCoords[n] * plotWidth + Global::axisMargin - 20, getHeight() - Global::axisMargin, 40, 20, Justification::centred);
                
                if (n % 10 == lim)
                {
                    n += (9 - lim);
                    ++j;
                }
            }
        }
        g.drawText("Frequency (Hz)", (getWidth() + Global::axisMargin) * 0.5 - 50, getHeight() - Global::axisMargin * 0.5 - 10, 100, 30, Justification::centred);
    }
    if (Global::showLogSlider)
    {
        g.setColour (Colours::white);
        Rectangle<int> bounds = logBaseSlider->getBounds();
        bounds.setX(bounds.getX() - logLabel->getWidth() - 1);
        bounds.setY(bounds.getY() - 10);
        bounds.setHeight(bounds.getHeight() + 20);
        bounds.setWidth(bounds.getWidth() + 120 + logLabel->getWidth() + 1);
        g.fillRect (bounds);
        g.setColour(Colours::black);
        g.drawRect (bounds);
    }
}
Path Response::generateResponsePath (std::vector<double>& data, float visualScaling)
{
    Path response;
    //response.startNewSubPath(0, -dBData[0] * visualScaling + zeroDbHeight);
    response.startNewSubPath(Global::axisMargin, -data[0] * visualScaling + zeroDbHeight); // draw RT instead of filter magnitude
    auto spacing = (plotWidth) / static_cast<double> (data.size());
    auto x = spacing + Global::axisMargin;
    float newY;
    for (int y = 0; y < data.size(); y++)
    {
        newY = -data[y] * visualScaling + zeroDbHeight;
        response.lineTo(x, newY);
        x += spacing;
    }
    response.lineTo (getWidth(), newY);
    
    return response;
}

void Response::resized()
{
    if (showingIR)
    {
        zeroDbHeight = (getHeight() - Global::axisMargin) * 0.5;
        ampLabel->setVisible (true);
        RTLabel->setVisible (false);
        gainLabel->setVisible (false);
        
        AffineTransform transformAmp;
        transformAmp = transformAmp.rotated (-0.5 * double_Pi, Global::axisMargin, zeroDbHeight);
        transformAmp = transformAmp.translated (-Global::axisMargin, 0);
        ampLabel->setTransform (transformAmp);
        ampLabel->setBounds (getLocalBounds().removeFromLeft (Global::axisMargin * 2.0));
            
    } else {
        ampLabel->setVisible (false);
        RTLabel->setVisible (true);
        gainLabel->setVisible (true);

        zeroDbHeight = getHeight() * Global::zeroDbRatio;
        plotWidth = getWidth() - Global::axisMargin;
        
        //// Calculate grid-lines ////
        setLogBase (Global::logBase, true);
        
        Rectangle<int> leftAxis = getLocalBounds().removeFromLeft (Global::axisMargin * 2.0);
        AffineTransform transformRT;
        transformRT = transformRT.rotated (-0.5 * double_Pi, Global::axisMargin, zeroDbHeight * 0.5);
        transformRT = transformRT.translated (-Global::axisMargin * 0.75, 0);
        RTLabel->setTransform (transformRT);

        AffineTransform transformGain;
        transformGain = transformGain.rotated (-0.5 * double_Pi, Global::axisMargin, getHeight() - (Global::axisMargin + getHeight()-zeroDbHeight) * 0.5);
        transformGain = transformGain.translated(-Global::axisMargin * 0.75, 0);
        gainLabel->setTransform (transformGain);
        
        leftAxis.removeFromBottom (Global::axisMargin);
        RTLabel->setBounds(leftAxis.removeFromTop (zeroDbHeight));
        gainLabel->setBounds(leftAxis);
    }
    
    Rectangle<int> topControlArea (0, 0, getWidth(), 100);
    topControlArea.reduce(10, 10);
    
    if (Global::showRTGainButtons)
    {
        Rectangle<int> legendArea = topControlArea.removeFromRight (100);
        buttons[0]->setBounds(legendArea.removeFromBottom (35));
        legendArea.removeFromBottom (10);
        buttons[1]->setBounds(legendArea.removeFromBottom (35));
        topControlArea.removeFromLeft(30);
    }
    
    if (Global::showLogSlider)
    {
        topControlArea.removeFromRight (20);
        topControlArea.removeFromBottom (topControlArea.getHeight() * 0.5);
        logButton->setBounds (topControlArea.removeFromRight(100));
        topControlArea.removeFromRight (10);
        logBaseSlider->setBounds (topControlArea.removeFromRight (getWidth() * 0.4));
    } else {
        topControlArea.removeFromBottom (topControlArea.getHeight() * 0.6);
        IRButton->setBounds (topControlArea.removeFromRight(100));
    }
}

void Response::calculateResponse (std::vector<double> coefficients)
{
    std::complex<double> i (0.0, 1.0);
    std::complex<double> omega (0.0, 0.0);
//    double testDiv = abs(log10(1.0 / static_cast<double>(Global::fftOrder)));
    for (int k = 1; k <= Global::fftOrder; ++k)
    {
//       omega.real (k / static_cast<double>(Global::fftOrder) * double_Pi);
        double linearVal = k / static_cast<double>(Global::fftOrder);
        omega.real (double_Pi * ((pow (logBase, linearVal) - 1.0) / (logBase - 1.0)));
//        omega.real ((log10(k / static_cast<double>(Global::fftOrder)) / testDiv + 1) * double_Pi);
        linearData[k-1] *= (coefficients[0] + coefficients[1] * exp(-i * omega) + coefficients[2] * exp(-2.0 * i * omega))
                                     / (coefficients[3] + coefficients[4] * exp(-i * omega) + coefficients[5] * exp(-2.0 * i * omega));
    }
}


void Response::linearGainToDB()
{
    unstable = false;
	for (int i = 0; i < Global::fftOrder; ++i)
    {
		dBData[i] = Global::limit (20.0 * log10(abs(linearData[i])), -60.0, 10.0);
        if (dBData[i] >= 0)
            unstable = true;
        RTData[i] = -60.0 * (dLen) / (Global::limit(dBData[i], -1e10, -1e-10) * fs); // calculate RT - this should be -60*dLen/RT*fs
	}
//    if (init) // update the delay line length based on RT = 1
//    {
//        dLen = (Global::limit (dBData[0], -1e10, -1e-10) * fs) / (-60.0);
//        std::cout << "Delay line length used for RT = " << dLen << std::endl;
//        init = false;
//        initialiseIRComb();
//        linearGainToDB();
//    }
}

void Response::buttonClicked (Button* button)
{
    if (button == logButton.get())
    {
        changeGrid();
        return;
    }
    else if (button == IRButton.get())
    {
        IRButton->setButtonText(showingIR ? "Show IR" : "Show EQ");
        showingIR = !showingIR;
        resized();
    }
    
    for (int i = 0; i < buttons.size(); ++i)
        if (button == buttons[i])
        {
            drawToggles[i] = !drawToggles[i];
            return;
        }
}

void Response::sliderValueChanged (Slider* slider)
{
    if (slider == logBaseSlider.get())
    {
        setLogBase (slider->getValue());
    }
}

void Response::setLogBase (double val, bool init)
{
    logBase = val;
    
    if (drawBandLines)
    {
        if (init)
        {
            std::vector<double> tmp (Global::numOctaveBands, 0);
            gridLineCoords = tmp;
        }
        
        for (int n = 1; n <= Global::numOctaveBands; ++n)
        {
            double y = 125 * pow(2,n-3);
            gridLineCoords[n-1] = log(y * (logBase - 1.0) / (fs * 0.5) + 1.0) / log(logBase);
        }
    } else {
        if (init)
        {
            std::vector<double> tmp (32, 0);
            gridLineCoords = tmp;
        }
        int m = 10;
        int n = 0;
        for (int i = 1; i <= gridLineCoords.size(); ++i)
        {
            ++n;
            double y = m * n;
            gridLineCoords[i-1] = log(y * (logBase - 1.0) / (fs * 0.5) + 1.0) / log(logBase);
            if (n % 10 == 0)
            {
                m *= 10;
                n = 0;
            }
            
        }
    }
        
}

void Response::initialiseIRComb()
{
    IRComb = std::make_shared<EQComb> (dLen);
}

void Response::calculateIR()
{
    double output;
    int idx = 0;
    IRComb->zeroCoefficients();
    for (int i = 0; i < Global::IRseconds * fs; ++i)
    {
        output = IRComb->filter (i < dLen ? noiseBurst[i] : 0);
        IRComb->addScatOutput (output);
        IRComb->increment();
        IRComb->zeroWritePointer();
        if (i % static_cast<int> (fs / Global::IRsamplesPerSecond) == 0)
        {
            IRdata[idx] = output;
            ++idx;
        }
    }
}
