#include "VoiceChangerComponent.h"

VoiceChangerComponent::VoiceChangerComponent()
{
    title.setText ("AudMod Realtime Voice Changer", juce::dontSendNotification);
    title.setJustificationType (juce::Justification::centred);
    title.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    addAndMakeVisible (title);

    modeSelector.addItem ("Clean", static_cast<int> (VoiceMode::clean));
    modeSelector.addItem ("Robot", static_cast<int> (VoiceMode::robot));
    modeSelector.addItem ("Radio", static_cast<int> (VoiceMode::radio));
    modeSelector.addItem ("Monster", static_cast<int> (VoiceMode::monster));
    modeSelector.addItem ("Whisper", static_cast<int> (VoiceMode::whisper));
    modeSelector.setSelectedId (static_cast<int> (VoiceMode::clean));
    modeSelector.addListener (this);
    addAndMakeVisible (modeSelector);

    wetLabel.setText ("Wet Mix", juce::dontSendNotification);
    addAndMakeVisible (wetLabel);
    wetSlider.setRange (0.0, 1.0, 0.01);
    wetSlider.setValue (wetMix);
    wetSlider.addListener (this);
    addAndMakeVisible (wetSlider);

    intensityLabel.setText ("Intensity", juce::dontSendNotification);
    addAndMakeVisible (intensityLabel);
    intensitySlider.setRange (0.0, 1.0, 0.01);
    intensitySlider.setValue (intensity);
    intensitySlider.addListener (this);
    addAndMakeVisible (intensitySlider);

    setSize (540, 280);
    setAudioChannels (2, 2);
}

VoiceChangerComponent::~VoiceChangerComponent()
{
    shutdownAudio();
}

void VoiceChangerComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    juce::ignoreUnused (samplesPerBlockExpected);
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlockExpected), 2 };

    ringOsc.prepare (spec);
    ringOsc.setFrequency (45.0f);

    radioFilter.prepare (spec);
    radioFilter.setType (juce::dsp::StateVariableTPTFilterType::bandpass);
    radioFilter.setCutoffFrequency (1700.0f);
    radioFilter.setResonance (0.65f);

    lowPass.prepare (spec);
    lowPass.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    lowPass.setCutoffFrequency (220.0f);
    lowPass.setResonance (0.4f);
}

void VoiceChangerComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* buffer = bufferToFill.buffer;

    if (buffer == nullptr)
        return;

    auto dry = juce::AudioBuffer<float> (*buffer);
    auto wet = juce::AudioBuffer<float> (*buffer);

    switch (currentMode)
    {
        case VoiceMode::clean:
            break;
        case VoiceMode::robot:
            applyRobot (wet);
            break;
        case VoiceMode::radio:
            applyRadio (wet);
            break;
        case VoiceMode::monster:
            applyMonster (wet);
            break;
        case VoiceMode::whisper:
            applyWhisper (wet);
            break;
    }

    for (int channel = 0; channel < buffer->getNumChannels(); ++channel)
    {
        auto* dryData = dry.getReadPointer (channel, bufferToFill.startSample);
        auto* wetData = wet.getReadPointer (channel, bufferToFill.startSample);
        auto* out = buffer->getWritePointer (channel, bufferToFill.startSample);

        for (int i = 0; i < bufferToFill.numSamples; ++i)
        {
            const auto d = dryData[i];
            const auto w = wetData[i];
            out[i] = juce::jlimit (-1.0f, 1.0f, d * (1.0f - wetMix) + w * wetMix);
        }
    }
}

void VoiceChangerComponent::releaseResources() {}

void VoiceChangerComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (20, 24, 30));

    auto bounds = getLocalBounds().reduced (16);
    g.setColour (juce::Colour::fromRGB (42, 47, 58));
    g.fillRoundedRectangle (bounds.toFloat(), 12.0f);
}

void VoiceChangerComponent::resized()
{
    auto area = getLocalBounds().reduced (30);
    title.setBounds (area.removeFromTop (42));

    area.removeFromTop (8);
    modeSelector.setBounds (area.removeFromTop (34));

    area.removeFromTop (12);
    wetLabel.setBounds (area.removeFromTop (20));
    wetSlider.setBounds (area.removeFromTop (32));

    area.removeFromTop (12);
    intensityLabel.setBounds (area.removeFromTop (20));
    intensitySlider.setBounds (area.removeFromTop (32));
}

void VoiceChangerComponent::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &modeSelector)
        currentMode = static_cast<VoiceMode> (modeSelector.getSelectedId());
}

void VoiceChangerComponent::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &wetSlider)
        wetMix = static_cast<float> (wetSlider.getValue());

    if (slider == &intensitySlider)
        intensity = static_cast<float> (intensitySlider.getValue());
}

void VoiceChangerComponent::applyRobot (juce::AudioBuffer<float>& buffer)
{
    ringOsc.setFrequency (juce::jmap (intensity, 18.0f, 90.0f));

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer (ch);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
            data[i] *= 0.75f * ringOsc.processSample (0.0f);
    }
}

void VoiceChangerComponent::applyRadio (juce::AudioBuffer<float>& buffer)
{
    radioFilter.setCutoffFrequency (juce::jmap (intensity, 900.0f, 2800.0f));

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    radioFilter.process (context);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer (ch);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
            data[i] = std::tanh (data[i] * (1.8f + intensity * 3.2f));
    }
}

void VoiceChangerComponent::applyMonster (juce::AudioBuffer<float>& buffer)
{
    lowPass.setCutoffFrequency (juce::jmap (intensity, 120.0f, 350.0f));

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    lowPass.process (context);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer (ch);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const auto rectified = std::copysign (std::sqrt (std::abs (data[i])), data[i]);
            data[i] = std::tanh ((rectified * 1.5f) + (data[i] * 0.5f));
        }
    }
}

void VoiceChangerComponent::applyWhisper (juce::AudioBuffer<float>& buffer)
{
    const float noiseAmount = juce::jmap (intensity, 0.01f, 0.10f);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer (ch);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const auto noise = random.nextFloat() * 2.0f - 1.0f;
            const auto deVoiced = data[i] * 0.25f;
            data[i] = std::tanh (deVoiced + noise * noiseAmount);
        }
    }
}
