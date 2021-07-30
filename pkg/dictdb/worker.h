#pragma once

// C++ Standard Library
#include <memory>

// DictDB
#include "data.h"


// Worker context.
typedef struct {
  std::shared_ptr<std::thread> thread;
  std::shared_ptr<dictdb_t> db;
  int client_socket;
  bool cancel;
} dictdb_worker_context_t;

// Worker entry point.
void worker(std::shared_ptr<dictdb_worker_context_t> context);
