/*
  ==============================================================================

    SynthVoice.cpp
    Created: 27 May 2024 12:09:19pm
    Author:  yassine

  ==============================================================================
*/

#include "SynthVoice.h"
using namespace std;

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
  return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;

}
void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
  switch (waveId)
  {
  case 1:
      oscSine.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
      break;
  case 2:
      oscSaw.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
  case 3:
      oscSquare.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
  default:
      break;
  }
  
  adsr.noteOn();
}
void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
  adsr.noteOff();
  if (! allowTailOff || ! adsr.isActive())
  {
    clearCurrentNote();
  }
}
void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue)
{

}
void SynthVoice::pitchWheelMoved (int newPitchWheelValue)
{

}
void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
  //ADSR
  adsr.setSampleRate(sampleRate);

  
  //OSCILLATOR 
  juce::dsp::ProcessSpec spec;
  spec.maximumBlockSize = samplesPerBlock;
  spec.sampleRate = sampleRate;
  spec.numChannels = outputChannels;


  oscSine.prepare(spec);
  oscSaw.prepare(spec);
  oscSquare.prepare(spec);
  
  lfoOsc.prepare(spec);



}

// Public method to update adsr params
void SynthVoice::updateADSR(const float attack, const float decay, const float sustain, const float release)
{
  adsrParams.attack = attack;
  adsrParams.decay = decay;
  adsrParams.sustain = sustain;
  adsrParams.release = release;

  adsr.setParameters(adsrParams);
}

// Public method to update WaveTypes
void SynthVoice::updateWaveType(const int id)
{
  waveId = id;
}

// Public method to update ADSR activation
void SynthVoice::updateAdsrActivation(const bool activation)
{
  applyADSR = activation;
}

// Public Method to Update lfo parameters 
void SynthVoice::updateLFO(const float frequency, const float depth)
{
  lfoFreq = frequency;
  lfoDepth = depth;
}

void SynthVoice::renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples)
{

  synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
  synthBuffer.clear();

  //Oscillator
  juce::dsp::AudioBlock<float> audioBlock {synthBuffer} ;
  juce::dsp::AudioBlock<float> lfoAudioBlock {lfoBuffer} ;

  switch (waveId)
  {
  case 1:
      oscSine.process (juce::dsp::ProcessContextReplacing<float> (audioBlock));
      break;
  case 2:
      oscSaw.process (juce::dsp::ProcessContextReplacing<float> (audioBlock));
  case 3:
      oscSquare.process (juce::dsp::ProcessContextReplacing<float> (audioBlock));
  default:
      break;
  }
  

  //ADSR
  if (applyADSR){
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());
  }

  // LFO
  if (applyLFO)
  {
  lfoBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
  lfoBuffer.clear();
  lfoOsc.setFrequency(lfoFreq);
  lfoOsc.process(juce::dsp::ProcessContextReplacing<float> (lfoAudioBlock));
  //lfoBuffer.applyGain(lfoDepth);
  
  }

  // Add synth buffer to output Buffer
  for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel){
    
    if (applyLFO)
    {
      for (int sample=0; sample<synthBuffer.getNumSamples(); sample++)
      { 
        auto tmpLFO = lfoBuffer.getSample(channel, sample);
        auto tmpSynth = synthBuffer.getSample(channel, sample);
        synthBuffer.setSample(channel, sample, (1-lfoDepth)*tmpSynth+lfoDepth*tmpLFO*tmpSynth); 
      }
    }
    outputBuffer.addFrom(channel, startSample, synthBuffer, channel,0, numSamples);
    
    if (! adsr.isActive())
    {
      clearCurrentNote();
    }
  }

  lfoBuffer.clear();



}