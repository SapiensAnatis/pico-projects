#include <memory>
#include "pico/stdlib.h"
#include "LEDRendererW.hpp"

int main()
{
    std::unique_ptr<MorseRenderer> renderer(new LEDRendererW());
    while (true)
    {
        renderer->Render("SOS");
    }
}
