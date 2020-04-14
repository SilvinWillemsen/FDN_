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

    nums.resize (3, 0);
    dens.resize (3, 0);
}

EQFilter::~EQFilter()
{
}

void EQFilter::paint (Graphics& g)
{
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

void EQFilter::setCoeffs (std::vector<double> coeffsToSet)
{
    if (coeffsToSet.size() != 6)
    {
        std::cout << "Nope" << std::endl;
        return;
    } else {
        for (int i = 0; i < 3; ++i)
        {
            nums[i] = coeffsToSet[i];
            dens[i] = coeffsToSet[i + 3];
        }
    }
}

double EQFilter::filter (double x)
{
    xn = x;
    yn = nums[0] * xn + nums[1] * xnMin1 + nums[2] * xnMin2 - dens[1] * ynMin1 - dens[2] * ynMin2;

    ynMin2 = ynMin1;
    ynMin1 = yn;

    xnMin2 = xnMin1;
    xnMin1 = xn;
    
    return yn;
    
}
