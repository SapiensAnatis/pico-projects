#include <cassert>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <charconv>
#include <memory>

#include "parsing.hpp"

static const std::map<const std::basic_string_view<char>, CollectionType> collection_map = {
    {"Domestic Waste", CollectionType::DomesticWaste},
    {"Food Waste", CollectionType::FoodWaste},
    {"Recycling", CollectionType::Recycling},
    {"Garden Waste", CollectionType::GardenWaste}};

static bool parse_collection_string(const std::basic_string_view<char> &service_string,
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

template <typename TNumber>
static bool try_parse_number(const std::string_view &input, TNumber &out)
{
    const std::from_chars_result result =
        std::from_chars(input.data(), input.data() + input.size(), out);

    if (result.ec == std::errc::invalid_argument || result.ec == std::errc::result_out_of_range) {
        return false;
    }

    return true;
}

static bool parse_date(const std::string &date_time_string, Date& out_date) {
    // We receive dates in the format DD/MM/YYYY 00:00:00
    // We don't care about the time

    auto string_cursor_pos = date_time_string.begin();
    
    uint8_t day;
    std::string_view day_view{string_cursor_pos, string_cursor_pos + 2};

    if (!try_parse_number(day_view, day)) {
        return false;
    }

    string_cursor_pos += 3;
    assert(string_cursor_pos != date_time_string.end());

    uint8_t month;
    std::string_view month_view{string_cursor_pos, string_cursor_pos + 2};

    if (!try_parse_number(month_view, month)) {
        return false;
    }

    string_cursor_pos += 3;
    assert(string_cursor_pos != date_time_string.end());

    uint16_t year;
    std::string_view year_view{string_cursor_pos, string_cursor_pos + 4};

    if (!try_parse_number(year_view, year)) {
        return false;
    }

    out_date = Date{.year = year, .month = month, .day = day};

    return true;
}

static ParseResult parse_collection(const cJSON *collection, BinCollection &out_bin_collection) {
    cJSON *date = cJSON_GetObjectItem(collection, "Date");
    if (!cJSON_IsString(date)) {
        std::cerr << "Error parsing JSON: $.Collections[0].Date was not a string\n";
        return ParseResult::InvalidJsonSchema;
    }

    cJSON *service = cJSON_GetObjectItem(collection, "Service");
    if (!cJSON_IsString(service)) {
        std::cerr << "Error parsing JSON: $.Collections[0].Service was not a string\n";
        return ParseResult::InvalidJsonSchema;
    }

    Date parsed_date;
    if (!parse_date(std::string(date->valuestring), parsed_date)) {
        std::cerr << "Error parsing JSON: $.Collections[0].Date '" << date->valuestring
                  << "' was not a valid date\n";
        return ParseResult::InvalidJsonSchema;
    }

    CollectionType parsed_collection_type;
    if (!parse_collection_string(std::string(service->valuestring), parsed_collection_type)) {
        std::cerr << "Error parsing JSON: $.Collections[0].Service '" << service->valuestring
                  << "' did not match expected format\n";
        return ParseResult::InvalidJsonSchema;
    }

    out_bin_collection = {};

    out_bin_collection.date = parsed_date;
    out_bin_collection.collection_type = parsed_collection_type;

    return ParseResult::Success;
}

std::ostream& operator<<(std::ostream& stream, Date date) {
    // Must static cast to 32-bit int otherwise uint8_t is printed as a char
    stream << static_cast<int32_t>(date.year) << "-";
    stream << std::setfill('0') << std::setw(2) << static_cast<int32_t>(date.month) << "-"; 
    stream << std::setfill('0') << std::setw(2) << static_cast<int32_t>(date.day);

    return stream;
}

ParseResult parse_response(const std::basic_string_view<char> &response_body,
                           BinCollection &out_bin_collection_1,
                           BinCollection &out_bin_collection_2) {
    auto json = std::unique_ptr<cJSON, decltype(cJSON_free) *>{
        cJSON_ParseWithLength(response_body.data(), response_body.size()), cJSON_free};

    if (json.get() == nullptr) {
        const char *error_ptr = cJSON_GetErrorPtr();
        std::cerr << "Error parsing JSON: parse failure at " << error_ptr << "\n";
        return ParseResult::InvalidJson;
    }

    cJSON *collections = cJSON_GetObjectItem(json.get(), "Collections");
    if (!cJSON_IsArray(collections)) {
        std::cerr << "Error parsing JSON: $.Collections was not an array\n";
        return ParseResult::InvalidJsonSchema;
    }

    cJSON *first_collection = cJSON_GetArrayItem(collections, 0);
    if (!cJSON_IsObject(first_collection)) {
        std::cerr << "Error parsing JSON: $.Collections[0] was not an object\n";
        return ParseResult::InvalidJsonSchema;
    }

    ParseResult first_parse_result = parse_collection(first_collection, out_bin_collection_1);
    if (first_parse_result != ParseResult::Success) {
        std::cerr << "Failed to parse first collection\n";
        return first_parse_result;
    }

    cJSON *second_collection = cJSON_GetArrayItem(collections, 1);
    if (!cJSON_IsObject(first_collection)) {
        std::cerr << "Error parsing JSON: $.Collections[0] was not an object\n";
        return ParseResult::InvalidJsonSchema;
    }

    ParseResult second_parse_result = parse_collection(second_collection, out_bin_collection_2);
    if (second_parse_result != ParseResult::Success) {
        std::cerr << "Failed to parse second collection\n";
        return second_parse_result;
    }

    return ParseResult::Success;
}
