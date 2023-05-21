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
    void Render(const std::string &inputString);

protected:
    static constexpr uint16_t DitDuration = 200; // Milliseconds
    static constexpr uint16_t DahDuration = DitDuration * 3;

    static constexpr uint16_t SignalSpaceDuration = DitDuration;
    static constexpr uint16_t CharSpaceDuration = DitDuration * 3;
    static constexpr uint16_t WordSpaceDuration = DitDuration * 7;

    virtual void Dit() = 0;
    virtual void Dah() = 0;

private:
    void Space();

    static const std::map<char, const std::string> MorseMap;
};

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