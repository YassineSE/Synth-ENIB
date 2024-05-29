/*
  ==============================================================================

    SynthVoice.h
    Created: 27 May 2024 12:09:19pm
    Author:  yassine

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
  bool canPlaySound (juce::SynthesiserSound* sound) override;
  void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
  void stopNote (float velocity, bool allowTailOff) override;
  void controllerMoved (int controllerNumber, int newControllerValue) override;
  void pitchWheelMoved (int newPitchWheelValue) override;
  void prepareToPlay( double sampleRate, int samplesPerBlock, int outputChannels);
  void renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples) override;
  void updateADSR(const float attack, const float decay, const float sustain, const float release);
  void updateWaveType(const int id);
  void updateAdsrActivation(const bool activation);
  void updateLFO(const float frequency, const float depth);
  bool applyADSR = true;
  bool applyLFO = true;
  int waveId;
  float gainValue;
  float lfoFreq;
  float lfoDepth;



private:

  juce::AudioBuffer<float> synthBuffer;
  juce::AudioBuffer<float> lfoBuffer;
  
    //ADSR
  juce::ADSR adsr;
  juce::ADSR::Parameters adsrParams;
  
  //OSCILLATOR
  juce::dsp::Oscillator<float> osc { [](float x) { return std::sin(x); } };
  juce::dsp::Oscillator<float> oscSine {[] (float x) { return std::sin (x); }, 200} ;
  juce::dsp::Oscillator<float> oscSaw {[] (float x){ return x / juce::MathConstants<float>::pi; }, 200};
  juce::dsp::Oscillator<float> oscSquare {[](float x) { return x < 0.0f ? -1.0f : 1.0f; }, 200 };
  
  std::function<float(float)> lfoSine = [&](float x) { return std::sin(x); };
  juce::dsp::Oscillator<float> lfoOsc{lfoSine, 200};

  bool isPrepared { false};
};