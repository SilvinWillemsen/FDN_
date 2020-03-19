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
    
    double calculate (double input);
    void initialise (double sampleRate);
    void recalculateCoeffs (bool init = false);
    
    std::vector<double> pareq (double G, double GB, double w0, double bw);//, double num[], double den[])
    void interactionMatrix (double* G, double gw, double* wg, double* wc, double* bw);
    void aceq (int idx);
    void getAttenuation();
	void getDelayLines();
    
    EQComb* getEQComb (int idx) { return eqCombs[idx]; };
    
    // Debug print
    void debugPrint (std::vector<std::vector<double>>& vect);
    
    // Set the reverb time
    void setRT (int i, double val) { RT[i] = val; };

    // makes sure that the states of the filters and the delay lines are reset
    void zeroCoefficients() { for (auto eqComb : eqCombs) { eqComb->zeroCoefficients(); }};
    
    std::vector<double> getCoefficients (int combIdx, int filterIdx) // filterIdx should be all coefficients
    {
        return eqCombs[combIdx]->getCoefficientsOfFilter (filterIdx);
    }
    
    double getAvgDLen();
    double getMinDLen();
    double getMaxDLen();

private:
    OwnedArray<EQComb> eqCombs;
    std::vector<double> b;
    std::vector<double> c;
    double d = 0.5;
    
    double output;
	
    double fs;
    
    std::vector<std::vector<double>> A;
    std::vector<double> RT; // reverbTime
    
    // variables pre-initialised
    std::vector<std::vector<double>> gainDB;
    std::vector<std::vector<double>> numsOpt;
    std::vector<std::vector<double>> densOpt;
    
    std::vector<double> dLen;
    
    // Center frequencies
    std::vector<double> fc2 = { 31.25, 44.2, 62.5, 88.39, 125, 176.78, 250, 353.55, 500, 707.11, 1000, 1414.2, 2000, 2828.4, 4000, 5656.9, 8000, 11313.7, 16000 };
    
    std::vector<double> Goptdb;
    std::vector<double> Gopt;
    std::vector<double> G2optdb;
    std::vector<double> G2opt;
    std::vector<double> G2woptdb;
    std::vector<double> G2wopt;
    
    std::vector<std::vector<double>> leak;
    std::vector<std::vector<double>> leak2;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDN)
};
