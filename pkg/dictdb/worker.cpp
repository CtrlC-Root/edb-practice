#include "worker.h"

// POSIX
#include <unistd.h>     // close

// DictDB
#include <protocol.h>


// Worker entry point.
void worker(std::shared_ptr<dictdb_worker_context_t> context) {
  ssize_t bytes;
  std::vector<std::byte> buffer;

  while (!context->cancel) {
    // https://man7.org/linux/man-pages/man2/read.2.html
    buffer.resize(2048);
    bytes = read(context->client_socket, &buffer[0], buffer.size());
    if (bytes == -1) {
      // TODO: handle error
      break;
    }

    if (bytes == 0) {
      // XXX: no more to read
      break;
    }

    buffer.resize(bytes);

    dictdb_op_t operation;
    decode_operation(buffer, operation);

    // XXX
    uint8_t result = 0;

    switch (operation.type) {
      case OperationType::PING:
        result = 128;
        break;

      case OperationType::INSERT:
        context->db->words.insert(std::pair<std::string, bool>(operation.word, true));
        result = 1;
        break;

      case OperationType::SEARCH:
        result = context->db->words.count(operation.word);
        break;

      case OperationType::DELETE:
        context->db->words.erase(operation.word);
        result = 1;
        break;
    }

    // XXX
    bytes = write(context->client_socket, (char*) &result, 1);
    if (bytes < 1) {
      // TODO: handle error
      break;
    }
  }

  // XXX
  close(context->client_socket);
}
