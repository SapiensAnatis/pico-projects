add_executable(morse main.cpp MorseRenderer.cpp)

# pull in common dependencies
target_link_libraries(morse pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(morse)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
