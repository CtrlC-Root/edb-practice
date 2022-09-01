import random
import string
import typing as t
import unittest

from .dictionary import LIMIT_WORD_COUNT, LIMIT_WORD_SIZE
from .dictionary import Entry, Dictionary


class DictionaryTestCase(unittest.TestCase):
    @staticmethod
    def generate_random_words() -> t.List[str]:
        alphabet = string.ascii_letters + string.digits
        words = set()
        while len(words) < LIMIT_WORD_COUNT:
            word_size = random.randint(4, LIMIT_WORD_SIZE)
            word = ''.join(random.choices(alphabet, k=word_size))
            words.add(word)

        return list(words)

    @staticmethod
    def load_dictionary_words() -> t.List[str]:
        words = set()
        with open('/usr/share/dict/words', 'r') as input_file:
            for line in input_file.readlines():
                words.add(line.strip())

        return list(words)

    def setUp(self):
        self.words = DictionaryTestCase.generate_random_words()

    def test_insert_all(self):
        dictionary = Dictionary()
        for word in self.words:
            index = dictionary.insert(word)
            self.assertTrue(index is not None, "failed to insert word")

        self.assertEqual(
            dictionary.count,
            len(self.words),
            "dictionary word count does not match inserted word count")

    def test_basic(self):
        dictionary = Dictionary()
        self.assertEqual(dictionary.count, 0, "dictionary is not empty")

        # choose a few sample words randomly
        sample_words = random.choices(self.words, k=32)

        # insert the words
        for word in sample_words:
            index = dictionary.insert(word)
            self.assertTrue(index is not None, "failed to insert word")

        # verify the dictionary has the right number of words
        self.assertEqual(
            dictionary.count,
            len(sample_words),
            "dictionary is missing words")

        # search for the words and verify they are found
        for word in sample_words:
            index = dictionary.search(word)
            self.assertTrue(index is not None, "failed to find word")

        # remove the words
        for word in sample_words:
            index = dictionary.remove(word)
            self.assertTrue(index is not None, "failed to remove word")

        # verify the dictionary is empty
        self.assertEqual(dictionary.count, 0, "dictionary is not empty")

        # search for the words and verify they are not found
        for word in sample_words:
            index = dictionary.search(word)
            self.assertTrue(index is None, "found word after removing it")

    def test_hash_distribution(self):
        hashes = set()
        for word in self.words:
            hashes.add(Entry.hash_word(word))

        ratio = len(hashes) / len(self.words)
        self.assertTrue(
            ratio > 0.8,
            "hash function distribution is too narrow")
