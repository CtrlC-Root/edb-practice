#pragma once

// Intel TBB
#include <oneapi/tbb/concurrent_hash_map.h>


// XXX
// https://spec.oneapi.io/versions/latest/elements/oneTBB/source/containers/concurrent_hash_map_cls.html
typedef tbb::concurrent_hash_map<std::string, bool> dictdb_word_map_t;

// XXX
typedef struct {
  dictdb_word_map_t words;
} dictdb_t;

// // XXX
// typedef enum class AsyncState {
//   INIT,
//   RECV,
//   SEND
// } dictdb_aio_state_t;

// // XXX
// typedef enum struct OperationType:uint8_t {
//   PING = 0,
//   INSERT = 1,
//   SEARCH = 2,
//   DELETE = 3
// } dictdb_op_type_t;
//
// // XXX
// typedef struct {
//   dictdb_op_type_t type;
//   std::string data;
// } dictdb_op_request_t;
//
// // XXX
// typedef struct {
//   uint8_t result;
// } dictdb_op_response_t;
