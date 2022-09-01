from __future__ import annotations

import ctypes
import typing as t


LIMIT_WORD_COUNT = 1 * (10 ** 6)  # 2 million
LIMIT_WORD_SIZE = 32  # https://en.wikipedia.org/wiki/Longest_word_in_English
LIMIT_BUCKET_SIZE = 100


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
            buffer[index % 8] = sum([
                buffer[index % 8],
                letter_ascii,
                index,
                (letter_ascii >> shift_value)
            ]) % 256

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
        ('entries', Entry * (LIMIT_WORD_COUNT * 2))
    ]

    def insert(self, word: str) -> t.Optional[int]:
        """
        Insert the word and return it's index or None if no empty entries are
        found.
        """

        hash = Entry.hash_word(word)
        initial_index = hash % len(self.entries)
        for index in range(initial_index, initial_index + LIMIT_BUCKET_SIZE + 1):
            entry = self.entries[index % len(self.entries)]
            if entry.hash == hash and entry.word == word:
                return index

            elif entry.hash == 0:
                entry.hash = hash
                entry.word = word
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
            entry = self.entries[index % len(self.entries)]
            if entry.hash == hash and entry.word == word:
                return index

        return None

    def remove(self, word: str) -> t.Optional[int]:
        """
        Remove the word and return it's previous index or None if not found.
        """

        hash = Entry.hash_word(word)
        initial_index = hash % len(self.entries)
        for index in range(initial_index, initial_index + LIMIT_BUCKET_SIZE + 1):
            entry = self.entries[index % len(self.entries)]
            if entry.hash == hash and entry.word == word:
                entry.hash = 0
                self.count -= 1
                return index

        return None
