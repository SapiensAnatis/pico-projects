#ifndef PARSING_PARSING_H_
#define PARSING_PARSING_H_

#include <vector>
#include "date/date.h"

enum CollectionType : uint8_t
{
    DomesticWaste,
    FoodWaste,
    Recycling,
    GardenWaste
};

struct BinCollection
{
    date::year_month_day date;
    CollectionType collection_type;
};

int8_t parse_response(const std::basic_string_view<char> &response_body, BinCollection &out_bin_collection);

#endif // PARSING_PARSING_H_