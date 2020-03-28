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
class Response    : public Component,
                    public Button::Listener,
                    public Slider::Listener
{
public:
    Response (double avgDLen, double fs);
    ~Response();

    void paint (Graphics&) override;
    void resized() override;

    Path generateResponsePath (std::vector<double>& data, float visualScaling);
    
    void setDataToZero() {
        for (int i = 0; i < Global::fftOrder; ++i)
        {
            linearData[i].real (1.0);
            linearData[i].imag (0.0);
        }
    }
    void calculateResponse (std::vector<double> coefficients);
	void calculateTargetResponse(std::vector<double> gainDB, std::vector<double> RT);
    void linearGainToDB();

    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    void setLogBase (double val, bool init = false);
    void changeGrid() { drawBandLines = !drawBandLines; setLogBase(logBase, true); };
    
private:
    double fs;
//    std::vector<double> dLen;
	std::vector<std::vector<double>> gainDB;
    
    std::vector<std::complex<double>> linearData;
    std::vector<double> dBData;
	std::vector<double> RTData;
	std::vector<double> targetRT;
    
    std::unique_ptr<Label> RTLabel;
    std::unique_ptr<Label> gainLabel;

    OwnedArray<TextButton> buttons;
    std::vector<bool> drawToggles;
    
    bool unstable = false;
    double zeroDbHeight;

    std::vector<double> gridLineCoords;
    
    double logBase = Global::logBase;
    bool drawBandLines = true;
//    OwnedArray<Label> bandLabels;
    
    double plotWidth;
    double dLen;
    double multiplicationFactor = 1;
    
    bool init = true;
    
    std::unique_ptr<TextButton> logButton;
    std::unique_ptr<Slider> logBaseSlider;
    std::unique_ptr<Label> logLabel;
    
    Colour unstableColour {255, 128, 128};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Response)
};
