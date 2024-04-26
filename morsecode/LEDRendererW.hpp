#if PICO_W

#ifndef LEDRendererWHpp
#define LEDRendererWHpp

#include "pico/stdlib.h"
#include "MorseRenderer.hpp"

/// @brief LED renderer for Pico W
class LEDRendererW : public MorseRenderer
{
public:
    LEDRendererW();

private:
    void Dit();
    void Dah();
};

#endif

#endif