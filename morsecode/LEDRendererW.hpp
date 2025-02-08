#if PICO_W

#ifndef LEDRendererWHpp
#define LEDRendererWHpp

#include "MorseRenderer.hpp"
#include "pico/stdlib.h"

/// @brief LED renderer for Pico W
class LEDRendererW : public MorseRenderer {
  public:
    LEDRendererW();

  private:
    void Dit();
    void Dah();
};

#endif

#endif