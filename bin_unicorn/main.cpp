#include <iostream>
#include <chrono>

#include "pico_unicorn.hpp"
#include "pico/cyw43_arch.h"
#include "http/fetch_data.hpp"

using namespace pimoroni;

PicoUnicorn pico_unicorn;

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
        sleep_ms(3000);
    }

    std::vector<char> response_buffer;
    response_buffer.reserve(2048);

    int result = fetch_collection_data("89%20Hamilton%20Road%20Reading,%20RG15RB", response_buffer);

    if (result != 0)
    {
        return -1;
    }

    std::cout << "Fetching second response" << std::endl;

    result = fetch_collection_data("89%20Hamilton%20Road%20Reading,%20RG15RB", response_buffer);

    if (result != 0)
    {
        return -1;
    }

    cyw43_arch_deinit();

    return 0;
}