#ifndef MorseRendererHpp
#define MorseRendererHpp

#include <string>
#include <map>
#include <cstdint>
#include <string>
#include "pico/stdlib.h"

template <uint16_t DitDuration>
class MorseRenderer
{
public:
    void Render(std::string inputString)
    {
        for (char inputChar : inputString)
        {
            for (char morseChar : morseMap.at(inputChar))
            {
                switch (morseChar)
                {
                case '.':
                    Dit();
                    break;
                case '-':
                    Dah();
                    break;
                case ' ':
                    Space();
                    break;
                }
                sleep_ms(signalSpaceDuration);
            }

            sleep_ms(charSpaceDuration);
        }
    }

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

    static const std::map<char, std::string> morseMap;
};

template <uint16_t DitDuration>
class LEDRenderer : public MorseRenderer<DitDuration>
{
public:
    LEDRenderer()
    {
        gpio_init(ledPin);
    }

private:
    void Dit()
    {
        gpio_put(ledPin, 1);
        sleep_ms(MorseRenderer<DitDuration>::ditDuration);
        gpio_put(ledPin, 0);
    }

    void Dah()
    {
        gpio_put(ledPin, 1);
        sleep_ms(MorseRenderer<DitDuration>::dahDuration);
        gpio_put(ledPin, 0);
    }
    const uint ledPin = PICO_DEFAULT_LED_PIN;
};

template <uint16_t DitDuration>
const std::map<char, std::string> MorseRenderer<DitDuration>::morseMap(
    {{'A', ".-"},
     {'B', "-..."},
     {'C', "-.-."},
     {'D', "-.."},
     {'E', "."},
     {'F', "..-."},
     {'G', "--."},
     {'H', "...."},
     {'I', ".."},
     {'J', ".---"},
     {'K', "-.-"},
     {'L', ".-.."},
     {'M', "--"},
     {'N', "-."},
     {'O', "---"},
     {'P', ".--."},
     {'Q', "--.-"},
     {'R', ".-."},
     {'S', "..."},
     {'T', "-"},
     {'U', "..-"},
     {'V', "...-"},
     {'W', ".--"},
     {'X', "-..-"},
     {'Y', "-.--"},
     {'Z', "--.."},
     {'1', ".----"},
     {'2', "..---"},
     {'3', "...--"},
     {'4', "....-"},
     {'5', "....."},
     {'6', "-...."},
     {'7', "--..."},
     {'8', "---.."},
     {'9', "----."},
     {'0', "-----"},
     {' ', " "}});

#endif