#include <iostream>

#include "pico_unicorn.hpp"
#include "pico/cyw43_arch.h"
#include "http/fetch_data.hpp"
#include "parsing/parsing.hpp"

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

    while (!connect_wifi())
    {
        cyw43_arch_deinit();
        sleep_ms(3000);
    }

    std::vector<char> response_buffer;
    response_buffer.reserve(2048);

    int fetch_result = fetch_collection_data(BIN_UNICORN_HOME_ADDRESS, response_buffer);
    if (fetch_result != 0)
    {
        std::cout << "Failed to fetch collection data: error " << fetch_result << '\n';
        return -1;
    }

    // This is not really standards-compliant but it seems to work for now.
    // TODO: Be a bit more intelligent and read the Content-Length header instead
    if (response_buffer.back() != 0)
    {
        std::cout << "Response buffer was not null-terminated - this is not supported.\n";
        return -1;
    }

    std::string_view response_view(response_buffer.data());
    int body_start = response_view.find("\r\n\r\n");
    int nr = response_view.find("OK");
    if (body_start == std::string::npos)
    {
        std::cout << "Failed to find start of response body\n";
        return -1;
    }

    BinCollection next_collection;
    int parse_result = parse_response(response_view.substr(body_start), next_collection);
    if (parse_result != 0)
    {
        std::cout << "Failed to parse response: error " << parse_result << '\n';
        return -1;
    }

    std::cout << "Next bin collection is " << (int32_t)next_collection.collection_type << " on " << next_collection.date << '\n';

    cyw43_arch_deinit();

    return 0;
}