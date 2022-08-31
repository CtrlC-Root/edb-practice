#!/usr/bin/env python

from __future__ import annotations

import argparse

from dictdb import DatabaseClient


def main():
    # parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-n', '--memory-name',
        type=str,
        default='dictdb',
        help="shared memory and semaphore name")

    args = parser.parse_args()

    # connect to the database
    with DatabaseClient(name=args.memory_name) as client:
        print(f"Count: {client.count}")


if __name__ == '__main__':
    main()
