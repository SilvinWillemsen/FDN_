/*
  ==============================================================================

    Global.h
    Created: 13 Nov 2019 9:35:08am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

namespace Global
{
    static const int FDNorder = 16;
    static const int numOctaveBands = 10;
    static const int minDelayLength = 1500;
    static const int maxDelayLength = 4500;
    
    static const int numDesignFreqs = numOctaveBands * 2 - 1;

    static const double RT = 0.4;
    static const double RTmin = 0.2;
    static const double RTmax = 15.0;
    
    // Make this dynamic!! (using 125*2^(n-3))
    static const std::vector<double> fc1 = { 31, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
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
    
    // GUI
    static const int sliderHeight = 40;
}
