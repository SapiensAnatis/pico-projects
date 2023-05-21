#include "pico/stdlib.h"
#include "MorseRenderer.hpp"

int main()
{
    MorseRenderer<500> *renderer = new LEDRenderer<500>();
    renderer->Render("test");
}
