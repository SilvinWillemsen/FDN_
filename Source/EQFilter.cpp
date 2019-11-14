/*
  ==============================================================================

    EQFilter.cpp
    Created: 13 Nov 2019 9:23:06am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "EQFilter.h"

//==============================================================================
EQFilter::EQFilter()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    nums.resize (3, 0);
    dens.resize (3, 0);
    yVec.resize (3, 0);
    xVec.resize (3, 0);
    
    // initialise output vector
//    yVec.resize (3, 0);
//    yn = &yVec[0];
//    ynMin1 = &yVec[1];
//    ynMin2 = &yVec[2];
    
    // initialise input vector
//    xVec.resize (3, 0);
//    xn = &xVec[0];
//    xnMin1 = &xVec[1];
//    xnMin2 = &xVec[2];
}

EQFilter::~EQFilter()
{
}

void EQFilter::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("EQFilters", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void EQFilter::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void EQFilter::setCoeffs (std::vector<float> coeffsToSet)
{
    if (coeffsToSet.size() != 6)
    {
        std::cout << "WTF" << std::endl;
        return;
    } else {
        for (int i = 0; i < 3; ++i)
        {
            nums[i] = coeffsToSet[i];
            dens[i] = coeffsToSet[i + 3];
        }
    }
}

float EQFilter::filter (float x)
{
    xn = x;
    yn = nums[0] * xn + nums[1] * xnMin1 + nums[2] * xnMin2 - dens[1] * ynMin1 - dens[2] * ynMin2;

    ynMin2 = ynMin1;
    ynMin1 = yn;

    xnMin2 = xnMin1;
    xnMin1 = xn;
    
    return yn;
    
}

//float EQFilter::filter (float x)
//{
//    if (x != 0)
//        std::cout << "nonzero" << std::endl;
//
//    xn[0] = x;
//    yn[0] = nums[0] * xn[0] + nums[1] * xnMin1[0] + nums[2] * xnMin2[0]
//        + dens[1] * ynMin1[0] + dens[2] * ynMin2[0];
//
//    output = yn[0];
//    if (isnan (output))
//        std::cout << "wait" << std::endl;
//
//    dummyPtrY = ynMin2;
//    ynMin2 = ynMin1;
//    ynMin1 = yn;
//    yn = dummyPtrY;
//
//    dummyPtrX = xnMin2;
//    xnMin2 = xnMin1;
//    xnMin1 = xn;
//    xn = dummyPtrX;
//
//    return output;
//
//}
