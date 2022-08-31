from __future__ import annotations

import ctypes
import mmap
import typing as t

import posix_ipc

from .dictionary import Dictionary, Entry


class SharedResources:
    def __init__(self, name, owner: bool):
        self._name = name
        self._owner = owner

        self._memory = None
        self._buffer = None
        self._semaphore = None
        self._dictionary = None

    def open(self):
        # create or open shared memory segment
        self._memory = posix_ipc.SharedMemory(
            name=f'/{self._name}',
            flags=posix_ipc.O_CREX if self._owner else 0,
            size=ctypes.sizeof(Dictionary))

        # map shared memory segment to in-memory byte buffer
        # https://docs.python.org/3/library/mmap.html
        self._buffer = mmap.mmap(self._memory.fd, self._memory.size)

        # close shared memory segment file descriptor because we no longer need it
        # and now we don't have to clean it up later
        self._memory.close_fd()

        # create semaphore
        self._semaphore = posix_ipc.Semaphore(
            name=f'/{self._name}',
            flags=posix_ipc.O_CREX if self._owner else 0,
            initial_value=1)

        # create dictionary from mapped buffer
        self._dictionary = Dictionary.from_buffer(self._buffer)

    def close(self):
        # remove dictionary
        self._dictionary = None

        # close memory mapped buffer
        self._buffer.close()

        if self._owner:
            # delete semaphore and shared memory
            self._semaphore.unlink()
            self._memory.unlink()

        else:
            # close semaphore
            self._semaphore.close()


class Database(SharedResources):
    def __init__(self, name: str):
        super().__init__(name=name, owner=True)

    def __enter__(self) -> Database:
        self.open()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()


class DatabaseClient(SharedResources):
    def __init__(self, name: str):
        super().__init__(name=name, owner=False)

    def __enter__(self) -> DatabaseClient:
        self.open()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    @property
    def count(self) -> int:
        return self._dictionary.count

    def insert(self, words: t.Sequence[str]) -> int:
        count = 0
        with self._semaphore:
            for word in words:
                index = self._dictionary.insert(word)
                if index:
                    count += 1

        return count

    def search(self, words: t.Sequence[str]) -> int:
        count = 0
        with self._semaphore:
            for word in words:
                index = self._dictionary.search(word)
                if index:
                    count += 1

        return count

    def remove(self, words: t.Sequence[str]) -> int:
        count = 0
        with self._semaphore:
            for word in words:
                index = self._dictionary.remove(word)
                if index:
                    count += 1

        return count
