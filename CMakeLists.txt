# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.20)

set(PICO_BOARD pico_w CACHE STRING "Board type")
set(PICO_DEOPTIMIZED_DEBUG 1)

if (PICO_BOARD STREQUAL pico_w)
    add_compile_definitions(PICO_W)
endif()

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)

project(JayPicoProjects C CXX ASM)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
# set(PICO_CXX_ENABLE_EXCEPTIONS 1)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()



add_subdirectory(morsecode)

include(wifi_credentials.cmake OPTIONAL)

if (DEFINED WIFI_SSID AND DEFINED WIFI_PASSWORD)
    message("Detected WIFI_SSD and WIFI_PASSWORD. Including networking-enabled projects.")
    add_subdirectory(bin_unicorn)
else()
    message(WARNING "WIFI_SSD and/or WIFI_PASSWORD not set. Skipping configuration of projects that require networking. "
                    "To build these projects, create a file called wifi_credentials.cmake at the repository root which "
                    "sets these variables.")
endif()
