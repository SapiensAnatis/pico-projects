include(../pimoroni_pico_import.cmake)

include(${PIMORONI_PICO_PATH}/common/pimoroni_i2c.cmake)
include(${PIMORONI_PICO_PATH}/common/pimoroni_bus.cmake)
include(${PIMORONI_PICO_PATH}/libraries/pico_unicorn/pico_unicorn.cmake)

add_executable(bin_unicorn 
    http/tls_client.c 
    http/fetch_data.cpp 
    parsing/parsing.cpp 
    ../lib/cJSON/cJSON.c
    main.cpp)

include(address.cmake)

target_compile_definitions(bin_unicorn PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    BIN_UNICORN_HOME_ADDRESS=\"${BIN_UNICORN_HOME_ADDRESS}\"
)

target_link_libraries(bin_unicorn 
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mbedtls
    pico_mbedtls
    pico_stdlib
    hardware_pio
    hardware_dma
    pico_graphics
    pico_unicorn
)

target_include_directories(bin_unicorn PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_SOURCE_DIR}/lib/cJSON
    ${CMAKE_SOURCE_DIR}/lib/date/include
)

pico_add_extra_outputs(bin_unicorn)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")