#include <array>
#include <iostream>

#include "http/fetch_data.hpp"
#include "parsing/parsing.hpp"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "util.hpp"

constexpr uint32_t three_hours_ms = 3 * 60 * 60 * 1000;

constexpr uint32_t error_sleep = three_hours_ms;
constexpr uint32_t success_sleep = three_hours_ms * 2;

constexpr uint32_t wifi_connect_fail_sleep = 5 * 1000;

constexpr size_t response_buffer_size = 2048;

/// @brief Connect to the WiFi network using the WIFI_SSID and WIFI_PASSWORD definitions.
/// @return True if successful, otherwise false.
bool connect_wifi() {
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK)) {
        std::cout << "Failed to initialise WiFi connection.\n";
        return false;
    }

    cyw43_arch_enable_sta_mode();

    std::cout << "Connecting to WiFi...\n";
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK,
                                           30000)) {
        std::cout << "Failed to connect to WiFi.\n";
        return false;
    } else {
        std::cout << "Connected to WiFi.\n";
        return true;
    }
}

/// @brief Main worker loop.
/// @param address The address to fetch collection data for.
/// @param response_buffer
/// @return True if the loop succeeded, otherwise false.
template <size_t BufferSize>
bool work_loop(const std::string address, std::array<char, BufferSize> &response_buffer) {
    int fetch_result = fetch_collection_data(address, response_buffer);
    if (fetch_result != 0) {
        std::cout << "Failed to fetch collection data: error=" << std::to_string(fetch_result)
                  << "\n";
        return false;
    }

    // This is not really standards-compliant but it seems to work for now.
    // Could be a bit more intelligent and read the Content-Length header instead
    if (response_buffer.back() != 0) {
        std::cout << "Response buffer was not null-terminated - this is not supported.\n";
        return false;
    }

    std::string_view response_view(response_buffer.data());
    int body_start = response_view.find("\r\n\r\n");
    int nr = response_view.find("OK");
    if (body_start == std::string::npos) {
        std::cout << "Failed to find start of response body\n";
        return false;
    }

    BinCollection next_collection;
    BinCollection next_collection_2;
    ParseResult parse_result =
        parse_response(response_view.substr(body_start), next_collection, next_collection_2);
    if (parse_result != ParseResult::Success) {
        std::cout << "Failed to parse response: error=" << std::to_string(parse_result) << "\n";
        return false;
    }

    std::cout << "Next bin collection is " << (int32_t)next_collection.collection_type << " on "
              << next_collection.date << '\n';

    return true;
}

int main() {
    stdio_init_all();

    bool connected_to_wifi = false;
    do {
        connected_to_wifi = connect_wifi();

        if (!connected_to_wifi) {
            cyw43_arch_deinit();
            sleep_ms(wifi_connect_fail_sleep);
        }
    } while (!connected_to_wifi);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    std::string address = url_encode(BIN_UNICORN_HOME_ADDRESS);

    std::array<char, response_buffer_size> response_buffer;

    while (true) {
        bool success = work_loop<response_buffer_size>(address, response_buffer);
        if (success) {
            // TODO: If the device is started in the day prior to the collection data changing,
            // sleeping here could lead to stale data being displayed. Consider using NTP instead to
            // re-run the work loop at a specific time when an update is expected.

            std::cout << "Work loop succeeded. Sleeping for " << std::to_string(success_sleep)
                      << " ms\n";
            sleep_ms(success_sleep);
        } else {
            std::cout << "Work loop failed! Sleeping for " << std::to_string(error_sleep)
                      << " ms\n";
            sleep_ms(error_sleep);
        }
    }

    cyw43_arch_deinit();

    return 0;
}