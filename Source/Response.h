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
    Response (int minDLen, int maxDLen, double fs);
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
    void setIRData (std::vector<double>& irData);
//    void calculateTargetResponse (std::vector<double> gainDB, std::vector<double> RT);
    void linearGainToDB();

    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    void setLogBase (double val, bool init = false);
    void changeGrid() { drawBandLines = !drawBandLines; setLogBase(logBase, true); };
    
    void initialiseIRComb();
    bool isShowingIR() { return showingIR; };
    std::shared_ptr<EQComb>& getIRComb() { return IRComb; };
    
    void calculateIR();
    void setIRseconds (double seconds) { IRseconds = seconds; IRsamplesPerSecond = Global::IRplotDataPoints / seconds; };
private:
    double fs;
//    std::vector<double> dLen;
	std::vector<std::vector<double>> gainDB;
    
    std::vector<std::complex<double>> linearData;
    std::vector<double> dBData;
	std::vector<double> RTData;
	std::vector<double> targetRT;
    
    std::vector<double> IRdata;

    std::unique_ptr<Label> RTLabel;
    std::unique_ptr<Label> gainLabel;
    std::unique_ptr<Label> ampLabel;


    OwnedArray<TextButton> buttons;
    std::vector<bool> drawToggles;
    
    bool unstable = false;
    double zeroDbHeight;

    std::vector<double> gridLineCoords;
    
    double logBase = Global::logBase;
    bool drawBandLines = true;
//    OwnedArray<Label> bandLabels;
    
    double plotWidth;
    int minDLen, maxDLen;
    double multiplicationFactor = 1;
    
    std::unique_ptr<TextButton> logButton;
    std::unique_ptr<Slider> logBaseSlider;
    std::unique_ptr<Label> logLabel;
    
    std::unique_ptr<TextButton> IRButton;
    bool showingIR = Global::initShowIR;
    Colour unstableColour {255, 128, 128};
    
//    std::shared_ptr<EQComb> IRCombLong;
//    std::shared_ptr<EQComb> IRCombShort;
    std::shared_ptr<EQComb> IRComb;
    bool curIRIsLong = true;

    std::vector<double> noiseBurst;
    double IRseconds;
    double IRsamplesPerSecond;
    
    double maxSliderValue = 1.0; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Response)
};
