from __future__ import annotations

import ctypes
import mmap
import typing as t

import posix_ipc


LIMIT_WORD_COUNT = 10 ** 6  # 1 million
LIMIT_WORD_SIZE = 32  # https://en.wikipedia.org/wiki/Longest_word_in_English
LIMIT_BUCKET_SIZE = 10


# https://docs.python.org/3/library/ctypes.html#structures-and-unions
class Entry(ctypes.Structure):
    _fields_ = [
        ('hash', ctypes.c_uint64),
        # https://docs.python.org/3/library/ctypes.html#arrays
        ('data', ctypes.c_char * LIMIT_WORD_SIZE)
    ]

    @staticmethod
    def hash_word(word: str) -> int:
        """
        Hash function specialized to English words.

        https://stackoverflow.com/a/14010513/937006
        """

        buffer = bytearray(8 * [0x00])
        hash = ctypes.c_uint64.from_buffer(buffer)

        hash.value = len(word)
        for index, letter in enumerate(word):
            letter_ascii = ord(letter)
            shift_value = (hash.value // (index + 1)) % 5
            buffer[index % 8] += letter_ascii + \
                index + (letter_ascii >> shift_value)

        return hash.value

    @property
    def word(self) -> str:
        return self.data.decode('ascii')

    @word.setter
    def word(self, value: str):
        self.data = value.encode('ascii')


class Dictionary(ctypes.Structure):
    _fields_ = [
        ('count', ctypes.c_uint64),
        # https://docs.python.org/3/library/ctypes.html#arrays
        ('entries', Entry * LIMIT_WORD_COUNT)
    ]

    def insert(self, word: str) -> t.Optional[int]:
        """
        Insert the word and return it's index or None if no empty entries are
        found.
        """

        hash = Entry.hash_word(word)
        initial_index = hash % len(self.entries)
        for index in range(initial_index, initial_index + LIMIT_BUCKET_SIZE + 1):
            if self.entries[index].hash == hash:
                return index

            elif self.entries[index].hash == 0:
                self.entries[index].hash = hash
                self.entries[index].word = word
                self.count += 1
                return index

        return None

    def search(self, word: str) -> t.Optional[int]:
        """
        Search for the word and return it's index if found.
        """

        hash = Entry.hash_word(word)
        initial_index = hash % len(self.entries)
        for index in range(initial_index, initial_index + LIMIT_BUCKET_SIZE + 1):
            if self.entries[index].hash == hash:
                return index

        return None

    def remove(self, word: str) -> t.Optional[int]:
        """
        Remove the word and return it's previous index or None if not found.
        """

        hash = Entry.hash_word(word)
        initial_index = hash % len(self.entries)
        for index in range(initial_index, initial_index + LIMIT_BUCKET_SIZE + 1):
            if self.entries[index].hash == hash:
                self.entries[index].hash = 0
                self.count -= 1
                return index

        return None


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
