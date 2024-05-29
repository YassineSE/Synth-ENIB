/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SynthENIBAudioProcessorEditor::SynthENIBAudioProcessorEditor (SynthENIBAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible (keyboardComponent);



    // Combobox for waveType Selection
    addAndMakeVisible(waveType);
    waveType.addItemList(juce::StringArray {"Sine", "Saw", "Square"}, 1);
    waveType.setTextWhenNothingSelected("Waveform");
    waveType.onChange = [this] {waveTypeChanged(); };
    waveLabel.setText ("Select WaveForm", juce::dontSendNotification);
    waveLabel.attachToComponent (&waveType, false);
    waveLabel.setJustificationType(juce::Justification::centred);
    
    // ADSR ACTIVATIOn
    //ADSR Controls
    addAndMakeVisible(adsrActivateButton);
    addAndMakeVisible(adsrActivateLabel);
    addAndMakeVisible(adsrLabel);
    adsrActivateButton  .onClick = [this] { updateToggleState (&adsrActivateButton);   };
    adsrActivateLabel.setText ("Activate", juce::dontSendNotification);
    adsrActivateLabel.attachToComponent (&adsrActivateButton, true);
    adsrActivateLabel.setJustificationType(juce::Justification::centred);
    adsrLabel.setText("ADSR", juce::dontSendNotification);
    adsrLabel.setJustificationType(juce::Justification::centred);
    adsrLabel.setFont(juce::Font (25.0f, 0));

    // ADSR ATTACHMENTS
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    attackAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "ATTACK", attackSlider);
    decayAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "DECAY", decaySlider);
    sustainkAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "SUSTAIN", sustainSlider);
    releaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "RELEASE", releaseSlider);
    
    //MakeVisible
    //Attack
    setSliderParams(attackSlider, attackLabel, "Attack");
    setSliderParams(decaySlider, decayLabel, "Decay");
    setSliderParams(sustainSlider, sustainLabel, "Sustain");
    setSliderParams(releaseSlider, releaseLabel, "Release");



    //Gain Controls
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(gainLabel);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN", gainSlider);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 30);
    gainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::palegreen);
    gainSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    gainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::seagreen);
    gainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::seagreen); 
    gainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::palegreen);
    gainLabel.setText ("Master Volume", juce::dontSendNotification);
    gainLabel.attachToComponent (&gainSlider, false);
    gainLabel.setJustificationType(juce::Justification::centred);

    
    // LFO FREQ CONTROL
    addAndMakeVisible(lfoFreqSlider);
    addAndMakeVisible(lfoFreqLabel);
    lfoFreqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LFOFREQ", lfoFreqSlider);
    lfoFreqSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 30);
    lfoFreqSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::palegreen);
    lfoFreqSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    lfoFreqSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::seagreen);
    lfoFreqSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::seagreen); 
    lfoFreqSlider.setColour(juce::Slider::thumbColourId, juce::Colours::palegreen);
    lfoFreqLabel.setText ("LFO FREQ", juce::dontSendNotification);
    lfoFreqLabel.attachToComponent (&lfoFreqSlider, false);
    lfoFreqLabel.setJustificationType(juce::Justification::centred);

    // LFO DEPTH CONTROL
    addAndMakeVisible(lfoDepthSlider);
    addAndMakeVisible(lfoDepthLabel);
    lfoDepthSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LFODEPTH", lfoDepthSlider);
    lfoDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    lfoDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 30);
    lfoDepthSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::palegreen);
    lfoDepthSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    lfoDepthSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::seagreen);
    lfoDepthSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::seagreen); 
    lfoDepthSlider.setColour(juce::Slider::thumbColourId, juce::Colours::palegreen);
    lfoDepthLabel.setText ("LFO DEPTH", juce::dontSendNotification);
    lfoDepthLabel.attachToComponent (&lfoDepthSlider, false);
    lfoDepthLabel.setJustificationType(juce::Justification::centred);


    // FILTER
    addAndMakeVisible(cutoffFrequencySlider);
    addAndMakeVisible(cutoffFrequencyLabel);
    cutoffFrequencySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    cutoffFrequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "cutoff_frequency", cutoffFrequencySlider);
    cutoffFrequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    cutoffFrequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 30);
    cutoffFrequencySlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::palegreen);
    cutoffFrequencySlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    cutoffFrequencySlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::seagreen);
    cutoffFrequencySlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::seagreen); 
    cutoffFrequencySlider.setColour(juce::Slider::thumbColourId, juce::Colours::palegreen);
    cutoffFrequencyLabel.setText("FILTER CUTOFF", juce::dontSendNotification);
    cutoffFrequencyLabel.attachToComponent (&cutoffFrequencySlider, false);
    cutoffFrequencyLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(highpassButton);
    highpassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "highpass", highpassButton);
    
    addAndMakeVisible(highpassButtonLabel);
    highpassButtonLabel.setText("HIGHPASS", juce::dontSendNotification);



    // WAVE VIEWER
    addAndMakeVisible(audioProcessor.waveViewer);
    audioProcessor.waveViewer.setColours(juce::Colours::black, juce::Colours::whitesmoke.withAlpha(0.5f));

    setSize (1200, 900);
    startTimer (400);


}

