#if PICO_W

#include "LEDRendererW.hpp"
#include "pico/cyw43_arch.h"

LEDRendererW::LEDRendererW()
{
    stdio_init_all();
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed");
        exit(1);
    }
}

void LEDRendererW::Dit()
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(MorseRenderer::DitDuration);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

void LEDRendererW::Dah()
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(MorseRenderer::DahDuration);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

#endif