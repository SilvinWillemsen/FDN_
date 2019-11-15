/*
  ==============================================================================

    EQFilters.h
    Created: 13 Nov 2019 9:23:06am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class EQFilter    : public Component
{
public:
    EQFilter();
    ~EQFilter();

    void paint (Graphics&) override;
    void resized() override;
    
    void setCoeffs (std::vector<double> coeffs);
    std::vector<double> getNums() { return nums; };
    std::vector<double> getDens() { return dens; };
    
    double filter (double x);
    
    void zeroCoefficients() { yn = 0; ynMin1 = 0; ynMin2 = 0; xn = 0; xnMin1 = 0; xnMin2 = 0; };
    
    std::vector<double> getCoefficients() {
        return std::vector<double> {nums[0], nums[1], nums[2], dens[0], dens[1], dens[2]};
    };
    
private:
    std::vector<double> nums;
    std::vector<double> dens;
    
    double yn = 0;
    double ynMin1 = 0;
    double ynMin2 = 0;
    
    double xn = 0;
    double xnMin1 = 0;
    double xnMin2 = 0;
    
    
//    float* yn;
//    float* ynMin1;
//    float* ynMin2;
//
//    float* xn;
//    float* xnMin1;
//    float* xnMin2;
//
//    float* dummyPtrY;
//    float* dummyPtrX;
    
//    float output;
    
//    std::vector<float> yVec;
//    std::vector<float> xVec;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQFilter)
};
