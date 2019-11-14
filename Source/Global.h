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

    static const float RT = 10.0;
    
    static const std::vector<float> fc1 = { 31, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
    
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
}