SynthENIBAudioProcessorEditor::~SynthENIBAudioProcessorEditor()
{
}

//==============================================================================
void SynthENIBAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SynthENIBAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    

    const auto bounds = getLocalBounds().reduced (10);
    const auto padding = 10;
    const auto sliderWidth = bounds.getWidth() / 6 - padding;
    const auto sliderHeight = bounds.getHeight() / 6 - padding;
    const auto sliderStartX = 140;
    const auto sliderStartY = bounds.getHeight() / 2 - (sliderHeight / 2) - 80;


    // WAVETYPE SELECTION COMBOBOX
    const auto waveTypeWidth = 300;
    const auto waveTypeHeight = 80;
    const auto waveTypeStartX = (getWidth()/2 - waveTypeWidth)/2;
    const auto waveTypeStartY = 60;
    waveType.setBounds(waveTypeStartX, waveTypeStartY, waveTypeWidth, waveTypeHeight);

    // GAIN SLIDER
    const auto gainSliderWidth = 160;
    const auto gainSliderHeight = 160;
    const auto gainSliderStartX = getWidth()/2 + (getWidth()/2-gainSliderWidth)/2;
    const auto gainSliderStartY = 60;
    gainSlider.setBounds(gainSliderStartX, gainSliderStartY, gainSliderWidth, gainSliderHeight);

    // ADSR ACTIVATION BUTTON
    
    const auto adsrActivateButtonWidth = 100;
    const auto adsrActivateButtonHeight = 100;
    const auto adsrActivateButtonStartX = getWidth()/2 - padding;
    const auto adsrActivateButtonStartY = gainSlider.getBottom() + padding;
    adsrLabel.setBounds(adsrActivateButtonStartX - 30, adsrActivateButtonStartY - 4*padding, 100, 15);
    adsrActivateButton.setBounds(adsrActivateButtonStartX + 2*padding, adsrLabel.getBottom()  - 2*padding, adsrActivateButtonWidth, adsrActivateButtonHeight);

    // ADSR SLIDERS
    attackSlider.setBounds(sliderStartX, sliderStartY, sliderWidth, sliderHeight);
    decaySlider.setBounds(attackSlider.getRight() + 50, sliderStartY, sliderWidth, sliderHeight);
    sustainSlider.setBounds(decaySlider.getRight() + 50, sliderStartY, sliderWidth, sliderHeight);
    releaseSlider.setBounds(sustainSlider.getRight() + 50, sliderStartY, sliderWidth, sliderHeight);
    
    
    // KEYBOARD COMPONENT
    const auto keyboardHeight = 150;
    const auto keyboardWidth = getWidth();
    keyboardComponent.setBounds (0, bounds.getHeight()- keyboardHeight + 2*padding, keyboardWidth, keyboardHeight);


    // LFO SLIDERS
    const auto lfoSliderWidth = 100;
    const auto lfoSliderHeight = 100;
    const auto lfoSliderStartX = sliderStartX - 4 * padding;
    const auto lfoSliderStartY = attackSlider.getBottom() + 8*padding;
    lfoFreqSlider.setBounds(lfoSliderStartX, lfoSliderStartY, lfoSliderWidth, lfoSliderHeight);
    lfoDepthSlider.setBounds(lfoFreqSlider.getRight() + 2*padding, lfoSliderStartY, lfoSliderWidth, lfoSliderHeight);
    


    // FILTER
    cutoffFrequencySlider.setBounds({ lfoDepthLabel.getRight() + 2*padding, lfoSliderStartY, lfoSliderWidth, lfoSliderHeight });
    
    highpassButton.setBounds(
        { cutoffFrequencySlider.getX(),
         cutoffFrequencySlider.getY() +
          cutoffFrequencySlider.getHeight() + 5,
         30, 50 });
    highpassButtonLabel.setBounds(
        { cutoffFrequencySlider.getX() + highpassButton.getWidth() + 5,
         highpassButton.getY(),
         cutoffFrequencySlider.getWidth() - highpassButton.getWidth(),
         highpassButton.getHeight() });
    
    // WAVE VIEWER
    audioProcessor.waveViewer.setBounds(getWidth()/2 + 25, lfoSliderStartY - 4*padding, 450, 250);
}

void SynthENIBAudioProcessorEditor::setSliderParams(juce::Slider& slider, juce::Label& label, juce::String text)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 30);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::palegreen);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::seagreen);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::seagreen); 
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::palegreen);
    label.setText (text, juce::dontSendNotification);
    label.attachToComponent (&slider, false);
    label.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}

void SynthENIBAudioProcessorEditor::waveTypeChanged()
{   
    auto tmpWaveId = waveType.getSelectedId();
    audioProcessor.synthAudioSource.audioSourceWaveId = tmpWaveId;
}







// update ADSR Toggle State
void SynthENIBAudioProcessorEditor::updateToggleState(juce::Button* Button)
{
    auto state = Button->getToggleState();
    audioProcessor.synthAudioSource.audioSourceApplyADSR = state;

}