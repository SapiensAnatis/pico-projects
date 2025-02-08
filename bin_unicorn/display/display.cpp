#define UNICORN_MAX_X 16
#define UNICORN_MAX_Y 7

#include "display.hpp"
#include "pico/stdlib.h"
#include "pico_unicorn.hpp"

using namespace pimoroni;

PicoUnicorn pico_unicorn;

struct Colour {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct DisplayCoordinate {
    uint8_t x;
    uint8_t y;
};

template <size_t N> struct MonoColourImage {
    Colour colour;
    DisplayCoordinate pixels[N];
};

// clang-format off
constexpr MonoColourImage error_image = {
    Colour{255, 0, 0},
    {
        {4, 0}, {5, 0}, {10, 0}, {11, 0},
        {5, 1}, {6, 1}, {9, 1}, {10, 1},
        {6, 2}, {7, 2}, {8, 2}, {9, 2},
        {7, 3}, {8, 3},
        {6, 4}, {7, 4}, {8, 4}, {9, 4},
        {5, 5}, {6, 5}, {9, 5}, {10, 5},
        {4, 6}, {5, 6}, {10, 6}, {11, 6},
    }
};

constexpr MonoColourImage connecting_wifi_image = {
    Colour{64, 64, 255},
    {
        {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
        {3, 1}, {12, 1},
        {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2},
        {4, 3}, {11, 3},
        {6, 4}, {7, 4}, {8, 4}, {9, 4},
        {5, 5}, {10, 5},
        {7, 6}, {8, 6},
    }
};

constexpr MonoColourImage loading_image = {
    Colour{255, 255, 255},
    {
        {3, 3}, {4, 3}, {7, 3}, {8, 3}, {11, 3}, {12, 3},
        {3, 4}, {4, 4}, {7, 4}, {8, 4}, {11, 4}, {12, 4},
    }
};
// clang-format on

static Colour get_collection_colour(CollectionType type) {
    switch (type) {
    case CollectionType::DomesticWaste:
        return Colour{255, 255, 255};
    case CollectionType::FoodWaste:
        return Colour{0, 0, 255};
    case CollectionType::Recycling:
        return Colour{255, 0, 0};
    case CollectionType::GardenWaste:
        return Colour{0, 255, 0};
    default:
        return Colour{};
    };
}

template <size_t N> static void display_mono_colour_image(const MonoColourImage<N> image) {
    pico_unicorn.clear();

    for (auto &point : image.pixels) {
        pico_unicorn.set_pixel(point.x, point.y, image.colour.r, image.colour.g, image.colour.b);
    }
}

static void display_collection(BinCollection collection, bool half) {
    uint8_t x_start = half ? UNICORN_MAX_X / 2 : 0;

    for (uint8_t x = x_start; x <= UNICORN_MAX_X; x++) {
        for (uint8_t y = 0; y <= UNICORN_MAX_Y; y++) {
            Colour collection_colour = get_collection_colour(collection.collection_type);
            pico_unicorn.set_pixel(x, y, collection_colour.r, collection_colour.g,
                                   collection_colour.b);
        }
    }
}

void display_connecting_wifi() { display_mono_colour_image(connecting_wifi_image); }

void display_loading() { display_mono_colour_image(loading_image); }

void display_next_collections(BinCollection collection_1, BinCollection collection_2) {
    display_collection(collection_1, false);

    if (collection_2.date == collection_1.date) {
        display_collection(collection_2, true);
    }
}

void display_failure() { display_mono_colour_image(error_image); }