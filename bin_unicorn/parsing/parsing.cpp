#include <cassert>
#include <iostream>
#include <map>
#include <string>
extern "C"
{
#include "cJSON.h"
}
#include "date/date.h"

#include "parsing.hpp"

static const std::map<const std::basic_string_view<char>, CollectionType>
    collection_map = {{"Domestic Waste", DomesticWaste},
                      {"Food Waste", FoodWaste},
                      {"Recycling", Recycling},
                      {"Garden Waste", GardenWaste}};

static bool parse_collection_string(const std::basic_string_view<char> &service_string, CollectionType &out_collection_type)
{
    int first_word_end = service_string.find(" Collection Service");
    if (first_word_end == -1)
    {
        return false;
    }

    auto collection = collection_map.find(service_string.substr(0, first_word_end));
    if (collection == collection_map.end())
    {
        return false;
    }

    out_collection_type = collection->second;
    return true;
}

static bool parse_date(const std::string &date_time_string, date::year_month_day &out_date)
{
    std::istringstream date_stream(date_time_string);
    date_stream >> date::parse("%d/%m/%Y", out_date);

    return !date_stream.fail();
}

static ParseResult parse_collection(const cJSON *collection, BinCollection &out_bin_collection)
{
    cJSON *date = cJSON_GetObjectItem(collection, "Date");
    if (!cJSON_IsString(date))
    {
        std::cerr << "Error parsing JSON: $.Collections[0].Date was not a string";
        return ParseResult::InvalidJsonSchema;
    }

    cJSON *service = cJSON_GetObjectItem(collection, "Service");
    if (!cJSON_IsString(service))
    {
        std::cerr << "Error parsing JSON: $.Collections[0].Service was not a string";
        return ParseResult::InvalidJsonSchema;
    }

    date::year_month_day parsed_date;
    if (!parse_date(std::string(date->valuestring), parsed_date))
    {
        std::cerr << "Error parsing JSON: $.Collections[0].Date '" << date->valuestring << "' was not a valid date";
        return ParseResult::InvalidJsonSchema;
    }

    CollectionType parsed_collection_type;
    if (!parse_collection_string(std::string(service->valuestring), parsed_collection_type))
    {
        std::cerr << "Error parsing JSON: $.Collections[0].Service '" << service->valuestring << "' did not match expected format";
        return ParseResult::InvalidJsonSchema;
    }

    out_bin_collection = BinCollection{
        out_bin_collection.date = parsed_date,
        out_bin_collection.collection_type = parsed_collection_type,
    };
}

ParseResult parse_response(
    const std::basic_string_view<char> &response_body,
    BinCollection &out_bin_collection_1,
    BinCollection &out_bin_collection_2)
{
    auto wrapper = cJSONWrapper(cJSON_ParseWithLength(response_body.data(), response_body.size()));
    if (wrapper.json == nullptr)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        std::cerr << "Error parsing JSON: parse failure at " << error_ptr << "\n";
        return ParseResult::InvalidJson;
    }

    cJSON *collections = cJSON_GetObjectItem(wrapper.json, "Collections");
    if (!cJSON_IsArray(collections))
    {
        std::cerr << "Error parsing JSON: $.Collections was not an array";
        return ParseResult::InvalidJsonSchema;
    }

    cJSON *first_collection = cJSON_GetArrayItem(collections, 0);
    if (!cJSON_IsObject(first_collection))
    {
        std::cerr << "Error parsing JSON: $.Collections[0] was not an object";
        return ParseResult::InvalidJsonSchema;
    }

    ParseResult first_parse_result = parse_collection(first_collection, out_bin_collection_1);
    if (first_parse_result != ParseResult::Success)
    {
        std::cerr << "Failed to parse first collection";
        return first_parse_result;
    }

    cJSON *second_collection = cJSON_GetArrayItem(collections, 1);
    if (!cJSON_IsObject(first_collection))
    {
        std::cerr << "Error parsing JSON: $.Collections[0] was not an object";
        return ParseResult::InvalidJsonSchema;
    }

    ParseResult second_parse_result = parse_collection(second_collection, out_bin_collection_2);
    if (second_parse_result != ParseResult::Success)
    {
        std::cerr << "Failed to parse second collection";
        return second_parse_result;
    }

    return ParseResult::Success;
}
