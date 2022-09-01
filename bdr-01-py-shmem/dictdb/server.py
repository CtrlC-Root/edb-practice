#!/usr/bin/env python

from __future__ import annotations

import argparse
import signal
import sys
import time

from .database import Database


def main():
    # parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-n', '--memory-name',
        type=str,
        default='dictdb',
        help="shared memory and semaphore name")

    args = parser.parse_args()

    # handle signals
    stop_requested = False
    def handle_signal(signal_number, signal_frame):
        stop_requested = True

    signal.signal(signal.SIGINT, handle_signal)
    signal.signal(signal.SIGTERM, handle_signal)

    # create the database
    with Database(name=args.memory_name) as database:
        # keep running until asked to stop
        while not stop_requested:
            time.sleep(1)


if __name__ == '__main__':
    main()
