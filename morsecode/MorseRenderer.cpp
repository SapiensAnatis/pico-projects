#include "MorseRenderer.hpp"

const std::string MorseRenderer::GetMorse(char inputChar)
{
    char lookupChar = toupper(inputChar);
    MorseCharacter encoded;

    if (lookupChar <= '9')
    {
        encoded = Digits[lookupChar - '0'];
    }
    else
    {
        encoded = Letters[lookupChar - 'A'];
    }

    return DecodeMorse(encoded);
}

void MorseRenderer::Render(const std::string &inputString)
{
    for (char inputChar : inputString)
    {
        // Spaces are not encoded in the list of characters
        if (inputChar == ' ')
        {
            Space();
            continue;
        }

        const std::string morseString = GetMorse(inputChar);

        for (char morseChar : morseString)
        {
            switch (morseChar)
            {
            case DotCharacter:
                Dit();
                break;
            case DashCharacter:
                Dah();
                break;
            case SpaceCharacter:
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