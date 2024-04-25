#include "pico/stdlib.h"
#include "LEDRendererW.hpp"

int main()
{
    MorseRenderer *renderer = new LEDRendererW();
    while (true)
    {
        renderer->Render("J");
    }
}
