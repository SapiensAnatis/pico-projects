#include <array>
#include <iostream>
#include <string>

#include "./util.hpp"
#include "http/http.hpp"
#include "parsing/parsing.hpp"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

constexpr uint32_t THREE_HOURS_MS = 3 * 60 * 60 * 1000;

constexpr uint32_t ERROR_SLEEP = THREE_HOURS_MS;
constexpr uint32_t SUCCESS_SLEEP = THREE_HOURS_MS * 2;

constexpr uint32_t WIFI_CONNECT_FAIL_SLEEP = 5 * 1000;

constexpr size_t RESPONSE_BUFFER_SIZE = 2048;

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
    http::HttpsGetResult fetch_result = http::fetch_collection_data(address, response_buffer);
    if (fetch_result != http::HttpsGetResult::Success) {
        std::cout << "Failed to fetch collection data: error=" << static_cast<int32_t>(fetch_result)
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

    auto parse_result = parsing::parse_response(response_view.substr(body_start));

    if (!parse_result.has_value()) {
        std::cout << "Failed to parse response: error="
                  << static_cast<int32_t>(parse_result.error()) << "\n";
        return false;
    }

    auto next_collection = std::get<0>(*parse_result);

    std::cout << "Next bin collection is " << static_cast<int32_t>(next_collection.collection_type)
              << " on " << next_collection.date << '\n';

    return true;
}

int main() {
    stdio_init_all();

    bool connected_to_wifi = false;
    do {
        connected_to_wifi = connect_wifi();

        if (!connected_to_wifi) {
            cyw43_arch_deinit();
            sleep_ms(WIFI_CONNECT_FAIL_SLEEP);
        }
    } while (!connected_to_wifi);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    std::string address = url_encode(BIN_UNICORN_HOME_ADDRESS);

    std::array<char, RESPONSE_BUFFER_SIZE> response_buffer;

    while (true) {
        bool success = work_loop<RESPONSE_BUFFER_SIZE>(address, response_buffer);
        if (success) {
            // TODO: If the device is started in the day prior to the collection data changing,
            // sleeping here could lead to stale data being displayed. Consider using NTP instead to
            // re-run the work loop at a specific time when an update is expected.

            std::cout << "Work loop succeeded. Sleeping for " << std::to_string(SUCCESS_SLEEP)
                      << " ms\n";
            sleep_ms(SUCCESS_SLEEP);
        } else {
            std::cout << "Work loop failed! Sleeping for " << std::to_string(ERROR_SLEEP)
                      << " ms\n";
            sleep_ms(ERROR_SLEEP);
        }
    }

    cyw43_arch_deinit();

    return 0;
}