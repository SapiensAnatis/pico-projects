#include <string>
#include "MorseRenderer.hpp"
#include "pico/stdlib.h"

template <uint16_t DitDuration>
const std::map<char, const char *> MorseRenderer<DitDuration>::morseMap(
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

template <uint16_t DitDuration>
void MorseRenderer<DitDuration>::Render(std::string inputString)
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

template <uint16_t DitDuration>
LEDRenderer<DitDuration>::LEDRenderer()
{
    gpio_init(ledPin);
}

template <uint16_t DitDuration>
void LEDRenderer<DitDuration>::Dit()
{
    gpio_put(ledPin, 1);
    sleep_ms(MorseRenderer<DitDuration>::ditDuration);
    gpio_put(ledPin, 0);
}

template <uint16_t DitDuration>
void LEDRenderer<DitDuration>::Dah()
{
    gpio_put(ledPin, 1);
    sleep_ms(MorseRenderer<DitDuration>::dahDuration);
    gpio_put(ledPin, 0);
}
