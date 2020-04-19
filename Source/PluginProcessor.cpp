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
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    fdn = std::make_shared<FDN>();
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
    recalculateMod = fs / static_cast<double> (Global::updatePerSecond);
    transportSource.prepareToPlay (samplesPerBlock, sampleRate);
    
    cpuStore.resize (Global::updatePerSecond + 2, 0);
}

void Fdn_AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    transportSource.releaseResources();
    
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
    curMillis = Time::getCurrentTime().toMilliseconds();
    
    if (fdn == nullptr)
        return;
    
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    AudioSourceChannelInfo bufferToFill (&buffer, 0, buffer.getNumSamples());
    transportSource.getNextAudioBlock (bufferToFill);
    if (transportSource.getCurrentPosition() >= 8)
    {
        transportSource.setPosition (0.0);
    }
    
    const float* input = bufferToFill.buffer->getReadPointer (0);
    float* const channeldataL = buffer.getWritePointer(0);
    float* const channeldataR = totalNumOutputChannels > 1 ? buffer.getWritePointer(1) : nullptr;
//    ScopedNoDenormals noDenormals;
//    const float* input = buffer.getReadPointer (0);
    if (fdn == nullptr)
        return;
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        totInput = (input[i] * 0.5 + (t == 0 ? 1.0 : 0.0)) * 0.5;
        output = fdn->calculate (totInput);
//        channeldataL[i] = input[i];
        channeldataL[i] = Global::limit ((totInput * 3.0 * dryGain + (1.0 - dryGain) * output) * 0.25, -1, 1);
        if (channeldataR != nullptr)
            channeldataR[i] = Global::limit ((totInput * 3.0 * dryGain + (1.0 - dryGain) * output) * 0.25, -1, 1);
        ++t;
        ++tt;
    }
    if (tt >= recalculateMod)
    {
        tt -= recalculateMod;
        if (!coeffsFixed)
            fdn->recalculateCoeffs();
    }

    if (zeroCoeffsFlag)
    {
        fdn->zeroCoefficients();
        zeroCoeffsFlag = false;
        t = 0;
    }

    if (changeMatTypeFlag)
    {
        changeMatTypeFlag = false;
        fdn->setScatteringMatrix (matType);
    }

    if (changeFDNorderFlag)
    {
        fdn->changeFDNorder (orderToChangeTo, matTypeToChangeTo);
        changeFDNorderFlag = !changeFDNorderFlag;
    }

    if (changeDelayLineSettingFlag)
    {
        fdn->changeDelayLineSetting (dLenSetToChangeTo, minDLenToSet, maxDLenToSet);
        changeDelayLineSettingFlag = !changeDelayLineSettingFlag;
    }

    cpuStore[timer % (Global::updatePerSecond + 2)] = 0.001 * (Time::getCurrentTime().toMilliseconds() - curMillis) * (fs / (buffer.getNumSamples()));
    ++timer;
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

void Fdn_AudioProcessor::changeFDNorder (int order, MatrixType matType)
{
    orderToChangeTo = order;
    matTypeToChangeTo = matType;
    changeFDNorderFlag = true;
}

void Fdn_AudioProcessor::changeDelayLineSetting (DelayLineSetting dLenSet, int min, int max)
{
    dLenSetToChangeTo = dLenSet;
    std::cout << "DLenSet: " << dLenSet << std::endl;
    minDLenToSet = min;
    maxDLenToSet = max;
    changeDelayLineSettingFlag = true;
}
