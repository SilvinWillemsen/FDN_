/*
  ==============================================================================

    Global.h
    Created: 13 Nov 2019 9:35:08am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once
enum MatrixType
{
    hadamard,
    householder
};

namespace Global
{
    static const int FDNorder = 16;
    static const int numOctaveBands = 10;
    static const int minDelayLength = 1500;
    static const int maxDelayLength = 4500;
    
    static const int numDesignFreqs = numOctaveBands * 2 - 1;

    static const double RT = 1.0;
    static const double RTmin = 0.03;
    static const double RTmax = 15.0;
    
    static const double dryWetInit = 0.45;
    static const double initInputGain = 0.5;
    
    // Make this dynamic!! (using 125*2^(n-3))
    static const std::vector<double> fc1 = { 31.25, 62.5, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
    static const double sliderCoeff = 0.6;
    
    static double limit (double val, double min, double max)
    {
        if (val < min)
        {
            val = min;
            return val;
        }
        else if (val > max)
        {
            val = max;
            return val;
        }
        return val;
    }
    
    static const double updatePerSecondRatio = 0.2;
    
    static const MatrixType initMatType = hadamard;
    // GUI
    static const bool horSliders = false;
//    static const int sliderHeight = 40;
    static const bool sliderDependency = false;
    static const double logBase = 1000;
    static const int axisMargin = 40;
    static const double zeroDbRatio = 0.6;
    static const int fftOrder = 1024;


    static const bool showRTGainButtons = false;
    static const bool showLogSlider = false;
    
    static const bool initShowIR = false;
    
//    static const double IRseconds = 5.5;
//    static const double IRsamplesPerSecond = 200;
    static const double IRplotDataPoints = 1000;
    
}
