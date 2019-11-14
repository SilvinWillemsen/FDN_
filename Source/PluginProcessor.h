/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FDN.h"

using namespace std;
//typedef complex<double> dcomp;
//==============================================================================
//class Aceq : public 

//==============================================================================
class Fdn_AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
	Fdn_AudioProcessor();
		
	/*{
		addParameter(dryGain = new AudioParameterFloat("dryGain", "Dry Gain", 0.0f, 1.0f, 0.5f));
	
	}*/
    ~Fdn_AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

	void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
	//{
		//buffer.applyGain(*dryGain);
	//};


    //==============================================================================
    AudioProcessorEditor* createEditor() override { return new GenericAudioProcessorEditor(*this); }
	bool hasEditor() const override; //{ return true; };

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    std::unique_ptr<FDN> fdn;
    
    AudioParameterFloat* dryGain;
	AudioParameterFloat* RT31;
	AudioParameterFloat* RT63;
	AudioParameterFloat* RT125;
	AudioParameterFloat* RT250;
	AudioParameterFloat* RT500;
	AudioParameterFloat* RT1000;
	AudioParameterFloat* RT2000;
	AudioParameterFloat* RT4000;
	AudioParameterFloat* RT8000;
	AudioParameterFloat* RT16000;
    
    double fs;
    float output;
	
    bool init = false;
    unsigned long t = 0;
    Random rand;

	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Fdn_AudioProcessor)
};
