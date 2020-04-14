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
    AudioProcessorEditor* createEditor() override;
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

    std::shared_ptr<FDN> getFDN() { return fdn; };    
    void setZeroCoeffsFlag() { zeroCoeffsFlag = true; };
    
    void setDryGain (double val) { dryGain = val; };
    void setInputGain (double val) { inputGain = val; };

    double getFs() { return fs; };
    void changeMatType (MatrixType mt) { changeMatTypeFlag = true; matType = mt; };
    
    void changeFDNorder (int order, MatrixType matType);
    
    void fixCoefficients (bool setFixed) { coeffsFixed = setFixed; }
    
    double getCPU() {
        double tmp = 0;
        for (int i = 0; i < cpuStore.size(); ++i)
            tmp += cpuStore[i];
        
        return tmp / cpuStore.size(); };
private:
    //==============================================================================
    std::shared_ptr<FDN> fdn;
    
    double fs;
    float totInput;
    float output;
    double inputGain = Global::initInputGain;
	
    bool init = false;
    unsigned long t = !Global::initWithImpulse;
    unsigned long tt = 0;
    Random rand;
    
    bool zeroCoeffsFlag = false;
    int recalculateMod;
    double dryGain = Global::dryWetInit;
    
    bool changeMatTypeFlag = false;
    MatrixType matType = Global::initMatType;
    
    bool changeFDNorderFlag = false;
    int orderToChangeTo = 0;
    MatrixType matTypeToChangeTo;

    bool coeffsFixed = false;
    
    double cpu = 1;
    
    long curMillis;
    int timer = 0;
    
    std::vector<double> cpuStore;
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Fdn_AudioProcessor)
};
