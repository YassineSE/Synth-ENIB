/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JucePluginDefines.h>
#include "SynthAudioSource.h"
//==============================================================================
SynthENIBAudioProcessor::SynthENIBAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                    ), apvts(*this, nullptr, "Parameters", createParameters()), synthAudioSource(keyboardState), filter(), waveViewer(1)
#endif
{
    waveViewer.setRepaintRate(35);
    waveViewer.setBufferSize(256);
}

SynthENIBAudioProcessor::~SynthENIBAudioProcessor()
{
}

//==============================================================================
const juce::String SynthENIBAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SynthENIBAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SynthENIBAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SynthENIBAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SynthENIBAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SynthENIBAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SynthENIBAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SynthENIBAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SynthENIBAudioProcessor::getProgramName (int index)
{
    return {};
}

void SynthENIBAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SynthENIBAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    
    synthAudioSource.prepareToPlay (samplesPerBlock, sampleRate);
    
    filter.setSamplingRate(static_cast<float>(sampleRate));
}

void SynthENIBAudioProcessor::releaseResources()
{
    synthAudioSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SynthENIBAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void SynthENIBAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    
    // ADSR PARAMS
    auto& attack = *apvts.getRawParameterValue("ATTACK");
    auto& decay = *apvts.getRawParameterValue("DECAY");
    auto& sustain = *apvts.getRawParameterValue("SUSTAIN");
    auto& release = *apvts.getRawParameterValue("RELEASE");
    // GAIN
    auto& gainProcessor = *apvts.getRawParameterValue("GAIN");
    // LFO
    auto& lfoFreq = *apvts.getRawParameterValue("LFOFREQ");
    auto& lfoDepth = *apvts.getRawParameterValue("LFODEPTH");
    
    // Filter
    auto& cutoffFrequencyParameter = *apvts.getRawParameterValue("cutoff_frequency");
    auto& highpassParameter = *apvts.getRawParameterValue("highpass");
    
    
    
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    juce::AudioSourceChannelInfo bufferToFill(&buffer, 0, buffer.getNumSamples());
    

    synthAudioSource.attack = attack.load();
    synthAudioSource.decay = decay.load();
    synthAudioSource.sustain = sustain.load();
    synthAudioSource.release = release.load();
    synthAudioSource.audioSourceLfoFreq = lfoFreq.load();
    synthAudioSource.audioSourceLfoDepth = lfoDepth.load();


    synthAudioSource.getNextAudioBlock(bufferToFill);

    
    // retrieve and set the parameter values
    const auto cutoffFrequency = cutoffFrequencyParameter.load();
    // in C++, std::atomic<T> to T conversion is equivalent to a load
    const auto highpass = highpassParameter.load() < 0.5f ? false : true;
    filter.setCutoffFrequency(cutoffFrequency);
    filter.setHighPass(highpass);

    // perform the filtering
    filter.processBlock(buffer, midiMessages);


    // Apply gain to buffer
    buffer.applyGain(gainProcessor);

    windowBuffer.makeCopyOf(buffer);
    windowBuffer.applyGain(4.0f);
    waveViewer.pushBuffer(windowBuffer);
    
    
    
    
}

//==============================================================================
bool SynthENIBAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SynthENIBAudioProcessor::createEditor()
{
    return new SynthENIBAudioProcessorEditor (*this);
}

//==============================================================================
void SynthENIBAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SynthENIBAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthENIBAudioProcessor();
}

// Value Tree
juce::AudioProcessorValueTreeState::ParameterLayout SynthENIBAudioProcessor::createParameters(){
    
    // Comobobox: OSC PARAMS
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>("OSC", "Oscillator", juce::StringArray {"Sine", "Saw", "Square"}, 0));


    // Global Gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", juce::NormalisableRange<float>(0.0f, 0.1f, 0.01f, 0.48f), 0.1f));
    
    //Attack
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK", "Attack", juce::NormalisableRange<float> {0.1f, 1.0f, 0.1f}, 0.1f));
    //Decay
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", juce::NormalisableRange<float> {0.1f, 1.0f, 0.1f}, 0.1f));
    //Sustain
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain", juce::NormalisableRange<float> {0.1f, 1.0f, 0.1f}, 1.0f));
    //Release
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release", juce::NormalisableRange<float> {0.1f, 3.0f, 0.1f}, 0.4f));

    //LFO FREQUENCY
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFOFREQ", "LfoFreq", juce::NormalisableRange<float> {0.1f, 20.0f, 0.1f}, 3.0f));
    //LFO DEPTH
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LFODEPTH", "LfoDepth", juce::NormalisableRange<float> {0.0f, 1.0f, 0.1f}, 0.0f));
    

    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff_frequency","Cutoff Frequency",juce::NormalisableRange{20.f,10000.f,0.1f,0.2f,false},500.f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("highpass","Highpass",false));


    return {params.begin(), params.end()};
}