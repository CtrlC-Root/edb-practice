#include "worker.h"

// XXX
#include <iostream>
#include <unistd.h>     // close

// DictDB
#include "operation.h"

// XXX
#define BUFFER_SIZE 2048


// XXX
void worker(std::shared_ptr<dictdb_worker_context_t> context) {
  ssize_t bytes;
  uint8_t buffer[BUFFER_SIZE];
  memset(&buffer, 0, BUFFER_SIZE);

  while (!context->cancel) {
    // https://man7.org/linux/man-pages/man2/read.2.html
    bytes = read(context->client_socket, buffer, BUFFER_SIZE);
    if (bytes == -1) {
      // TODO: handle error
      break;
    }

    if (bytes == 0) {
      // XXX: no more to read
      break;
    }

    buffer[bytes] = 0;

    dictdb_op_t operation;
    operation.type = OperationType{buffer[0]};
    operation.word = std::string((char*) &buffer[2], buffer[1]);

    std::cout << "recv(" << bytes << "): " << operation.word << std::endl;

    buffer[0] = 128;
    bytes = write(context->client_socket, (char*) &buffer, 1);
    if (bytes < 1) {
      std::cout << "partial write" << std::endl;
    }
  }

  // XXX
  close(context->client_socket);
}
