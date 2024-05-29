/*
  ==============================================================================

    SynthAudioSource.h
    Created: 27 May 2024 12:09:28pm
    Author:  yassine

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"
#include "SynthVoice.h"
//#include "PluginProcessor.h"

class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource (juce::MidiKeyboardState& keyState);

    void setUsingSineWaveSound();
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
  
    juce::Synthesiser synth;
    float attack, decay, sustain, release;
    int audioSourceWaveId;
    bool audioSourceApplyADSR;
    float audioSourceLfoFreq;
    float audioSourceLfoDepth;

private:

    
    juce::MidiKeyboardState& keyboardState;
};
