#ifndef MorseRendererHpp
#define MorseRendererHpp

#include <string>
#include <map>
#include <cstdint>
#include "pico/stdlib.h"

template <uint16_t DitDuration>
class MorseRenderer
{
public:
    void Render(std::string inputString);

protected:
    static constexpr uint16_t ditDuration = DitDuration;
    static constexpr uint16_t dahDuration = DitDuration * 3;

    static constexpr uint16_t signalSpaceDuration = DitDuration;
    static constexpr uint16_t charSpaceDuration = DitDuration * 3;
    static constexpr uint16_t wordSpaceDuration = DitDuration * 7;

    virtual void Dit() = 0;
    virtual void Dah() = 0;

private:
    void Space()
    {
        sleep_ms(wordSpaceDuration);
    }

    static const std::map<char, const char *> morseMap;
};

template <uint16_t DitDuration>
class LEDRenderer : MorseRenderer<DitDuration>
{
public:
    LEDRenderer();

private:
    void Dit();
    void Dah();
    const uint ledPin = PICO_DEFAULT_LED_PIN;
};

#endif