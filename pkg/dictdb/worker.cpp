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
  memset(&buffer, 0, BUFFER_SIZE);

  while (!context->cancel) {
    // TODO
    bytes = read(context->client_socket, buffer, BUFFER_SIZE);
    if (bytes == 0) {
      break;
    }

    if (bytes < 3) {
      std::cout << "partial read" << std::endl;
      break;
    }

    buffer[bytes] = 0;
    std::cout << "recv(" << bytes << "): " << &buffer[2] << std::endl;

    uint8_t result = 128;
    bytes = write(context->client_socket, &result, 1);
  }

  // XXX
  close(context->client_socket);
}
