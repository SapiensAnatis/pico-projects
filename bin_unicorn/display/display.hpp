#ifndef DISPLAY_DISPLAY_H_
#define DISPLAY_DISPLAY_H_

#include "../parsing/parsing.hpp"

/// @brief Display the next two bin collections on the Unicorn display - using a different colour for each type.
/// If both collections are on the same day, the screen will light up with both colours.
void display_next_collections(BinCollection collection_1, BinCollection collection_2);

#endif