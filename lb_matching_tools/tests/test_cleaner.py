import unittest
from lb_matching_tools.cleaner import MetadataCleaner


TEST_STRINGS = [
    ("recording", "Nothing fancy about this!", "Nothing fancy about this!"),
    ("recording", "Tabula Rasa (feat. Lorraine Weiss)", "Tabula Rasa"),
    ("recording", "For The Love feat. Amy True", "For The Love"),
    ("recording", "For The Love ft. Amy True", "For The Love"),
    ("recording", "For The Love ft Amy True", "For The Love"),
    ("recording", "Birds Without a Feather", "Birds Without a Feather"),
    ("recording", "Don't Give up - 2001 remaster", "Don't Give up"),
    ("recording", "Running up that hill (a deal with god) - 2018 Remaster", "Running up that hill (a deal with god)"),
    ("recording", "TO STAY ALIVE [Feat. SkullyOSkully]", "TO STAY ALIVE"),
    ("artist", "Amy Winehouse, Weird Al", "Amy Winehouse"),
    ("recording", "Amy Winehouse, Weird Al", "Amy Winehouse, Weird Al"),
]

def suite():
    suite = unittest.TestSuite()
    suite.addTests(TestMetadataCleaner(clean_type, input, output) for clean_type, input, output in TEST_STRINGS)
    return suite


class TestMetadataCleaner(unittest.TestCase):
    def __init__(self, clean_type, input, output):
        super(TestMetadataCleaner, self).__init__()
        self.input = input
        self.output = output
        self.clean_type = clean_type

    def runTest(self):
        cl = MetadataCleaner()
        if self.clean_type == "recording":
            self.assertEqual(cl.clean_recording(self.input), self.output)
        else:
            self.assertEqual(cl.clean_artist(self.input), self.output)
