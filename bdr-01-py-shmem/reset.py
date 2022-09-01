#!/usr/bin/env python

import posix_ipc


def main():
    name = 'dictdb'

    try:
        posix_ipc.SharedMemory(name=f'/{name}').unlink()
        print(f"Shared memory removed: {name}")

    except posix_ipc.ExistentialError:
        print(f"Shared memory not found: {name}")

    try:
        posix_ipc.Semaphore(name=f'/{name}').unlink()
        print(f"Semaphore removed: {name}")

    except posix_ipc.ExistentialError:
        print(f"Semaphore not found: {name}")


if __name__ == '__main__':
    main()
