import pytest
from lb_matching_tools.cleaner import MetadataCleaner


ARTIST_TEST_STRINGS = [
    ("Amy Winehouse, Weird Al", "Amy Winehouse"),
    ("((( Punchline Paradise )))", "((( Punchline Paradise )))"),
    ('Dan No Ura 壇ノ浦の戦い', 'Dan No Ura'),
    ('4 Dan No Ura 壇ノ浦の戦い', '4 Dan No Ura'),
    ("Babylumalotoroony and the Jerry Lewis Bone-A-Thons featuring Athena", "Babylumalotoroony and the Jerry Lewis Bone-A-Thons"),
    ("Wilhelm Schneider-Clauss & Heimersdorfer Kinderchor", "Wilhelm Schneider-Clauss"),
    ("Jim Foster with The Kountry Kut-Ups", "Jim Foster")
]

RECORDING_TEST_STRINGS = [
    ("Nothing fancy about this!", "Nothing fancy about this!"),
    ("Tabula Rasa (feat. Lorraine Weiss)", "Tabula Rasa"),
    ("For The Love feat. Amy True", "For The Love"),
    ("For The Love ft. Amy True", "For The Love"),
    ("For The Love ft Amy True", "For The Love"),
    ("For The Lovefeat. Amy True", "For The Lovefeat. Amy True"),
    ("For The Love feat.Amy True", "For The Love"),
    ("Amoeba (raft boy)", "Amoeba (raft boy)"),
    ("Birds Without a Feather", "Birds Without a Feather"),
    ("Don't Give up - 2001 remaster", "Don't Give up"),
    ('Define Self-delusion at Dictionary.Com', 'Define Self-delusion at Dictionary.Com'),
    ("Running up that hill (a deal with god) - 2018 Remaster", "Running up that hill (a deal with god)"),
    ("TO STAY ALIVE [Feat. SkullyOSkully]", "TO STAY ALIVE"),
    ("Amy Winehouse, Weird Al", "Amy Winehouse, Weird Al"),
    ("!!ESIRPRUS (Ver. 2)", "!!ESIRPRUS"),
    ('"...as others see us...": I. Henry VIII (1491-1547)', '"...as others see us...": I. Henry VIII'),
    ("Herbert West – Reanimator", "Herbert West"),  # en dash
    ("Philip the Chancelor: Clavus pungens — 2vv conductus", "Philip the Chancelor: Clavus pungens"), # em dash
    ('"Beckoning Darkness" ~ Menu', '"Beckoning Darkness"'),
    ('Big Phat at Glen Echo, MD Contradance / CALLER: Ridge Kennedy', 'Big Phat at Glen Echo, MD Contradance'),
    # Trailing punctuation (or punctuation in general) can be discarded in a consecutive step
    ('Kikagaku Moyo/幾何学模様', 'Kikagaku Moyo/'),
    ('山地情歌 San-Di Love Song', 'San-Di Love Song'),
    ('Madness (DJ Gollum feat. DJ Cap Remix)', 'Madness (DJ Gollum feat. DJ Cap Remix)'),
    ('Other Place [Live]', 'Other Place'),
    ('Other Place {Live}', 'Other Place'),
    ('Other Place <Live>', 'Other Place'),
    ('Other Place [boo - hiss]', 'Other Place [boo - hiss]'),
    ('Other Place {boo - hiss}', 'Other Place {boo - hiss}'),
    ('Other Place <boo - hiss>', 'Other Place <boo - hiss>'),
    ('Other Place [2028 - remaster]', 'Other Place'),
    ('Other Place {2018 - release}', 'Other Place'),
    ('Other Place <1985 - tryout>', 'Other Place'),
    ('T - Ball Rag', 'T - Ball Rag'),
    (')--- ---x--- ---(', ')--- ---x--- ---('),
    ('[intentionally left blank]', '[intentionally left blank]'),
    ('Babylumalotoroony and the Jerry Lewis Bone-A-Thons featuring Athena', 'Babylumalotoroony and the Jerry Lewis Bone-A-Thons'),
    ("Guff (Johann Sebastian - Bach)", "Guff (Johann Sebastian - Bach)"), 
    ("-10 on the Care-Meter", "-10 on the Care-Meter"), 
    ("264 - Das Herz", "264 - Das Herz"),
    # This should not be split, because a / ~ - inside () [] {} <> should not be split
    ("(You're So Square) Baby I Don't Care (Movie Edit, 2013, Take 16/2021, Take 6) Binaural", 
     "(You're So Square) Baby I Don't Care (Movie Edit, 2013, Take 16/2021, Take 6) Binaural"),
    ("this random text (quite worthy!)", "this random text (quite worthy!)"),
    ("this random text (released 2018)", "this random text"),
    ("this random text (demo session)", "this random text"),
    ("this random text (tryout tape)", "this random text"),
    ("I Miss You (Howie B mix)", "I Miss You (Howie B mix)"),
    ("Run Jozi (Godly)", "Run Jozi (Godly)"),
    ("Όσο Και Να Σ' Αγαπάω (Υπ' Ευθύνη Μου)", "Όσο Και Να Σ' Αγαπάω (Υπ' Ευθύνη Μου)"),
    ("Όσο Και Να Σ' Αγαπάω (2018 remaster)", "Όσο Και Να Σ' Αγαπάω"),
    ("Όσο Και Να Σ' Αγαπάω - Remix 2023", "Όσο Και Να Σ' Αγαπάω"),
]

@pytest.mark.parametrize("test_input,expected", ARTIST_TEST_STRINGS)
def test_cleaner_artist(test_input, expected):
    cl = MetadataCleaner()
    assert cl.clean_artist(test_input) == expected

@pytest.mark.parametrize("test_input,expected", RECORDING_TEST_STRINGS)
def test_cleaner_recording(test_input, expected):
    cl = MetadataCleaner()
    assert cl.clean_recording(test_input) == expected
