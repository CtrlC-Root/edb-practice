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


// XXX
typedef enum class OperationType:uint8_t {
  PING = 0,
  INSERT = 1,
  SEARCH = 2,
  DELETE = 3
} dict_op_type_t;

// XXX
typedef struct {
  dict_op_type_t type;
  std::string word;
} dict_op_t;


// Application entry point.
int main(int argc, char* argv[]) {
  // parse command line arguments
  std::vector<std::string> args(argv + 1, argv + argc);

  if (args.size() != 2) {
    std::cout << "Usage: dict <insert|search|delete> <word>" << std::endl;
    exit(EXIT_FAILURE);
  }

  dict_op_t operation;
  operation.word = args[1];

  if (operation.word.size() > 254) {
    std::cerr << "word is too large" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (args[0] == "insert") {
    operation.type = OperationType::INSERT;
  } else if (args[0] == "search") {
    operation.type = OperationType::SEARCH;
  } else if (args[0] == "delete") {
    operation.type = OperationType::DELETE;
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
  buffer.push_back(static_cast<std::byte>(operation.type));
  buffer.push_back(static_cast<std::byte>(operation.word.size()));

  std::transform(
    operation.word.begin(),
    operation.word.end(),
    std::back_inserter(buffer),
    [](unsigned char c) { return std::byte{c}; });

  ssize_t bytes = write(client_socket, &buffer[0], buffer.size());
  if (bytes < buffer.size()) {
    std::cerr << "partial write" << std::endl;
  }

  std::byte result;
  bytes = read(client_socket, &result, 1);
  if (bytes != 1) {
    std::cerr << "partial read" << std::endl;
  }

  std::cout << static_cast<uint8_t>(result) << std::endl;

  // XXX
  rv = close(client_socket);
  if (rv == -1) {
    // XXX: check return value, guessed about -1
    std::cerr << "failed to close socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  // successful exit
  exit(EXIT_SUCCESS);
}
