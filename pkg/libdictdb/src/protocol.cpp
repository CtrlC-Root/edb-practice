#include "protocol.h"

// C++ Standard Library
#include <cassert>    // assert
#include <limits>     // std::numeric_limits


// Encode a request message into bytes.
void encode_request(std::vector<std::byte>& buffer, const dictdb_request_t& request) {
  assert((2 + request.word.size()) < std::numeric_limits<uint8_t>::max());  // Message size

  buffer.clear(); // O(n) for n = buffer.size()
  buffer.reserve(2 + request.word.size()); // O(n) for n = buffer.size()
  buffer.push_back(static_cast<std::byte>(2 + request.word.size()));  // ~O(1)
  buffer.push_back(static_cast<std::byte>(request.type)); // ~O(1)

  // word.size() * ~O(1) -> O(n) for n = word.size()
  std::transform(
    request.word.begin(),
    request.word.end(),
    std::back_inserter(buffer),
    [](unsigned char c) { return std::byte{c}; });
}

// Decode a request message from bytes.
void decode_request(std::vector<std::byte>& buffer, dictdb_request_t& request) {
  assert(buffer.size() > 1);  // Size and Operation Type bytes
  assert(static_cast<uint8_t>(buffer[0]) == buffer.size()); // Message size

  request.type = OperationType{buffer[1]}; // O(1)
  request.word = std::string(
    reinterpret_cast<const char *>(&buffer[2]),
    static_cast<uint8_t>(buffer[0]) - 2); // O(n) for n = buffer[1]
}

// Encode a response message into bytes.
void encode_response(std::vector<std::byte>& buffer, const dictdb_response_t& response) {
  buffer.clear(); // O(n) for n = buffer.size()
  buffer.reserve(2);  // O(n) for n = buffer.size()
  buffer.push_back(static_cast<std::byte>(2));  // ~O(1)
  buffer.push_back(static_cast<std::byte>(response.result));  // ~(O1)
}

// Decode a response message from bytes.
void decode_response(std::vector<std::byte>& buffer, dictdb_response_t& response) {
  assert(buffer.size() == 2); // Size and Result bytes
  assert(static_cast<uint8_t>(buffer[0]) == buffer.size()); // Message size

  response.result = OperationResult{buffer[1]}; // O(1)
}
