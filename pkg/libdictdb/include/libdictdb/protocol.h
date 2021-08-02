#pragma once

// C++ Standard Library
#include <string>
#include <vector>


/**
 * Database operation type.
 */
typedef enum class OperationType:uint8_t {
  PING = 0,
  INSERT = 1,
  SEARCH = 2,
  DELETE = 3
} dictdb_op_type_t;

/**
 * Request message.
 */
typedef struct {
  dictdb_op_type_t type;
  std::string word;
} dictdb_request_t;

/**
 * Encode a request message into bytes.
 */
void encode_request(std::vector<std::byte>& buffer, const dictdb_request_t& request);

/**
 * Decode a request message from bytes.
 */
void decode_request(std::vector<std::byte>& buffer, dictdb_request_t& request);
