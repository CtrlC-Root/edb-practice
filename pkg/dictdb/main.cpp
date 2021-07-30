// C++ standard library
#include <memory>       // unique_ptr
#include <string>
#include <iostream>     // cout, endl

// C standard library
#include <signal.h>     // sigaction, signal

// DictDB
#include "data.h"


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
  dictdb_t db;

  // process requests
  running = true;
  while (running) {
    // TODO
  }

  // successful exit
  exit(EXIT_SUCCESS);
}
