#include "worker.h"

// POSIX
#include <unistd.h>     // close

// DictDB
#include <libdictdb/protocol.h>


// Worker entry point.
void worker(std::shared_ptr<dictdb_worker_context_t> context) {
  ssize_t bytes;
  std::vector<std::byte> buffer;

  while (!context->cancel) {
    // retrieve the next client socket
    int client_socket;
    while (!context->client_sockets->try_pop(client_socket)) {
      if (context->cancel) {
        break;
      } else {
        // TODO: sleep a bit
      }
    }

    if (context->cancel) {
      break;
    }

    // https://man7.org/linux/man-pages/man2/read.2.html
    buffer.resize(2048);
    bytes = read(client_socket, &buffer[0], buffer.size());
    if (bytes == -1) {
      // TODO: handle error
      break;
    }

    if (bytes == 0) {
      // XXX: no more to read
      break;
    }

    buffer.resize(bytes);

    dictdb_request_t request;
    decode_request(buffer, request);

    // XXX
    uint8_t result = 0;

    switch (request.type) {
      case OperationType::PING:
        result = 128;
        break;

      case OperationType::INSERT:
        context->db->words.insert(std::pair<std::string, bool>(request.word, true));
        result = 1;
        break;

      case OperationType::SEARCH:
        result = context->db->words.count(request.word);
        break;

      case OperationType::DELETE:
        context->db->words.erase(request.word);
        result = 1;
        break;
    }

    // XXX
    bytes = write(client_socket, (char*) &result, 1);
    if (bytes < 1) {
      // TODO: handle error
      break;
    }

    // XXX
    close(client_socket);
  }
}
