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
    hadamard = 1,
    householder,
	identity,
    randomMat
};

enum Preset
{
    noPreset = 1,
    smallRoom,
    mediumRoom,
    largeRoom,
    concertHall,
    church
};

namespace Global
{
    static const int initFDNorder = 8;
    static const int numOctaveBands = 10;
    static const int minDelayLength = 1500;
    static const int maxDelayLength = 4500;
    
    static const int numDesignFreqs = numOctaveBands * 2 - 1;
    
    static const bool initWithImpulse = false;

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
    
    static const MatrixType initMatType = randomMat;
    
    static const std::vector<std::vector<double>> presetValues
    {
        {0.98, 0.72, 0.51, 0.39, 0.34, 0.36, 0.25, 0.27, 0.2, 0.1},     // smallRoom
        {1.62, 0.81, 0.76, 0.73, 0.63, 0.48, 0.41, 0.36, 0.27, 0.19},   // mediumRoom
        {1.2, 0.95, 0.71, 0.78, 0.85, 0.88, 0.87, 0.87, 0.62, 0.39},    // largeRoom
        {3.55, 2.28, 2.18, 2.05, 1.89, 1.86, 1.69, 1.28, 0.9, 0.52},    // concertHall
        {5.68, 4.41, 3.86, 2.86, 2.59, 2.61, 2.47, 2.02, 1.32, 0.92}    // church
    };
    
    static const bool usePredefinedDLens = false;
    static const std::vector<int> dLens
    {
        4466, 1548, 3673, 2237, 2234, 2903, 1672, 3104,
        4075, 3711, 3138, 1763, 3773, 2223, 4150, 2018,
        1651, 2033, 3995, 4434, 3106, 4346, 2183, 2865,
        4097, 4022, 4346, 3280, 3321, 3704, 3134, 1832,
        2010, 1699, 3021, 3502, 2042, 4138, 2209, 3236,
        1932, 1617, 3440, 3652, 2683, 2324, 3196, 1779,
        3183, 4145, 2390, 2887, 3618, 1789, 2257, 2867,
        3431, 1972, 3453, 3206, 2920, 4129, 3721, 3114
    };

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
    
    static const double IRplotDataPoints = 1000;
    
}
