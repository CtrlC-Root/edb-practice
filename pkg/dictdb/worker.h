#pragma once

// C++ Standard Library
#include <memory>

// Intel TBB
#include <oneapi/tbb/concurrent_queue.h>

// DictDB
#include "data.h"


// Worker context.
typedef struct {
  std::shared_ptr<std::thread> thread;
  std::shared_ptr<dictdb_t> db;
  std::shared_ptr<tbb::concurrent_queue<int>> client_sockets;
  bool cancel;
} dictdb_worker_context_t;

// Worker entry point.
void worker(std::shared_ptr<dictdb_worker_context_t> context);
