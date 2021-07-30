// C++ Standard Library
#include <string>
#include <iostream>

// XXX
#include <sys/socket.h> // socket, AF_*?
#include <sys/un.h>     // sockaddr_un
#include <unistd.h>     // close

// XXX
#define BUFFER_SIZE 2048


// Application entry point.
int main(int argc, char* argv[]) {
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
  std::string message("hello, world!");

  ssize_t bytes = write(client_socket, message.c_str(), message.size());
  std::cout << message.size() << " / " << bytes << std::endl;

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
