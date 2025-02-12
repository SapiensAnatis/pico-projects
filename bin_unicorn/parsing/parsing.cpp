#include <cassert>
#include <charconv>
#include <expected>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>

#include "parsing.hpp"
#include "util.hpp"

namespace parsing {

static const std::map<std::string_view, CollectionType> collection_map = {
    {"Domestic Waste", CollectionType::DomesticWaste},
    {"Food Waste", CollectionType::FoodWaste},
    {"Recycling", CollectionType::Recycling},
    {"Garden Waste", CollectionType::GardenWaste}};

static bool try_parse_collection_string(const std::string_view &service_string,
                                        CollectionType &out_collection_type) {
    int first_word_end = service_string.find(" Collection Service");
    if (first_word_end == -1) {
        return false;
    }

    auto collection = collection_map.find(service_string.substr(0, first_word_end));
    if (collection == collection_map.end()) {
        return false;
    }

    out_collection_type = collection->second;
    return true;
}

static bool try_parse_date(const std::string_view &date_time_string, Date &out_date) {
    // We receive dates in the format DD/MM/YYYY 00:00:00
    // We don't care about the time

    auto string_cursor_pos = date_time_string.begin();

    uint8_t day = 0;
    std::string_view day_view{string_cursor_pos, string_cursor_pos + 2};

    if (!try_parse_number(day_view, day)) {
        return false;
    }

    string_cursor_pos += 3;
    assert(string_cursor_pos != date_time_string.end());

    uint8_t month = 0;
    std::string_view month_view{string_cursor_pos, string_cursor_pos + 2};

    if (!try_parse_number(month_view, month)) {
        return false;
    }

    string_cursor_pos += 3;
    assert(string_cursor_pos != date_time_string.end());

    uint16_t year = 0;
    std::string_view year_view{string_cursor_pos, string_cursor_pos + 4};

    if (!try_parse_number(year_view, year)) {
        return false;
    }

    out_date = Date{.year = year, .month = month, .day = day};

    return true;
}

static std::expected<BinCollection, ParseError> parse_collection(const cJSON *collection) {
    cJSON *date = cJSON_GetObjectItem(collection, "Date");
    if (!cJSON_IsString(date)) {
        std::cerr << "Error parsing JSON: $.Collections[0].Date was not a string\n";
        return std::unexpected(ParseError::InvalidJsonSchema);
    }

    cJSON *service = cJSON_GetObjectItem(collection, "Service");
    if (!cJSON_IsString(service)) {
        std::cerr << "Error parsing JSON: $.Collections[0].Service was not a string\n";
        return std::unexpected(ParseError::InvalidJsonSchema);
    }

    Date parsed_date;
    if (!try_parse_date(std::string_view(date->valuestring), parsed_date)) {
        std::cerr << "Error parsing JSON: $.Collections[0].Date '" << date->valuestring
                  << "' was not a valid date\n";
        return std::unexpected(ParseError::InvalidJsonSchema);
    }

    CollectionType parsed_collection_type;
    if (!try_parse_collection_string(std::string_view(service->valuestring),
                                     parsed_collection_type)) {
        std::cerr << "Error parsing JSON: $.Collections[0].Service '" << service->valuestring
                  << "' did not match expected format\n";
        return std::unexpected(ParseError::InvalidJsonSchema);
    }

    BinCollection out_bin_collection = {};

    out_bin_collection.date = parsed_date;
    out_bin_collection.collection_type = parsed_collection_type;

    return out_bin_collection;
}

std::ostream &operator<<(std::ostream &stream, Date date) {
    // Must static cast to 32-bit int otherwise uint8_t is printed as a char
    stream << static_cast<int32_t>(date.year) << "-";
    stream << std::setfill('0') << std::setw(2) << static_cast<int32_t>(date.month) << "-";
    stream << std::setfill('0') << std::setw(2) << static_cast<int32_t>(date.day);

    return stream;
}

std::expected<BinCollectionPair, ParseError> parse_response(const std::string_view &response_body) {
    auto json = std::unique_ptr<cJSON, decltype(cJSON_free) *>{
        cJSON_ParseWithLength(response_body.data(), response_body.size()), cJSON_free};

    if (json.get() == nullptr) {
        const char *error_ptr = cJSON_GetErrorPtr();
        std::cerr << "Error parsing JSON: parse failure at " << error_ptr << "\n";
        return std::unexpected(ParseError::InvalidJson);
    }

    cJSON *collections = cJSON_GetObjectItem(json.get(), "Collections");
    if (!cJSON_IsArray(collections)) {
        std::cerr << "Error parsing JSON: $.Collections was not an array\n";
        return std::unexpected(ParseError::InvalidJsonSchema);
    }

    cJSON *first_collection = cJSON_GetArrayItem(collections, 0);
    if (!cJSON_IsObject(first_collection)) {
        std::cerr << "Error parsing JSON: $.Collections[0] was not an object\n";
        return std::unexpected(ParseError::InvalidJsonSchema);
    }

    auto first_parse_result = parse_collection(first_collection);
    if (!first_parse_result.has_value()) {
        std::cerr << "Failed to parse first collection\n";
        return std::unexpected(first_parse_result.error());
    }

    cJSON *second_collection = cJSON_GetArrayItem(collections, 1);
    if (!cJSON_IsObject(first_collection)) {
        std::cerr << "Error parsing JSON: $.Collections[0] was not an object\n";
        return std::unexpected(ParseError::InvalidJsonSchema);
    }

    auto second_parse_result = parse_collection(second_collection);
    if (!second_parse_result.has_value()) {
        std::cerr << "Failed to parse second collection\n";
        return std::unexpected(second_parse_result.error());
    }

    return BinCollectionPair{*first_parse_result, *second_parse_result};
}

} // namespace parsing