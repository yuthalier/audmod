#pragma once

#include <JuceHeader.h>

class VoiceChangerComponent final : public juce::AudioAppComponent,
                                    private juce::ComboBox::Listener,
                                    private juce::Slider::Listener
{
public:
    VoiceChangerComponent();
    ~VoiceChangerComponent() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    enum class VoiceMode
    {
        clean = 1,
        robot,
        radio,
        monster,
        whisper
    };

    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged (juce::Slider* slider) override;

    void applyRobot (juce::AudioBuffer<float>& buffer);
    void applyRadio (juce::AudioBuffer<float>& buffer);
    void applyMonster (juce::AudioBuffer<float>& buffer);
    void applyWhisper (juce::AudioBuffer<float>& buffer);

    VoiceMode currentMode { VoiceMode::clean };
    float wetMix { 0.7f };

    juce::ComboBox modeSelector;
    juce::Slider wetSlider;
    juce::Slider intensitySlider;
    juce::Label title;
    juce::Label wetLabel;
    juce::Label intensityLabel;

    juce::dsp::Oscillator<float> ringOsc { [] (float x) { return std::sin (x); } };
    juce::dsp::StateVariableTPTFilter<float> radioFilter;
    juce::dsp::StateVariableTPTFilter<float> lowPass;
    juce::Random random;

    float intensity { 0.6f };
    double currentSampleRate { 44100.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceChangerComponent)
};
