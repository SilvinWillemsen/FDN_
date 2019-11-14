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
    
    void setCoeffs (std::vector<float> coeffs);
    std::vector<float> getNums() { return nums; };
    std::vector<float> getDens() { return dens; };
    
    float filter (float x);
    
private:
    std::vector<float> nums;
    std::vector<float> dens;
    
    float yn = 0;
    float ynMin1 = 0;
    float ynMin2 = 0;
    
    float xn = 0;
    float xnMin1 = 0;
    float xnMin2 = 0;
    
    
//    float* yn;
//    float* ynMin1;
//    float* ynMin2;
//
//    float* xn;
//    float* xnMin1;
//    float* xnMin2;
    
    float* dummyPtrY;
    float* dummyPtrX;
    
    float output;
    
    std::vector<float> yVec;
    std::vector<float> xVec;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQFilter)
};
