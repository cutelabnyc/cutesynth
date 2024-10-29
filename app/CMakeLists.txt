cmake_minimum_required(VERSION 3.10)
project(WavetableOscillator)

set(CMAKE_C_STANDARD 99)

# Add PortAudio include and library directories
include_directories(../thirdparty/portaudio/include)
link_directories(../thirdparty/portaudio/lib/.libs)

# Add your executable and source files
add_executable(WavetableOscillator main.c oscillator.c)

# Link the PortAudio library and the math library
target_link_libraries(WavetableOscillator portaudio m)

