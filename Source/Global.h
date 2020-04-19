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

enum DelayLineSetting
{
    randomDLen = 1,
    gaussian,
    primes,
    uniform,
    randomDLenPredef,
    gaussianPredef,
    primesPredef,
    uniformPredef,
};

enum TransportState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

namespace Global
{
    static const int initFDNorder = 16;
    static const int numOctaveBands = 10;
    static const int minDelayLength = 1500;
    static const int maxDelayLength = 4500;
    static const int minRange = 500;
    static const int maxRange = 10000;
    static const int minRangeDiff = 100;
    
    static const int numDesignFreqs = numOctaveBands * 2 - 1;
    
    static const bool initWithImpulse = false;

    static const double RT = 1.0;
    static const double RTmin = 0.03;
    static const double RTmax = 15.0;
    
    static const double dryWetInit = 0.7;
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
    
    static const int updatePerSecond = 5;
    
    static const MatrixType initMatType = hadamard;
    
    static const std::vector<std::vector<double>> presetValues
    {
        {0.98, 0.72, 0.51, 0.39, 0.34, 0.36, 0.25, 0.27, 0.2, 0.1},     // smallRoom
        {1.62, 0.81, 0.76, 0.73, 0.63, 0.48, 0.41, 0.36, 0.27, 0.19},   // mediumRoom
        {1.2, 0.95, 0.71, 0.78, 0.85, 0.88, 0.87, 0.87, 0.62, 0.39},    // largeRoom
        {3.55, 2.28, 2.18, 2.05, 1.89, 1.86, 1.69, 1.28, 0.9, 0.52},    // concertHall
        {5.68, 4.41, 3.86, 2.86, 2.59, 2.61, 2.47, 2.02, 1.32, 0.92}    // church
    };
    
    static const DelayLineSetting initDelayLineSetting = randomDLenPredef;
    static const bool initPreDefOn = static_cast<int> (initDelayLineSetting) > 4 ? true : false;

    // randomly distributed delay lengths
    static const std::vector<int> randomDLens
    {
        3673, 2237, 2234, 2903, 1672, 3104, 3236, 1832,
        4075, 3711, 3138, 1763, 3773, 2223, 4150, 2018,
        1651, 2033, 3995, 4434, 3106, 4346, 2183, 2865,
        4097, 4022, 1548, 4346, 3280, 3321, 3704, 3134,
        2010, 1699, 3021, 3502, 4466, 2042, 4138, 2209,
        1932, 1617, 3440, 3652, 2683, 2324, 3196, 1779,
        3183, 4145, 2390, 2887, 3618, 1789, 2257, 2867,
        3431, 1972, 3453, 3206, 2920, 4129, 3721, 3114
    };
    
    // gaussian-distributed delay lengths
    static const std::vector<int> gaussianDLens
    {
        3106, 2760, 3203, 3126, 3367, 2942, 3284, 2510,
        2532, 2520, 3231, 3318, 2134, 3031, 3470, 2462,
        2789, 3034, 1999, 3732, 1964, 4143, 1947, 3066,
        2607, 2349, 4013, 3271, 2903, 3122, 2619, 1867,
        2431, 3475, 2593, 3728, 3165, 3391, 2895, 3049,
        2754, 3647, 2644, 2176, 3015, 3397, 4424, 3074,
        3012, 2815, 3138, 3258, 2447, 3621, 4827, 3343,
        3427, 3152, 2456, 2645, 3132, 2504, 4017, 2098
    };
//    static const std::vector<int> dLens // very freaky sound
//    {
//        676, 1780, 2217, 628, 2196, 656, 678, 1260, 1940, 1114, 937, 1114, 608, 2046, 874, 654
//    };
    
    // prime delay lengths
    static const std::vector<int> primeDLens
    {
        2927, 2593, 2273, 3697, 1877, 3877, 2477, 3461,
        1609, 3779, 3541, 4259, 1669, 3539, 3637, 4013,
        3121, 4003, 1627, 3733, 3511, 4093, 2411, 3011,
        2801, 2017, 4013, 1621, 3023, 2161, 4201, 3079,
        1783, 4027, 1613, 1907, 2129, 3797, 2543, 1579,
        3967, 2551, 3833, 4111, 3059, 4159, 1753, 1601,
        2423, 1993, 4493, 2707, 3719, 3547, 3769, 4219,
        2389, 2087, 1709, 3229, 3083, 3391, 3259, 2791
    };
    
    // uniformly distributed delay lengths
    static const std::vector<int> uniformDLens2
    {
        1562, 4294
    };
    
    static const std::vector<int> uniformDLens4
    {
        1670, 2835, 3030, 3827
    };
    
    static const std::vector<int> uniformDLens8
    {
        1688, 2048, 2300, 2866, 3037, 3579, 3911, 4146
    };
    
    static const std::vector<int> uniformDLens16
    {
        1607, 1834, 1906, 2165, 2385, 2502, 2749, 2943,
        3147, 3226, 3389, 3638, 3844, 3941, 4235, 4347
    };
    
    static const std::vector<int> uniformDLens32
    {
        1573, 1612, 1737, 1846, 1917, 1992, 2142, 2169,
        2332, 2389, 2442, 2537, 2664, 2777, 2818, 2977,
        3015, 3121, 3226, 3320, 3443, 3471, 3576, 3693,
        3823, 3935, 3941, 4083, 4128, 4281, 4378, 4410
    };
    
    static const std::vector<int> uniformDLens64
    {
        1513, 1585, 1594, 1681, 1714, 1780, 1790, 1833,
        1917, 1949, 2014, 2055, 2076, 2119, 2195, 2214,
        2258, 2304, 2377, 2405, 2457, 2506, 2554, 2602,
        2667, 2708, 2754, 2795, 2849, 2883, 2908, 2986,
        3018, 3085, 3131, 3167, 3208, 3280, 3327, 3335,
        3407, 3444, 3481, 3533, 3589, 3637, 3665, 3717,
        3774, 3832, 3877, 3906, 3964, 3987, 4032, 4086,
        4147, 4181, 4241, 4299, 4337, 4369, 4406, 4475
    };
    
    // GUI
    static const bool horSliders = false;
    static const bool useAdvancedWindow = true;
    static const int margin = 12;
    
    static const double logBase = 1000;
    static const int axisMargin = 40;
    static const double zeroDbRatio = 0.6;
    static const int fftOrder = 1024;

    static const bool showRTGainButtons = false;
    static const bool showLogSlider = false;
    
    static const bool initShowIR = false;
    
    static const double IRplotDataPoints = 1000;
    
}
