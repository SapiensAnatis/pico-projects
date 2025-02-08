#include "LEDRendererW.hpp"
#include "pico/stdlib.h"
#include <memory>

int main() {
    std::unique_ptr<MorseRenderer> renderer(new LEDRendererW());
    while (true) {
        renderer->Render("SOS");
    }
}
