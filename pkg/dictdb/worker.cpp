#include "worker.h"

// XXX
#include <iostream>
#include <unistd.h>     // close

// XXX
#define BUFFER_SIZE 2048


// XXX
void worker(std::shared_ptr<dictdb_worker_context_t> context) {
  ssize_t bytes;
  char buffer[BUFFER_SIZE];

  while (!context->cancel) {
    // TODO
    while ((bytes = read(context->client_socket, buffer, BUFFER_SIZE)) > 0) {
      buffer[bytes] = 0;
      std::cout << "recv: " << buffer << std::endl;
    }
  }

  // XXX
  close(context->client_socket);
}
