import re
import regex


class MetadataCleaner:

    RECORDING_EXPRESSIONS = [
        # Track name (guff #1) (guff #2)
        # Tabula Rasa (feat. Lorraine Weiss)
        # TO STAY ALIVE [Feat. SkullyOSkully]
        r"(?P<title>.+?)\s*?(?P<feat>(?:\[|\()?(?:feat(?:uring)?|ft)(?=\b)\.?)\s*?(?P<artists>.+)",

        # For The Love feat. Amy True
        # For The Love ft. Amy True
        # For The Love ft Amy True
        # Birds Without a Feather -> Nothing!
        r"\s*?(?P<title>.+?)\s*?(?P<feat>\(?(?:feat(?:uring)?|ft)(?=\b)\.?)\s*?(?P<artists>.+)\s*",

        # This track is crap (2018 remaster)
        r"\s*?(?P<title>[^(]+?)(?:\s*?\()(?P<dash>.*)\)$",

        # Don't Give up - 2001 remaster
        r"\s*?(?P<title>.+?)(?:\s+?[\u2010\u2012\u2013\u2014~/-])(?P<dash>.*)",

        # Kikagaku Moyo/幾何学模様
        r"\s*?(?P<title>.+?)(?:\s*?[~/])(?P<dash>.*)",
    ]

    ARTIST_EXPRESSIONS = [
        # Amy Winehouse, Weird Al Yankovic
        r"\s*?(?P<title>.+?)(?:\s*?,)(?P<comma>.*)",
    ]

    def __init__(self, preferred_script = "Latin"):
        self.recording_expressions = [re.compile(exp, re.IGNORECASE) for exp in self.RECORDING_EXPRESSIONS]
        self.artist_expressions = [re.compile(exp, re.IGNORECASE) for exp in self.ARTIST_EXPRESSIONS]
        self.foreign_script_expression = regex.compile(r"[^\p{Script=Common}\p{Script=" + preferred_script + r"}]+")

    def drop_foreign_chars(self, text: str):
        return regex.sub(self.foreign_script_expression, "", text).strip()

    def clean_recording(self, text: str):
        text = self.drop_foreign_chars(text)

        for i, exp in enumerate(self.recording_expressions):
            m = self.recording_expressions[i].match(text)
            if m is not None:
                return m.groups()[0]

        return text

    def clean_artist(self, text: str):

        cleaned = self.clean_recording(text)
        if text != cleaned:
            return cleaned

        for i, exp in enumerate(self.artist_expressions):
            m = self.artist_expressions[i].match(text)
            if m is not None:
                return m.groups()[0]

        return text
