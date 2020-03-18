/*
  ==============================================================================

    Response.h
    Created: 15 Nov 2019 9:14:32am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Global.h"
#include "FDN.h"
#include <valarray>
#include <complex>
//==============================================================================
/*
*/
class Response    : public Component, public Button::Listener
{
public:
    Response (double fs);
    ~Response();

    void paint (Graphics&) override;
    void resized() override;

    Path generateResponsePath (std::vector<double>& data, bool drawdB);
    
    void setDataToZero() {
        for (int i = 0; i < fftOrder; ++i)
        {
            linearData[i].real (1.0);
            linearData[i].imag (0.0);
        }
    }
    void calculateResponse (std::vector<double> coefficients);
	void calculateTargetResponse(std::vector<double> gainDB, std::vector<double> RT);
    void linearGainToDB();

    void buttonClicked (Button* button) override;
private:
    int fftOrder = 4000;
    double fs;
	std::vector<double> dLen;
	std::vector<std::vector<double>> gainDB;
    
    std::vector<std::complex<double>> linearData;
    std::vector<double> dBData;
	std::vector<double> RTData;
	std::vector<double> targetRT;

    OwnedArray<TextButton> buttons;
    std::vector<bool> drawToggles;
    
    bool unstable = false;
    double zeroDbRatio = 0.6;
    double zeroDbHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Response)
};
