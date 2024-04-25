#include "LEDRenderer.hpp"

LEDRenderer::LEDRenderer()
{
    gpio_init(LedPin);
    gpio_set_dir(LedPin, GPIO_OUT);
}

void LEDRenderer::Dit()
{
    gpio_put(LedPin, 1);
    sleep_ms(MorseRenderer::DitDuration);
    gpio_put(LedPin, 0);
}

void LEDRenderer::Dah()
{
    gpio_put(LedPin, 1);
    sleep_ms(MorseRenderer::DahDuration);
    gpio_put(LedPin, 0);
}