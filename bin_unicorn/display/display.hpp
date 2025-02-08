#ifndef DISPLAY_DISPLAY_H_
#define DISPLAY_DISPLAY_H_

#include "../parsing/parsing.hpp"

/// @brief Display a WiFI symbol to indicate the Pico is connecting to the wireless network.
void display_connecting_wifi();

/// @brief Display a loading image to indicate the HTTP request / parsing is in progress.
void display_loading();

/// @brief Display the next two bin collections on the Unicorn display - using a different colour
/// for each type. If both collections are on the same day, the screen will light up with both
/// colours.
void display_next_collections(BinCollection collection_1, BinCollection collection_2);

/// @brief Draw a red X on the Unicorn display to indicate an error has occurred.
void display_failure();

#endif