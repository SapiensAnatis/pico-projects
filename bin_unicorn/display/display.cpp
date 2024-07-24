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
        return Color{};
    };
}

void draw_collection(BinCollection collection, bool half)
{
    uint8_t x_start = half ? UNICORN_MAX_X / 2 : 0;

    for (uint8_t x = x_start; x <= UNICORN_MAX_X; x++)
    {
        for (uint8_t y = 0; y <= UNICORN_MAX_Y; y++)
        {
            Color collection_colour = get_collection_colour(collection.collection_type);
            pico_unicorn.set_pixel(x, y, collection_colour.r, collection_colour.g, collection_colour.b);
        }
    }
}

void display_next_collections(BinCollection collection_1, BinCollection collection_2)
{
    draw_collection(collection_1, false);

    if (collection_2.date == collection_1.date)
    {
        draw_collection(collection_2, true);
    }
}