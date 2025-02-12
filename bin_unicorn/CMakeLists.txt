add_executable(bin_unicorn 
    http/tls_client.c 
    http/http.cpp
    parsing/parsing.cpp
    ../lib/cJSON/cJSON.c
    main.cpp)

include(address.cmake)

target_compile_definitions(bin_unicorn PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    BIN_UNICORN_HOME_ADDRESS=\"${BIN_UNICORN_HOME_ADDRESS}\"
)

target_compile_options(bin_unicorn PRIVATE
    -Wall
    -Werror
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
)

# Disable Pico SDK warnings.
# Bit of a hack, but not all headers appear to be included using -isystem so -Werror fails the build without this
file(GLOB_RECURSE PICO_SDK_FILES "${PICO_SDK_PATH}/*.c" "${PICO_SDK_PATH}/*.h")

set_source_files_properties(
    ${PICO_SDK_FILES}
    PROPERTIES
    COMPILE_FLAGS
    -w # disable all warnings
)

# This file has to do some cursed C stuff
set_source_files_properties(
    http/tls_client.c 
    PROPERTIES
    COMPILE_FLAGS
    -Wno-pointer-sign # unsigned char shenanigans
)

target_link_libraries(bin_unicorn 
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mbedtls
    pico_mbedtls
    pico_stdlib
    hardware_pio
    hardware_dma
)

target_include_directories(bin_unicorn PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

target_include_directories(bin_unicorn SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/cJSON)

pico_add_extra_outputs(bin_unicorn)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")