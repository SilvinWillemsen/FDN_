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
Response::Response (double fs) : fs (fs)
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
    
}

Response::~Response()
{
}

void Response::paint (Graphics& g)
{
    
    g.fillAll (Colours::white);   // clear the background
    if (unstable)
    {
        g.setColour (Colours::red.withAlpha (0.5f));
        g.fillRect (getLocalBounds());
    }
    if (drawToggles[0])
    {
        g.setColour (Colours::red);
        g.strokePath (generateResponsePath (dBData, true), PathStrokeType(2.0f));
    }
    
    if (drawToggles[1])
    {
        g.setColour(Colours::black);
        g.strokePath (generateResponsePath (RTData, false), PathStrokeType(2.0f));
    }
    
    g.setColour (Colours::lightgrey);
    g.drawLine (0, zeroDbHeight, getWidth(), zeroDbHeight, 1.0);
    
}
Path Response::generateResponsePath (std::vector<double>& data, bool drawdB)
{
    Path response;
    float visualScaling = drawdB ? 10 : 1;
    //response.startNewSubPath(0, -dBData[0] * visualScaling + zeroDbHeight);
	response.startNewSubPath(0, -data[0] * visualScaling + zeroDbHeight); // draw RT instead of filter magnitude
    auto spacing = getWidth() / static_cast<double> (fftOrder);
    auto x = spacing;
    float newY;
    for (int y = 0; y < fftOrder; y++)
    {
//        newY = -dBData[y] * visualScaling + zeroDbHeight;
        
        newY = -data[y] * visualScaling + zeroDbHeight;// draw RT instead od filter magnitude
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
    zeroDbHeight = getHeight() * zeroDbRatio;
    Rectangle<int> legendArea (getWidth() - 100, 0, 100, 100);
    
    legendArea.reduce (10, 10);
    buttons[0]->setBounds(legendArea.removeFromBottom (35));
    legendArea.removeFromBottom (10);
    buttons[1]->setBounds(legendArea.removeFromBottom (35));

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
        omega.real (double_Pi * ((pow (1000, linearVal) - 1.0) / 999.0));
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
		RTData[i] = -60.0 / Global::limit(dBData[i], -1e10, -1e-10); // calculate RT - this should be -60*dLen/RT*fs
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
