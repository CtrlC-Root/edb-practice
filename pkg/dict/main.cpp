// C++ Standard Library
#include <string>
#include <iostream>
#include <vector>
#include <cstddef>      // to_integer
#include <span>         // as_bytes, span

// XXX
#include <sys/socket.h> // socket, AF_*?
#include <sys/un.h>     // sockaddr_un
#include <unistd.h>     // close

// DictDB
#include <libdictdb/protocol.h>


// Application entry point.
int main(int argc, char* argv[]) {
  // parse command line arguments
  std::vector<std::string> args(argv + 1, argv + argc);

  if (args.size() != 2) {
    std::cout << "Usage: dict <insert|search|delete> <word>" << std::endl;
    exit(EXIT_FAILURE);
  }

  dictdb_request_t request;
  request.word = args[1];

  if (request.word.size() > 254) {
    std::cerr << "word is too large" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (args[0] == "insert") {
    request.type = OperationType::INSERT;
  } else if (args[0] == "search") {
    request.type = OperationType::SEARCH;
  } else if (args[0] == "delete") {
    request.type = OperationType::DELETE;
  } else {
    std::cerr << "unknown command: " << args[0] << std::endl;
    exit(EXIT_FAILURE);
  }

  // XXX
  int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (client_socket == -1) {
    std::cerr << "failed to create socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  // XXX
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, "socket", sizeof(addr.sun_path) - 1);

  // XXX
  int rv = connect(client_socket, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
  if (rv == -1) {
    std::cerr << "failed to connect socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  // XXX
  std::vector<std::byte> buffer;
  encode_request(buffer, request);

  ssize_t bytes = write(client_socket, &buffer[0], buffer.size());
  if (bytes < static_cast<ssize_t>(buffer.size())) {
    std::cerr << "partial write" << std::endl;
  }

  // XXX
  buffer.resize(255);
  bytes = read(client_socket, &buffer[0], buffer.size());
  // if (bytes != static_cast<ssize_t>(buffer.size())) {
  //   std::cerr << "partial read" << std::endl;
  // }

  buffer.resize(bytes);
  dictdb_response_t response;
  decode_response(buffer, response);

  // XXX
  rv = close(client_socket);
  if (rv == -1) {
    // XXX: check return value, guessed about -1
    std::cerr << "failed to close socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  // successful exit
  switch (response.result) {
    case OperationResult::ERROR:
      std::cerr << "operation failed" << std::endl;
      exit(EXIT_FAILURE);

    case OperationResult::SUCCESS:
      exit(EXIT_SUCCESS);

    case OperationResult::FAILURE:
      exit(EXIT_FAILURE);
  }
}
