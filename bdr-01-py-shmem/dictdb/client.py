#!/usr/bin/env python

from __future__ import annotations

import argparse
import sys

from .database import DatabaseClient


def main():
    # parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-n', '--memory-name',
        type=str,
        default='dictdb',
        help="shared memory and semaphore name")

    command_group = parser.add_mutually_exclusive_group(required=True)
    command_group.add_argument(
        '-i', '--insert',
        dest='command',
        action='store_const',
        const='insert')

    command_group.add_argument(
        '-s', '--search',
        dest='command',
        action='store_const',
        const='search')

    command_group.add_argument(
        '-r', '--remove',
        dest='command',
        action='store_const',
        const='remove')

    parser.add_argument(
        'word',
        nargs='+',
        help="word")

    args = parser.parse_args()

    # connect to the database
    count = None
    with DatabaseClient(name=args.memory_name) as client:
        # run the appropriate command
        if args.command == 'insert':
            count = client.insert(words=args.word)

        elif args.command == 'search':
            count = client.search(words=args.word)

        elif args.command == 'remove':
            count = client.remove(words=args.word)

    # exit with an exit code based on the response
    assert isinstance(count, int), "invalid response"
    sys.exit(0 if count == len(args.word) else 1)


if __name__ == '__main__':
    main()
