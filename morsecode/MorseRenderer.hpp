#ifndef MorseRendererHpp
#define MorseRendererHpp

#include <string>
#include <cstdint>
#include "pico/stdlib.h"

struct MorseCharacter
{
    uint8_t MorseLen : 3;
    uint8_t MorseData : 5; // 0 = dot, 1 = dash
};

constexpr char DotCharacter = '.';
constexpr char DashCharacter = '-';
constexpr char SpaceCharacter = ' ';

static constexpr MorseCharacter EncodeMorse(const char *morse)
{
    MorseCharacter result = {};

    for (int i = 0; morse[i] != 0; i++)
    {
        result.MorseLen++;
        result.MorseData |= (morse[i] == DashCharacter) << i;
    }

    return result;
}

static const std::string DecodeMorse(const MorseCharacter &morse)
{
    std::string result;
    result.reserve(morse.MorseLen);

    for (int i = 0; i < morse.MorseLen; i++)
    {
        bool isDash = (1 << i) & morse.MorseData;
        result += isDash ? '-' : '.';
    }

    return result;
}

class MorseRenderer
{
public:
    void Render(const std::string &inputString);

protected:
    static constexpr uint16_t DitDuration = 250; // Milliseconds
    static constexpr uint16_t DahDuration = DitDuration * 3;

    static constexpr uint16_t SignalSpaceDuration = DitDuration;
    static constexpr uint16_t CharSpaceDuration = DitDuration * 3;
    static constexpr uint16_t WordSpaceDuration = DitDuration * 7;

    virtual void Dit() = 0;
    virtual void Dah() = 0;

private:
    void Space();
    const std::string GetMorse(char inputChar);

    static constexpr MorseCharacter Digits[10]{
        EncodeMorse("-----"), // 0
        EncodeMorse(".----"), // 1
        EncodeMorse("..---"), // 2
        EncodeMorse("...--"), // 3
        EncodeMorse("....-"), // 4
        EncodeMorse("....."), // 5
        EncodeMorse("-...."), // 6
        EncodeMorse("--..."), // 7
        EncodeMorse("---.."), // 8
        EncodeMorse("----."), // 9
    };

    static constexpr MorseCharacter Letters[26]{
        EncodeMorse(".-"),   // A
        EncodeMorse("-..."), // B
        EncodeMorse("-.-."), // C
        EncodeMorse("-.."),  // D
        EncodeMorse("."),    // E
        EncodeMorse("..-."), // F
        EncodeMorse("--."),  // G
        EncodeMorse("...."), // H
        EncodeMorse(".."),   // I
        EncodeMorse(".---"), // J
        EncodeMorse("-.-"),  // K
        EncodeMorse(".-.."), // L
        EncodeMorse("--"),   // M
        EncodeMorse("-."),   // N
        EncodeMorse("---"),  // O
        EncodeMorse(".--."), // P
        EncodeMorse("--.-"), // Q
        EncodeMorse(".-."),  // R
        EncodeMorse("..."),  // S
        EncodeMorse("-"),    // T
        EncodeMorse("..-"),  // U
        EncodeMorse("...-"), // V
        EncodeMorse(".--"),  // W
        EncodeMorse("-..-"), // X
        EncodeMorse("-.--"), // Y
        EncodeMorse("--.."), // Z
    };
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