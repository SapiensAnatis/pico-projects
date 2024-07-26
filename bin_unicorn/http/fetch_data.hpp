#ifndef HTTP_FETCH_DATA_H_
#define HTTP_FETCH_DATA_H_

#include <string>
#include <vector>

/// @brief Fetch data on waste collection from Reading Borough Council.
/// @param buffer The buffer that the UTF-8 response should be written to. Will be demoted to a pointer in tls_client.c; ensure it has enough space reserved.
/// @param url_encoded_address The address to fetch collection data for. Must be URL-encoded.
/// @return A result code from the https_get request. 0 if successful. See tls_client.h https_get() documentation.
int8_t fetch_collection_data(std::string url_encoded_address, std::vector<char> &buffer);

#endif // HTTP_FETCH_DATA_H