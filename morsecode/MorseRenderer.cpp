#include "MorseRenderer.hpp"

const std::map<char, std::string> MorseRenderer::morseMap(
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
        const std::string morseString = morseMap.at(lookup);
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
            sleep_ms(signalSpaceDuration);
        }

        sleep_ms(charSpaceDuration);
    }
}

void MorseRenderer::Space()
{
    sleep_ms(wordSpaceDuration);
}

LEDRenderer::LEDRenderer()
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

void LEDRenderer::Dit()
{
    gpio_put(LED_PIN, 1);
    sleep_ms(MorseRenderer::ditDuration);
    gpio_put(LED_PIN, 0);
}

void LEDRenderer::Dah()
{
    gpio_put(LED_PIN, 1);
    sleep_ms(MorseRenderer::dahDuration);
    gpio_put(LED_PIN, 0);
}