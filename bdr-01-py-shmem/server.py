#!/usr/bin/env python

from __future__ import annotations

import argparse
import sys
import time

from dictdb import Database


def main():
    # parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-n', '--memory-name',
        type=str,
        default='dictdb',
        help="shared memory and semaphore name")

    args = parser.parse_args()

    # host the database until asked to stop
    print("Starting")
    with Database(name=args.memory_name) as database:
        try:
            print("Running", end="")
            while True:
                print(".", end="")
                sys.stdout.flush()
                time.sleep(1)

        except KeyboardInterrupt:
            print("Quitting")


if __name__ == '__main__':
    main()
