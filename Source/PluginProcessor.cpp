/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
//#include "PluginProcessor.h"

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Fdn_AudioProcessor::Fdn_AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::mono(), true)
                     #endif
                       )
#endif
{
    fdn = std::make_shared<FDN> ();
    
//    addParameter (dryGain = new AudioParameterFloat("dryGain", "Dry Gain", 0.0f, 1.0f, 0.55f));
//    addParameter (RT31 = new AudioParameterFloat("RT31", "T60 for 31.5 Hz", 0.2f, 10.0f, 1.0f));
//    addParameter (RT63 = new AudioParameterFloat("RT63", "T60 for 63 Hz", 0.2f, 10.0f, 1.0f));
//    addParameter (RT125 = new AudioParameterFloat("RT125", "T60 for 125 Hz", 0.2f, 10.0f, 1.0f));
//    addParameter (RT250 = new AudioParameterFloat("RT250", "T60 for 250 Hz", 0.2f, 10.0f, 1.0f));
//    addParameter (RT500 = new AudioParameterFloat("RT500", "T60 for 500 Hz", 0.2f, 10.0f, 1.0f));
//    addParameter (RT1000 = new AudioParameterFloat("RT1000", "T60 for 1000 Hz", 0.2f, 10.0f, 1.0f));
//    addParameter (RT2000 = new AudioParameterFloat("RT2000", "T60 for 2000 Hz", 0.2f, 10.0f, 1.0f));
//    addParameter (RT4000 = new AudioParameterFloat("RT4000", "T60 for 4000 Hz", 0.2f, 10.0f, 1.0f));
//    addParameter (RT8000 = new AudioParameterFloat("RT8000", "T60 for 8000 Hz", 0.2f, 10.0f, 1.0f));
//    addParameter (RT16000 = new AudioParameterFloat("RT16000", "T60 for 16000 Hz", 0.2f, 10.0f, 1.0f));
}

Fdn_AudioProcessor::~Fdn_AudioProcessor()
{
}

//==============================================================================
const String Fdn_AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Fdn_AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Fdn_AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Fdn_AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Fdn_AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Fdn_AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Fdn_AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Fdn_AudioProcessor::setCurrentProgram (int index)
{
}

const String Fdn_AudioProcessor::getProgramName (int index)
{
    return {};
}

void Fdn_AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Fdn_AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    fs = sampleRate;
    fdn->initialise (fs);
}

void Fdn_AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Fdn_AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Fdn_AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    float* const channeldataL = buffer.getWritePointer(0);
    float* const channeldataR = getTotalNumInputChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
    ScopedNoDenormals noDenormals;
    const float* input = buffer.getReadPointer (0);
    
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        if (t == 0)
        {
            init = true;
        }
        totInput = (input[i] + (init ? 1.0 : 0.0)) * inputGain;
    
        output = fdn->calculate (totInput);
        init = false;
        channeldataL[i] = Global::limit (totInput * dryGain + (1.0 - dryGain) * output, -1, 1);
        if (channeldataR != nullptr)
            channeldataR[i] = Global::limit (totInput * dryGain + (1.0 - dryGain) * output, -1, 1);
//        std::cout << channelData[i] << std::endl;
        ++t;
//        if (t % fdn->getEQComb(0)->getDelayLineLength() == 0)
//        {
//            std::cout << "wait" << std::endl;
//        }
    }
    fdn->recalculateCoeffs();
    if (recalculateCoeffsFlag == true)
    {
        fdn->zeroCoefficients();
        recalculateCoeffsFlag = false;
        t = 0;
    }

}

//AudioProcessorEditor* Fdn_AudioProcessor::createEditor()
//{
//    return new Fdn_AudioProcessorEditor (*this);
//}

//==============================================================================
bool Fdn_AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Fdn_AudioProcessor::createEditor()
{
    return new Fdn_AudioProcessorEditor (*this);
}

//==============================================================================
void Fdn_AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Fdn_AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Fdn_AudioProcessor();
}
