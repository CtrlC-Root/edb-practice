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

    // receive and decode the request message
    // https://man7.org/linux/man-pages/man2/read.2.html
    buffer.resize(255); // maximum message size is one byte
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

    // process the request operation and generate a response
    dictdb_response_t response;
    switch (request.type) {
      case OperationType::PING: {
        response.result = OperationResult::SUCCESS;
        break;
      }

      case OperationType::INSERT: {
        auto value = std::pair<std::string, bool>(request.word, true);
        response.result = context->db->words.insert(value) ?
          OperationResult::SUCCESS :
          OperationResult::FAILURE;

        break;
      }

      case OperationType::SEARCH: {
        auto count = context->db->words.count(request.word);
        response.result = (count > 0) ?
          OperationResult::SUCCESS :
          OperationResult::FAILURE;

        break;
      }

      case OperationType::DELETE: {
        response.result = context->db->words.erase(request.word) ?
          OperationResult::SUCCESS :
          OperationResult::FAILURE;

        break;
      }
    }

    // encode and send the response message
    encode_response(buffer, response);
    bytes = write(client_socket, (char*) &buffer[0], buffer.size());
    if (bytes < static_cast<ssize_t>(buffer.size())) {
      // TODO: handle error
      break;
    }

    // XXX
    close(client_socket);
  }
}
