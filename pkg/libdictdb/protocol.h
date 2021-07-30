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
 * Database operation.
 */
typedef struct {
  dictdb_op_type_t type;
  std::string word;
} dictdb_op_t;

/**
 * Encode an operation into bytes.
 */
void encode_operation(std::vector<std::byte>& buffer, const dictdb_op_t& operation);

/**
 * Decode an operation from bytes.
 */
void decode_operation(std::vector<std::byte>& buffer, dictdb_op_t& operation);
