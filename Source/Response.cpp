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
}

Response::~Response()
{
}

void Response::paint (Graphics& g)
{
    g.fillAll (Colours::white);   // clear the background
    g.strokePath (generateResponsePath(), PathStrokeType(2.0f));
    g.setColour (Colours::lightgrey);
    g.drawLine (0, getHeight() * 0.5, getWidth(), getHeight() * 0.5, 1.0);

}
Path Response::generateResponsePath()
{
    auto zeroDbHeight = getHeight() * 0.5;
    Path response;
	double visualScaling = 1;// 10;
    //response.startNewSubPath(0, -dBData[0] * visualScaling + zeroDbHeight);
	response.startNewSubPath(0, -RTData[0] * visualScaling + zeroDbHeight); // draw RT instead od filter magnitude
   
    auto spacing = getWidth() / static_cast<double> (fftOrder);
    auto x = spacing;
    float newY;
	float newYY;
    for (int y = 0; y < fftOrder; y++)
    {
        newY = -dBData[y] * visualScaling + zeroDbHeight;
		//newY = -RTData[y] * visualScaling + zeroDbHeight;// draw RT instead od filter magnitude
		
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

}

void Response::calculateResponse (std::vector<double> coefficients)
{
    std::complex<double> i (0.0, 1.0);
    std::complex<double> omega (0.0, 0.0);
//    double testDiv = abs(log10(1.0 / static_cast<double>(fftOrder)));
    for (int k = 1; k <= fftOrder; ++k)
    {
       omega.real (k / static_cast<double>(fftOrder) * double_Pi);
        double linearVal = k / static_cast<double>(fftOrder);
        //omega.real (double_Pi * ((pow (10, linearVal) - 1.0) / 9.0));
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
	for (int i = 0; i < fftOrder; ++i) {
		dBData[i] = Global::limit(20.0 * log10(abs(linearData[i])), -60.0, 10.0);
		
		RTData[i] = -60.0  / (dBData[i]); // calculate RT - this should be -60*dLen/RT*fs

	}
        
}
