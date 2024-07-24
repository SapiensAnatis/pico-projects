#include <iostream>

#include "pico_unicorn.hpp"
#include "pico/cyw43_arch.h"
#include "http/fetch_data.hpp"
#include "parsing/parsing.hpp"
#include "display/display.hpp"
#include "util.hpp"

bool connect_wifi()
{
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK))
    {
        std::cout << "Failed to initialise WiFi connection.\n";
        return false;
    }

    cyw43_arch_enable_sta_mode();

    std::cout << "Connecting to WiFi...\n";
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        std::cout << "Failed to connect to WiFi.\n";
        return false;
    }
    else
    {
        std::cout << "Connected to WiFi.\n";
        return true;
    }
}

int work_loop()
{
    std::vector<char> response_buffer;
    response_buffer.reserve(2048);

    std::string address = url_encode(BIN_UNICORN_HOME_ADDRESS);
    int fetch_result = fetch_collection_data(address, response_buffer);
    if (fetch_result != 0)
    {
        std::cout << "Failed to fetch collection data: error="
                  << std::to_string(fetch_result) << "\n";
        return -1;
    }

    // This is not really standards-compliant but it seems to work for now.
    // Could be a bit more intelligent and read the Content-Length header instead
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
    BinCollection next_collection_2;
    ParseResult parse_result = parse_response(response_view.substr(body_start), next_collection, next_collection_2);
    if (parse_result != ParseResult::Success)
    {
        std::cout << "Failed to parse response: error="
                  << std::to_string(parse_result) << "\n";
        return -1;
    }

    std::cout << "Next bin collection is " << (int32_t)next_collection.collection_type << " on " << next_collection.date << '\n';

    display_next_collections(next_collection, next_collection_2);

    return 0;
}

int main()
{
    stdio_init_all();

    while (!connect_wifi())
    {
        cyw43_arch_deinit();
        sleep_ms(3000);
    }

    int result = work_loop();
    if (result)
    {
        return result;
    }

    cyw43_arch_deinit();

    return 0;
}