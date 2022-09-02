# DictDB

An in-memory word dictionary.

## Architecture

The server process creates a POSIX shared memory segment to hold the dictionary database and a POSIX semaphore to guard insert/delete operations on it.

* Search can run concurrently to insert/remove but only one insert/remove can run at a time due to the shared semaphore. Possible improvements might be to use more fine-grained locking or switch to a lock-free hash table implementation.
* The hash function is chosen to do well for English ASCII encoded words and the default hash table bucket size is chosen to be fairly small for speed. It's possible for these assumptions to be invalid if the client is used with non-english words.

## Requirements

* Platform which supports POSIX IPC Shared Memory and Semaphores.
* Python 3.8+

## Quick Start

On macOS with Homebrew:

```bash
brew install python3
```

On Arch Linux with Pacman:

```bash
pacman -Syu python
```

## Quick Start

Create a Python virtual environment and install the package:

```bash
python -m venv env
source env/bin/activate
pip install -e .
```

Run the server:

```bash
dictdb --help
dictdb &  # run in background
```

Run the client to insert, search, and delete words:

```bash
dict --help
dict insert foo
dict search foo
dict delete foo
```

The client also supports executing a command with a sequence of words:

```bash
dict insert one two three
dict search foo bar one two
dict delete foo one three
```

## Benchmark

Run the benchmarking script:

```bash
./benchmark.py --help
./benchmark.py -w 12 -c 12 -k 1 -i 1000000 -r 100000    # single word per client command         
./benchmark.py -w 12 -c 12 -k 100 -i 1000000 -r 100000  # multiple words per client command
```

On macOS you may need to run the script by invoking it with the `python3`
binary or it will try to use the OS provided Python 2 interpreter instead.
