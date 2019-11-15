/*
  ==============================================================================

    EQComb.cpp
    Created: 13 Nov 2019 9:22:41am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Global.h"
#include "EQComb.h"

//==============================================================================
EQComb::EQComb (int dLen)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    delayLine.resize (dLen, 0);
    
    for (int i = 0; i < Global::numOctaveBands + 1; ++i)
    {
        eqFilters.add (new EQFilter ());
    }
}

EQComb::~EQComb()
{
}

void EQComb::paint (Graphics& g)
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
    g.drawText ("EQComb", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void EQComb::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


float EQComb::filter (float x)
{
    delayLine[writeLoc] += x;
    
    y = delayLine[readLoc];
    float out;
//    if (y != 0)
//        std::cout << "one moment" << std::endl;
    for (int i = 0; i < Global::numOctaveBands + 1; ++i)
    {
        y = eqFilters[i]->filter (y);
        
//        if (debugFlag)
//            std::cout << "debug" << std::endl;
//        out = eqFilters[i]->filter (y);
//
//        y = out;
    }
    return y;
}

void EQComb::zeroCoefficients()
{
    for (auto filter : eqFilters)
        filter->zeroCoefficients();
    
    for (int i = 0; i < delayLine.size(); ++i)
        delayLine[i] = 0;
}
