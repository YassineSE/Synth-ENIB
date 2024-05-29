/*
  ==============================================================================

    SynthSound.h
    Created: 27 May 2024 12:09:09pm
    Author:  yassine

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class SynthSound : public juce::SynthesiserSound
{
public:
  bool appliesToNote (int midiNoteNumber) override { return true;}
  bool appliesToChannel (int midiChannel) override { return true;}
};