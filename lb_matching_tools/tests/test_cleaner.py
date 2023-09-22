import unittest
from lb_matching_tools.cleaner import MetadataCleaner


TEST_STRINGS = [
    ("recording", "Nothing fancy about this!", "Nothing fancy about this!"),
    ("recording", "Tabula Rasa (feat. Lorraine Weiss)", "Tabula Rasa"),
    ("recording", "For The Love feat. Amy True", "For The Love"),
    ("recording", "For The Love ft. Amy True", "For The Love"),
    ("recording", "For The Love ft Amy True", "For The Love"),
    ("recording", "For The Lovefeat. Amy True", "For The Lovefeat. Amy True"),
    ("recording", "For The Love feat.Amy True", "For The Love"),
    ("recording", "Amoeba (raft boy)", "Amoeba (raft boy)"),
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
    # Trailing punctuation (or punctuation in general) can be discarded in a consecutive step
    ("recording", 'Kikagaku Moyo/幾何学模様', 'Kikagaku Moyo/'),
    ("recording", '山地情歌 San-Di Love Song', 'San-Di Love Song'),
    ("recording", 'Madness (DJ Gollum feat. DJ Cap Remix)', 'Madness (DJ Gollum feat. DJ Cap Remix)'),
    ("recording", 'Other Place [Live]', 'Other Place'),
    ("recording", 'Other Place {Live}', 'Other Place'),
    ("recording", 'Other Place <Live>', 'Other Place'),
    ("recording", 'Other Place [boo - hiss]', 'Other Place [boo - hiss]'),
    ("recording", 'Other Place {boo - hiss}', 'Other Place {boo - hiss}'),
    ("recording", 'Other Place <boo - hiss>', 'Other Place <boo - hiss>'),
    ("recording", 'Other Place [2028 - remaster]', 'Other Place'),
    ("recording", 'Other Place {2018 - release}', 'Other Place'),
    ("recording", 'Other Place <1985 - tryout>', 'Other Place'),
    ("recording", 'T - Ball Rag', 'T - Ball Rag'),
    ("recording", ')--- ---x--- ---(', ')--- ---x--- ---('),
    ("recording", '[intentionally left blank]', '[intentionally left blank]'),
    ("recording", 'Babylumalotoroony and the Jerry Lewis Bone-A-Thons featuring Athena', 'Babylumalotoroony and the Jerry Lewis Bone-A-Thons'),
    ("recording", "Guff (Johann Sebastian - Bach)", "Guff (Johann Sebastian - Bach)"), 
    ("recording", "-10 on the Care-Meter", "-10 on the Care-Meter"), 
    ("recording", "264 - Das Herz", "264 - Das Herz"),
    # This should not be split, because a / ~ - inside () [] {} <> should not be split
    ("recording", "(You're So Square) Baby I Don't Care (Movie Edit, 2013, Take 16/2021, Take 6) Binaural", 
                  "(You're So Square) Baby I Don't Care (Movie Edit, 2013, Take 16/2021, Take 6) Binaural"),
    ("recording", "this random text (quite worthy!)", "this random text (quite worthy!)"),
    ("recording", "this random text (released 2018)", "this random text"),
    ("recording", "this random text (demo session)", "this random text"),
    ("recording", "this random text (tryout tape)", "this random text"),
    ("recording", "I Miss You (Howie B mix)", "I Miss You (Howie B mix)"),
    ("recording", "Run Jozi (Godly)", "Run Jozi (Godly)"),
    ("recording", "Όσο Και Να Σ' Αγαπάω (Υπ' Ευθύνη Μου)", "Όσο Και Να Σ' Αγαπάω (Υπ' Ευθύνη Μου)"),
    ("recording", "Όσο Και Να Σ' Αγαπάω (2018 remaster)", "Όσο Και Να Σ' Αγαπάω"),
    ("recording", "Όσο Και Να Σ' Αγαπάω - Remix 2023", "Όσο Και Να Σ' Αγαπάω"),

    ("artist", "Amy Winehouse, Weird Al", "Amy Winehouse"),
    ("artist", "((( Punchline Paradise )))", "((( Punchline Paradise )))"),
    ("artist", 'Dan No Ura 壇ノ浦の戦い', 'Dan No Ura'),
    ("artist", '4 Dan No Ura 壇ノ浦の戦い', '4 Dan No Ura'),
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
