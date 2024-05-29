/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SynthAudioSource.h"
//==============================================================================
/**
*/
class SynthENIBAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    SynthENIBAudioProcessorEditor (SynthENIBAudioProcessor&);
    ~SynthENIBAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void setSliderParams(juce::Slider& slider, juce::Label& label, juce::String text);
    void waveTypeChanged();
    void updateToggleState(juce::Button* Button);


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SynthENIBAudioProcessor& audioProcessor;

    juce::MidiKeyboardComponent keyboardComponent;
    
    void timerCallback() override
    {
        keyboardComponent.grabKeyboardFocus();
        stopTimer();
    }

     //ADSR
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> sustainkAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    // ADSR Activate Button
    juce::ToggleButton adsrActivateButton;
    juce::Label adsrActivateLabel;
    juce::Label adsrLabel;
    

    // Combobox for Wavetypes
    //Combobox Control
    juce::ComboBox waveType;
    juce::Label waveLabel;



    //Gain Control
    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<SliderAttachment> gainSliderAttachment;

    //LFO CONTROL
    juce::Slider lfoFreqSlider;
    juce::Label lfoFreqLabel;
    juce::Slider lfoDepthSlider;
    juce::Label lfoDepthLabel;
    std::unique_ptr<SliderAttachment> lfoFreqSliderAttachment;
    std::unique_ptr<SliderAttachment> lfoDepthSliderAttachment;

    //FILTER
    juce::Slider cutoffFrequencySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffFrequencyAttachment;
    juce::Label cutoffFrequencyLabel;

    juce::ToggleButton highpassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highpassAttachment;
    juce::Label highpassButtonLabel;


    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthENIBAudioProcessorEditor)
};
