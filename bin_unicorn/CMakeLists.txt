include(../pimoroni_pico_import.cmake)

include(${PIMORONI_PICO_PATH}/common/pimoroni_i2c.cmake)
include(${PIMORONI_PICO_PATH}/common/pimoroni_bus.cmake)
include(${PIMORONI_PICO_PATH}/libraries/bitmap_fonts/bitmap_fonts.cmake)
include(${PIMORONI_PICO_PATH}/libraries/hershey_fonts/hershey_fonts.cmake)
include(${PIMORONI_PICO_PATH}/libraries/pico_explorer/pico_explorer.cmake)
include(${PIMORONI_PICO_PATH}/libraries/pico_unicorn/pico_unicorn.cmake)

add_executable(bin_unicorn http/tls_client.c http/fetch_data.cpp main.cpp)

target_compile_definitions(bin_unicorn PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
)

target_link_libraries(bin_unicorn 
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mbedtls
    pico_mbedtls
    pico_stdlib
    hardware_pio
	hardware_pwm
    hardware_dma
    pico_graphics
    pico_unicorn
)

target_include_directories(bin_unicorn PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_SOURCE_DIR}/lib/cJSON
)

pico_add_extra_outputs(bin_unicorn)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")