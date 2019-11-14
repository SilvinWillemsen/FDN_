/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Global.h"
//==============================================================================
Fdn_AudioProcessorEditor::Fdn_AudioProcessorEditor (Fdn_AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (int i = 0; i < Global::FDNorder; ++i)
    {
        sliders.add (new Slider ("RT" + String(Global::fc1[i])));
    }
    setSize (400, 800);
}

Fdn_AudioProcessorEditor::~Fdn_AudioProcessorEditor()
{
}

//==============================================================================
void Fdn_AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void Fdn_AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
