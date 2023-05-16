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
    ("recording", 'Define Self-delusion at Dictionary.Com', 'Define Self-delusion at Dictionary.Com'),
    ("recording", "Running up that hill (a deal with god) - 2018 Remaster", "Running up that hill (a deal with god)"),
    ("recording", "TO STAY ALIVE [Feat. SkullyOSkully]", "TO STAY ALIVE"),
    ("recording", "Amy Winehouse, Weird Al", "Amy Winehouse, Weird Al"),
    ("recording", "!!ESIRPRUS (Ver. 2)", "!!ESIRPRUS"),
    ("recording", '"...as others see us...": I. Henry VIII (1491-1547)', '"...as others see us...": I. Henry VIII'),
    ("recording", "Herbert West – Reanimator", "Herbert West"),  # en dash
    ("recording", "Philip the Chancelor: Clavus pungens — 2vv conductus", "Philip the Chancelor: Clavus pungens"), # em dash
    ("recording", '"Beckoning Darkness" ~ Menu', '"Beckoning Darkness"'),
    ("recording", 'Big Phat at Glen Echo, MD Contradance / CALLER: Ridge Kennedy', 'Big Phat at Glen Echo, MD Contradance'),
    ("recording", 'Kikagaku Moyo/幾何学模様', 'Kikagaku Moyo'),
    ("artist", "Amy Winehouse, Weird Al", "Amy Winehouse"),
    ("artist", "((( Punchline Paradise )))", "((( Punchline Paradise )))"),
    ("artist", "Babylumalotoroony and the Jerry Lewis Bone-A-Thons featuring Athena", "Babylumalotoroony and the Jerry Lewis Bone-A-Thons"),
    ("artist", "Wilhelm Schneider-Clauss & Heimersdorfer Kinderchor", "Wilhelm Schneider-Clauss"),
    ("artist", "Jim Foster with The Kountry Kut-Ups", "Jim Foster")

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
