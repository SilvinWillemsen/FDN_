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
#include <valarray>
#include <complex>
//==============================================================================
/*
*/
class Response    : public Component
{
public:
    Response (double fs);
    ~Response();

    void paint (Graphics&) override;
    void resized() override;

    Path generateResponsePath();
    
    void setDataToZero() {
        for (int i = 0; i < fftOrder; ++i)
        {
            linearData[i].real (1.0);
            linearData[i].imag (0.0);
        }
    }
    void calculateResponse (std::vector<double> coefficients);
    
    void linearGainToDB();
    
private:
    int fftOrder = 4096;
    double fs;
    
    std::vector<std::complex<double>> linearData;
    std::vector<double> dBData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Response)
};
