# AudMod: Lightweight Realtime Voice Changer (JUCE)

A minimal, fully coded JUCE app that captures your microphone input and plays it back with switchable voice effects in realtime.

## Features (MVP)
- Realtime monitoring (`AudioAppComponent`) with low latency.
- 5 voice modes:
  - Clean
  - Robot (ring modulation)
  - Radio (bandpass + saturation)
  - Monster (low-pass + nonlinear shaping)
  - Whisper (de-voiced + noise blend)
- Wet/Dry control and per-mode intensity control.
- Small codebase designed to extend quickly.

## Build

### Requirements
- CMake 3.22+
- A C++17 compiler
- Audio device input/output support

### Configure + build
```bash
cmake -S . -B build
cmake --build build -j
```

JUCE is fetched automatically using `FetchContent`.

## Run
The executable will be in your build output folder. Launch it, allow microphone access, and pick a mode.

## Extend next
- Add formant/pitch-shift modules.
- Add recording/export.
- Add per-mode parameter pages.
- Convert to VST3/AU plugin target using JUCE plugin helpers.
