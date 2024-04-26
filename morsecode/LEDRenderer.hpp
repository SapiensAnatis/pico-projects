#if PICO

#ifndef LEDRendererHpp
#define LEDRendererHpp

#include <string>
#include <cstdint>
#include "pico/stdlib.h"
#include "MorseRenderer.hpp"

/// @brief LED rendered for standard Pico
class LEDRenderer : public MorseRenderer
{
public:
    LEDRenderer();

private:
    void Dit();
    void Dah();

    static const uint LedPin = PICO_DEFAULT_LED_PIN;
};

#endif

#endif