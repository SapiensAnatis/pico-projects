#ifndef MorseRendererHpp
#define MorseRendererHpp

#include <string>
#include <map>
#include <cstdint>
#include <string>
#include "pico/stdlib.h"

class MorseRenderer
{
public:
    void Render(std::string inputString);

protected:
    static constexpr uint16_t ditDuration = 500;
    static constexpr uint16_t dahDuration = ditDuration * 3;

    static constexpr uint16_t signalSpaceDuration = ditDuration;
    static constexpr uint16_t charSpaceDuration = ditDuration * 3;
    static constexpr uint16_t wordSpaceDuration = ditDuration * 7;

    virtual void Dit() = 0;
    virtual void Dah() = 0;

private:
    void Space();

    static const std::map<char, std::string> morseMap;
};

class LEDRenderer : public MorseRenderer
{
public:
    LEDRenderer();

private:
    void Dit();
    void Dah();

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
};

#endif