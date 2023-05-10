import unittest
from lb_matching_tools.cleaner import MetadataCleaner


TEST_STRINGS = [
    ("Tabula Rasa (feat. Lorraine Weiss)", "Tabula Rasa"),
    ("For The Love feat. Amy True", "For The Love"),
    ("For The Love ft. Amy True", "For The Love"),
    ("For The Love ft Amy True", "For The Love"),
    ("Birds Without a Feather", "Birds Without a Feather"),
    ("Don't Give up - 2001 remaster", "Don't Give up"),
    ("Running up that hill (a deal with god) - 2018 Remaster", "Running up that hill (a deal with god)"),
    ("TO STAY ALIVE [Feat. SkullyOSkully]", "TO STAY ALIVE")
]


def suite():
    suite = unittest.TestSuite()
    suite.addTests(TestMetadataCleaner(input, output) for input, output in TEST_STRINGS)
    return suite


class TestMetadataCleaner(unittest.TestCase):
    def __init__(self, input, output):
        super(TestMetadataCleaner, self).__init__()
        self.input = input
        self.output = output

    def runTest(self):
        cl = MetadataCleaner()
        self.assertEqual(cl.clean(self.input), self.output)
