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
Response::Response (double dLen, double fs) : dLen (dLen), fs (fs)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    linearData.resize (fftOrder, 0);
    dBData.resize (fftOrder, 0);
	RTData.resize(fftOrder, 0);
	//targetRT.resize(fftOrder, 0);
    
    drawToggles.resize(2, true);
    
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
        g.setColour (Colours::red.withAlpha (0.5f));
        g.fillRect (getLocalBounds());
    }
    
    //// Draw gridlines ////

    g.setColour (Colours::black);
    g.drawLine (Global::axisMargin, 0, Global::axisMargin, getHeight(), 1.0f);
    g.drawLine (Global::axisMargin, zeroDbHeight, getWidth(), zeroDbHeight, 1.0);

    g.setColour (Colours::lightgrey);
    for (int n = 0; n < gridLineCoords.size(); ++n)
    {
        g.drawLine (gridLineCoords[n] * plotWidth + Global::axisMargin, 0, gridLineCoords[n] * plotWidth + Global::axisMargin, getHeight(), 1.0f);
    }
    if (drawToggles[0])
    {
        int visualScaling = 10;
        g.setColour (Colours::red);
        g.strokePath (generateResponsePath (dBData, true), PathStrokeType(2.0f));
        g.setColour (Colours::red.withAlpha (0.3f));
        for (int i = -2; i >= -8; i = i - 2)
        {
            g.drawLine(Global::axisMargin, -i * visualScaling + zeroDbHeight, getWidth(), -i * visualScaling + zeroDbHeight);
            g.drawText(String(i), 0, -i * visualScaling + zeroDbHeight - 10, Global::axisMargin - 5.0, 20, Justification::centredRight);
        }
    }
    g.setColour(Colours::black);
    g.drawText(String(0), 0, zeroDbHeight - 10, Global::axisMargin - 5.0, 20, Justification::centredRight);
    if (drawToggles[1])
    {
        int visualScaling = 15;

        g.setColour(Colours::black);
        g.strokePath (generateResponsePath (RTData, false), PathStrokeType(2.0f));
        g.setColour(Colours::black.withAlpha (0.3f));
        for (int i = 2; i <= 15; i = i + 2)
        {
            g.drawLine(Global::axisMargin, -i * visualScaling + zeroDbHeight, getWidth(), -i * visualScaling + zeroDbHeight);
            g.drawText(String(i), 0, -i * visualScaling + zeroDbHeight - 10, Global::axisMargin - 5.0, 20, Justification::centredRight);
        }
    }
    
    g.setColour (Colours::lightgrey);
}
Path Response::generateResponsePath (std::vector<double>& data, bool drawdB)
{
    Path response;
    float visualScaling = drawdB ? 10 : 15;
    //response.startNewSubPath(0, -dBData[0] * visualScaling + zeroDbHeight);
    response.startNewSubPath(Global::axisMargin, -data[0] * visualScaling + zeroDbHeight); // draw RT instead of filter magnitude
    auto spacing = (plotWidth) / static_cast<double> (fftOrder);
    auto x = spacing + Global::axisMargin;
    float newY;
    for (int y = 0; y < fftOrder; y++)
    {
        newY = -data[y] * visualScaling + zeroDbHeight;
        response.lineTo(x, newY);
        x += spacing;
    }
    response.lineTo(getWidth(), newY);
    
    return response;
}

void Response::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    zeroDbHeight = getHeight() * Global::zeroDbRatio;
    Rectangle<int> legendArea (getWidth() - 100, 0, 100, 100);
    plotWidth = getWidth() - Global::axisMargin;
    
    //// Calculate grid-lines ////
    setLogBase (Global::logBase, true);
    
    legendArea.reduce (10, 10);
    buttons[0]->setBounds(legendArea.removeFromBottom (35));
    legendArea.removeFromBottom (10);
    buttons[1]->setBounds(legendArea.removeFromBottom (35));

    Rectangle<int> leftAxis = getLocalBounds().removeFromLeft (Global::axisMargin * 2.0);
    AffineTransform transformRT;
    transformRT = transformRT.rotated (-0.5 * double_Pi, Global::axisMargin, zeroDbHeight * 0.5);
    transformRT = transformRT.translated(-Global::axisMargin * 0.75, 0);
    RTLabel->setTransform (transformRT);

    AffineTransform transformGain;
    transformGain = transformGain.rotated (-0.5 * double_Pi, Global::axisMargin, getHeight() - (getHeight()-zeroDbHeight) * 0.5);
    transformGain = transformGain.translated(-Global::axisMargin * 0.75, 0);
    gainLabel->setTransform (transformGain);
    
    RTLabel->setBounds(leftAxis.removeFromTop (zeroDbHeight));
    gainLabel->setBounds(leftAxis);
}

void Response::calculateResponse (std::vector<double> coefficients)
{
    std::complex<double> i (0.0, 1.0);
    std::complex<double> omega (0.0, 0.0);
//    double testDiv = abs(log10(1.0 / static_cast<double>(fftOrder)));
    for (int k = 1; k <= fftOrder; ++k)
    {
//       omega.real (k / static_cast<double>(fftOrder) * double_Pi);
        double linearVal = k / static_cast<double>(fftOrder);
        omega.real (double_Pi * ((pow (logBase, linearVal) - 1.0) / (logBase - 1.0)));
//        omega.real ((log10(k / static_cast<double>(fftOrder)) / testDiv + 1) * double_Pi);
        linearData[k-1] *= (coefficients[0] + coefficients[1] * exp(-i * omega) + coefficients[2] * exp(-2.0 * i * omega))
                                     / (coefficients[3] + coefficients[4] * exp(-i * omega) + coefficients[5] * exp(-2.0 * i * omega));
    }
}

//void Response::calculateTargetResponse(std::vector<double> gainDB, std::vector<double> RT)
//{
//	double delta;
//	double vecLen = fftOrder / 10.0f;
//	std::vector<double> linVec;
//
//	for (int i = 0; i < 9; ++i)
//	{
//		delta = (RT[i+1] - RT[i]) / vecLen;
//		for (int j = 0; j < vecLen; ++j)
//			linVec.push_back(RT[i] + j * delta);
//	}
//}


void Response::linearGainToDB()
{
    unstable = false;
	for (int i = 0; i < fftOrder; ++i)
    {
		dBData[i] = Global::limit (20.0 * log10(abs(linearData[i])), -60.0, 10.0);
        if (dBData[i] >= 0)
            unstable = true;
        RTData[i] = -60.0 * (dLen) / (Global::limit(dBData[i], -1e10, -1e-10) * fs); // calculate RT - this should be -60*dLen/RT*fs
	}
    if (init) // update the delay line length based on RT = 1
    {
        dLen = (Global::limit (dBData[0], -1e10, -1e-10) * fs) / (-60.0);
        std::cout << "Delay line length used for RT = " << dLen << std::endl;
        init = false;
        linearGainToDB();
    }
}

void Response::buttonClicked (Button* button)
{
    for (int i = 0; i < buttons.size(); ++i)
        if (button == buttons[i])
        {
            drawToggles[i] = !drawToggles[i];
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
