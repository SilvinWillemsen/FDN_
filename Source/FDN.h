/*
  ==============================================================================

    FDN.h
    Created: 13 Nov 2019 9:22:01am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Global.h"
#include <valarray>
#include <complex>
#include "../eigen/Eigen/Dense"
#include "../eigen/Eigen/src/Core/Transpose.h"
#include "EQComb.h"

//==============================================================================
/*
*/
class FDN    : public Component
{
public:
    FDN();
    ~FDN();

    void paint (Graphics&) override;
    void resized() override;
    
    float calculate (float input);
    void initialise (double sampleRate);
    
    std::vector<float> pareq (float G, float GB, float w0, float bw);//, float num[], float den[])
    void interactionMatrix (float* G, float gw, float* wg, float* wc, float* bw, std::vector<std::vector<float>>& leak);
    void aceq (std::vector<float> Gdb, std::vector<std::vector<float>>& numsopt, std::vector<std::vector<float>>& densopt);
    void getAttenuation (std::vector<float> RT, std::vector<int>& dLen, std::vector<std::vector<float>>& gainDB);
    
    EQComb* getEQComb (int idx) { return eqCombs[idx]; };
    
    // Debug print
    void debugPrint (std::vector<std::vector<float>>& vect);

private:
    OwnedArray<EQComb> eqCombs;
    std::vector<float> b;
    std::vector<float> c;
    float d = 0.5;
    
    float output;
    
    double fs;
    
    std::vector<std::vector<float>> A;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDN)
};
