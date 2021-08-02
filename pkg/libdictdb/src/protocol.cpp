#include "protocol.h"


// Encode an operation into a byte buffer.
void encode_operation(std::vector<std::byte>& buffer, const dictdb_op_t& operation) {
  // assert operation.word.size() > 0

  buffer.clear(); // O(n) for n = buffer.size()
  buffer.reserve(2 + operation.word.size()); // O(n) for n = buffer.size()

  buffer.push_back(static_cast<std::byte>(operation.type)); // ~O(1)
  buffer.push_back(static_cast<std::byte>(operation.word.size())); // ~O(1)

  // word.size() * ~O(1) -> O(n) for n = word.size()
  std::transform(
    operation.word.begin(),
    operation.word.end(),
    std::back_inserter(buffer),
    [](unsigned char c) { return std::byte{c}; });
}

// Decode an operation from bytes.
void decode_operation(std::vector<std::byte>& buffer, dictdb_op_t& operation) {
  // assert buffer.size() > 2

  operation.type = OperationType{buffer[0]}; // O(1)
  operation.word = std::string(
    reinterpret_cast<const char *>(&buffer[2]),
    static_cast<uint8_t>(buffer[1])); // O(n) for n = buffer[1]
}
