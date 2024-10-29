project ("CuteSynth" C)
cmake_minimum_required (VERSION 3.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_subdirectory (src)
add_subdirectory (app)

include(CTest)
add_subdirectory (thirdparty)
add_subdirectory (test)

# Add PortAudio as an external project
add_subdirectory(thirdparty/portaudio)

# Link your project with PortAudio
target_link_libraries(CuteSynth PRIVATE portaudio_static)

