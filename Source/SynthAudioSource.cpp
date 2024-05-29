/*
  ==============================================================================

    SynthAudioSource.cpp
    Created: 27 May 2024 12:09:28pm
    Author:  yassine

  ==============================================================================
*/

#include "SynthAudioSource.h"

SynthAudioSource::SynthAudioSource (juce::MidiKeyboardState& keyState)
    : keyboardState (keyState)
{   

    // This is what makes the synthesiser Polyphonic
    // We add multiples voices, each voice has its oscillator

    for (auto i = 0; i < 4; ++i)                // [1]
            synth.addVoice (new SynthVoice());
    synth.addSound (new SynthSound());       // [2]
}

void SynthAudioSource::setUsingSineWaveSound()
{
    synth.clearSounds();
}

void SynthAudioSource::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{   
    synth.setCurrentPlaybackSampleRate (sampleRate); // [3]
    
    for (int i=0; i<synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))){
            voice->prepareToPlay(sampleRate, samplesPerBlockExpected, 2);
        }
    }

    

}

void SynthAudioSource::releaseResources()
{
}

void SynthAudioSource::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    juce::MidiBuffer incomingMidi;
    keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample,
                                         bufferToFill.numSamples, true);       // [4]
    

    //ADSR CHANGES
    for (int i=0; i < synth.getNumVoices(); i++ )
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {   
            voice->updateWaveType(audioSourceWaveId);
            voice->updateAdsrActivation(audioSourceApplyADSR);
            voice->updateADSR(attack, decay, sustain, release);
            voice->updateLFO(audioSourceLfoFreq, audioSourceLfoDepth);
            
        }
    }
    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                           bufferToFill.startSample, bufferToFill.numSamples); // [5]

}
