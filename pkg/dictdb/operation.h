#pragma once

typedef enum class OperationType:uint8_t {
  PING = 0,
  INSERT = 1,
  SEARCH = 2,
  DELETE = 3
} dictdb_op_type_t;

typedef struct {
  dictdb_op_type_t type;
  std::string word;
} dictdb_op_t;
