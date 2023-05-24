#include "pico/stdlib.h"
#include "MorseRenderer.hpp"

int main()
{
    MorseRenderer *renderer = new LEDRenderer();
    while (true)
    {
        renderer->Render("J");
    }
}
