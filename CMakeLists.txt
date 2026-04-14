cmake_minimum_required(VERSION 3.22)

project(AudModVoiceChanger VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.2
)
FetchContent_MakeAvailable(juce)

juce_add_gui_app(AudModVoiceChanger
  PRODUCT_NAME "AudMod Voice Changer"
)

target_sources(AudModVoiceChanger PRIVATE
  Source/Main.cpp
  Source/VoiceChangerComponent.cpp
  Source/VoiceChangerComponent.h
)

target_compile_definitions(AudModVoiceChanger PRIVATE
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
)

target_link_libraries(AudModVoiceChanger PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  juce::juce_gui_extra
)
