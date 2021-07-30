// C++ standard library
#include <memory>       // shared_ptr, unique_ptr
#include <string>
#include <iostream>     // cout, endl
#include <vector>

// C standard library
#include <signal.h>     // sigaction, signal

// XXX
#include <sys/socket.h> // socket, AF_*?
#include <sys/un.h>     // sockaddr_un
#include <unistd.h>     // close

// DictDB
#include "data.h"
#include "worker.h"


// Flag to indicate if the process should continue running.
bool running = false;

// Process signals.
void handle_signal(int sig) {
  switch (sig) {
    // sent by process manager to indicate we should reload
    case SIGHUP:
      // XXX: reload daemon configuration files and reopen file descriptors
      break;

    case SIGTERM:
    case SIGINT:
      running = false;
      break;
  }
}

// Application entry point.
int main(int argc, char* argv[]) {
  // register signal handler for relevant signals
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handle_signal;

  if (sigaction(SIGHUP, &sa, NULL) < 0) {
    std::cerr << "failed to install SIGHUP signal handler" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGTERM, &sa, NULL) < 0) {
    std::cerr << "failed to install SIGTERM signal handler"  << std::endl;
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGINT, &sa, NULL) < 0) {
    std::cerr << "failed to install SIGINT signal handler"  << std::endl;
    exit(EXIT_FAILURE);
  }

  // initialize the database
  std::shared_ptr<dictdb_t> db = std::make_shared<dictdb_t>();

  // XXX
  int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (server_socket == -1) {
    std::cerr << "failed to create socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, "socket", sizeof(addr.sun_path) - 1);

  int rv = bind(server_socket, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
  if (rv == -1) {
    std::cerr << "failed to bind socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  rv = listen(server_socket, 32); // XXX: backlog size?
  if (rv == -1) {
    std::cerr << "failed to listen on socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  // XXX
  std::vector<std::shared_ptr<dictdb_worker_context_t>> workers;

  // process requests
  running = true;
  while (running) {
    // TODO
    int client_socket = accept(server_socket, NULL, NULL);

    // XXX
    auto context = std::make_shared<dictdb_worker_context_t>();
    context->db = db;
    context->client_socket = client_socket;
    context->cancel = false;
    context->thread = std::make_shared<std::thread>(worker, context);

    workers.push_back(context);
  }

  // XXX
  for (auto context : workers) {
    context->cancel = true;
    context->thread->join();
  }

  // XXX
  rv = close(server_socket);
  if (rv == -1) {
    // XXX: check return value, guessed about -1
    std::cerr << "failed to close server socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  // XXX: remove socket file
  rv = remove("socket");
  if (rv == -1) {
    std::cerr << "failed to remove socket file" << std::endl;
    exit(EXIT_FAILURE);
  }

  // successful exit
  exit(EXIT_SUCCESS);
}
