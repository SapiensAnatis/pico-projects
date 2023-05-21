#include "MorseRenderer.hpp"

const std::map<char, std::string> MorseRenderer::MorseMap(
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

void MorseRenderer::Render(std::string inputString)
{
    for (char inputChar : inputString)
    {
        char lookup = toupper(inputChar);
        const std::string morseString = MorseMap.at(lookup);
        for (char morseChar : morseString)
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
            default:
                break;
            }
            sleep_ms(SignalSpaceDuration);
        }
        sleep_ms(CharSpaceDuration);
    }
    sleep_ms(WordSpaceDuration);
}

void MorseRenderer::Space()
{
    sleep_ms(WordSpaceDuration);
}

LEDRenderer::LEDRenderer()
{
    gpio_init(LedPin);
    gpio_set_dir(LedPin, GPIO_OUT);
}

void LEDRenderer::Dit()
{
    gpio_put(LedPin, 1);
    sleep_ms(MorseRenderer::DitDuration);
    gpio_put(LedPin, 0);
}

void LEDRenderer::Dah()
{
    gpio_put(LedPin, 1);
    sleep_ms(MorseRenderer::DahDuration);
    gpio_put(LedPin, 0);
}