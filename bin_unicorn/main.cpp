#include <iostream>
#include <chrono>

#include "pico_unicorn.hpp"
#include "pico/cyw43_arch.h"

extern "C"
{
#include "tls_client.h"
}

using namespace pimoroni;

PicoUnicorn pico_unicorn;

#ifndef WIFI_SSID
#error Need to set WIFI_SSID
#endif
#ifndef WIFI_PASSWORD
#error Need to set WIFI_PASSWORD
#endif

bool connect_wifi()
{
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK))
    {
        printf("failed to initialise\n");
        return false;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
        return false;
    }
    else
    {
        printf("Connected.\n");
        return true;
    }
}

int main()
{
    stdio_init_all();

    std::cout << "Hello world from main.cpp" << std::endl;

    while (!connect_wifi())
    {
        cyw43_arch_deinit();
        sleep_ms(10000);
    }

    auto buffer = new std::vector<char>();
    buffer->reserve(1024);

    auto start = std::chrono::high_resolution_clock::now();
    int result = https_get(
        "api.reading.gov.uk",
        "/uk/1",
        "",
        buffer->data(),
        buffer->capacity());
    auto end = std::chrono::high_resolution_clock::now();

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Request finished in " << milliseconds.count() << " ms" << std::endl;

    pico_unicorn.set_pixel(0, 0, 255, 0, 0);
    pico_unicorn.set_pixel(1, 1, 255, 0, 0);

    cyw43_arch_deinit();

    return 0;
}