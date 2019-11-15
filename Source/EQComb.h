/*
  ==============================================================================

    EQComb.h
    Created: 13 Nov 2019 9:22:41am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "EQFilter.h"

//==============================================================================
/*
*/

class EQComb    : public Component
{
public:
    EQComb (int delayLength);
    ~EQComb();

    void paint (Graphics&) override;
    void resized() override;

    void setFilter (int i, std::vector<double> coeffs) { eqFilters[i]->setCoeffs (coeffs); };
    
    float filter (float x);
    
    void increment() { readLoc = (readLoc + 1) % delayLine.size(); writeLoc = (writeLoc + 1) % delayLine.size(); };
    
    EQFilter* getFilter (int idx) { return eqFilters[idx]; };
    int getDelayLineLength() { return static_cast<int> (delayLine.size()); };
    
    void debug() { debugFlag = true; };
    
    void addScatOutput (float val) { delayLine[writeLoc] += val; };
    void zeroWritePointer() { delayLine[writeLoc] = 0; };
    
    void zeroCoefficients();
    
    std::vector<double> getCoefficientsOfFilter (int filterIdx) { return eqFilters[filterIdx]->getCoefficients(); }
private:
    
    OwnedArray<EQFilter> eqFilters;
    std::vector<double> delayLine;
    
    int readLoc = 1;
    int writeLoc = 0;
    
    bool debugFlag = false;
    
    double y;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQComb)
};
