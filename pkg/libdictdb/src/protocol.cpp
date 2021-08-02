#include "protocol.h"

// C++ Standard Library
#include <cassert>


// Encode a request message into bytes.
void encode_request(std::vector<std::byte>& buffer, const dictdb_request_t& request) {
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
