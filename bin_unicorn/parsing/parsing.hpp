#ifndef PARSING_PARSING_H_
#define PARSING_PARSING_H_

extern "C"
{
#include "cJSON.h"
}

#include "date/date.h"

/// @brief A type of bin collection.
enum class CollectionType : uint8_t
{
    DomesticWaste,
    FoodWaste,
    Recycling,
    GardenWaste
};

/// @brief A data object representing a bin collection.
struct BinCollection
{
    date::year_month_day date;
    CollectionType collection_type;
};

/// @brief RAII wrapper for a cJSON pointer.
struct cJSONWrapper
{
    cJSON *json;

    cJSONWrapper(cJSON *json)
    {
        this->json = json;
    }

    ~cJSONWrapper()
    {
        cJSON_free(this->json);
    }
};

/// @brief Represents the result of trying to parse a bin collection.
enum ParseResult : int8_t
{
    /// @brief The provided JSON was malformed and could not be parsed by cJSON.
    InvalidJson = -2,
    /// @brief The provided JSON did not match the expected object format.
    InvalidJsonSchema = -1,
    /// @brief The parsing succeeded.
    Success = 0,
};

/// @brief Attempt to parse the two first bin collections in the array.
/// @param response_body A string_view of the JSON response body returned from the RBC API.
/// @param out_bin_collection_1 A reference to write the first parsed bin collection to.
/// @param out_bin_collection_2 A reference to write the second parsed bin collection to.
/// @returns A value indicating parse status (failure/success).
ParseResult parse_response(const std::basic_string_view<char> &response_body, BinCollection &out_bin_collection_1, BinCollection &out_bin_collection_2);

#endif // PARSING_PARSING_H_