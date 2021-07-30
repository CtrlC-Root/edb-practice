#!/usr/bin/env python

import os
import time
import string
import random
import argparse
import tempfile
import subprocess
from multiprocessing.pool import Pool
from collections import namedtuple


ClientRun = namedtuple('ClientRun', ['runtime', 'returncode'])


def run_client(args):
    binary, command, word = args
    start_time = time.perf_counter_ns()
    process = subprocess.run(
        [binary, command, word],
        stdout=subprocess.DEVNULL)

    end_time = time.perf_counter_ns()
    return ClientRun(
        runtime=(end_time - start_time),
        returncode=process.returncode)


def main():
    # parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-c', '--concurrency',
        type=int,
        default=os.cpu_count(),
        help="worker processes")

    parser.add_argument(
        '-k', '--chunk-size',
        type=int,
        default=1000,
        help="process pool chunk size")

    parser.add_argument(
        '-i', '--initial-words',
        type=int,
        help="initial words to load")

    args = parser.parse_args()

    # locate binaries
    build_directory = os.path.join(os.getcwd(), 'build')
    server_binary = os.path.join(build_directory, 'pkg', 'dictdb', 'dictdb')
    client_binary = os.path.join(build_directory, 'pkg', 'dict', 'dict')

    # create a temporary working directory
    with tempfile.TemporaryDirectory(prefix='dictdb-') as temp_directory:
        os.chdir(temp_directory)

        # start the server
        print(">> Start server")
        server_process = subprocess.Popen([server_binary])
        time.sleep(1.0)

        if server_process.returncode:
            print("Failed!")
            return

        # create a process pool
        with Pool(processes=args.concurrency) as pool:
            # load initial words
            print(">> Generate initial words")
            words = set(
                ''.join(random.choices(string.ascii_letters + string.digits, k=12))
                for i in range(args.initial_words))

            print(">> Load initial words")
            start_time = time.perf_counter()
            initial_load_runs = pool.map(
                run_client,
                [(client_binary, 'insert', word) for word in words],
                chunksize=args.chunk_size)

            elapsed_time = round(time.perf_counter() - start_time, 4)
            print(f"{elapsed_time} seconds")

            # XXX
            print(">> Generating test operations")
            insert_words = set(
                ''.join(random.choices(string.ascii_letters + string.digits, k=16))
                for i in range(10000))

            search_words = random.sample(list(words), 10000)
            delete_words = random.sample(list(words - set(search_words)), 10000)

            client_inserts = [(client_binary, 'insert', word) for word in insert_words]
            client_deletes = [(client_binary, 'delete', word) for word in search_words]
            client_searches = [(client_binary, 'search', word) for word in delete_words]
            client_operations = client_inserts + client_deletes + client_searches
            random.shuffle(client_operations)

            print(">> Running benchmark")
            start_time = time.perf_counter()
            benchmark_runs = pool.map(
                run_client,
                client_operations,
                chunksize=args.chunk_size)

            elapsed_time = round(time.perf_counter() - start_time, 4)
            print(f"{elapsed_time} seconds")

        # stop the server
        print(">> Stop server")
        server_process.terminate()
        server_process.wait()


if __name__ == '__main__':
    main()
