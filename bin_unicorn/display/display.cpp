#define UNICORN_MAX_X 16
#define UNICORN_MAX_Y 7

#include "pico/stdlib.h"
#include "pico_unicorn.hpp"
#include "display.hpp"

using namespace pimoroni;

PicoUnicorn pico_unicorn;

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

static Color get_collection_colour(CollectionType type)
{
    switch (type)
    {
    case CollectionType::DomesticWaste:
        return Color{255, 255, 255};
    case CollectionType::FoodWaste:
        return Color{0, 0, 255};
    case CollectionType::Recycling:
        return Color{255, 0, 0};
    case CollectionType::GardenWaste:
        return Color{0, 255, 0};
    default:
        return Color{128, 128, 128};
    };
}

void display_next_collections(BinCollection collection_1, BinCollection collection_2)
{
    for (int x = 0; x < UNICORN_MAX_X / 2; x++)
    {
        for (int y = 0; y < UNICORN_MAX_Y; y++)
        {
            Color collection_colour = get_collection_colour(collection_1.collection_type);
            pico_unicorn.set_pixel(x, y, collection_colour.r, collection_colour.g, collection_colour.b);
        }
    }
}