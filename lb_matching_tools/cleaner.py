import re
import regex

def hyphen_split_check(original, cleaned):
    if len(cleaned) < len(original) / 3:
        return False
    return True

class MetadataCleaner:

    RECORDING_EXPRESSIONS = [
        # This track is crap (2018 remaster)
        r"(?P<title>.+?)\s+(?P<enclosed>\(.+\)|\[.+\]|\{.+\}|\<.+\>)$",

        # Tabula Rasa (feat. Lorraine Weiss)
        # TO STAY ALIVE [Feat. SkullyOSkully]
        # For The Love feat. Amy True
        # For The Love ft. Amy True
        # For The Love ft Amy True
        # Birds Without a Feather -> Nothing!
        r"(?P<title>.+?)\s+?(?P<feat>[\[\(]?(?:feat(?:uring)?|ft)\b\.?)\s*?(?P<artists>.+)\s*",

        # Don't Give up - 2001 remaster
        r"(?P<title>.+?)(?:\s+?[\u2010\u2012\u2013\u2014~/-])(?![^(]*\))(?P<dash>.*)",
    ]

    ARTIST_EXPRESSIONS = [
        # Amy Winehouse, Weird Al Yankovic
        r"(?P<title>.+?)(?:\s*?,)(?P<comma>.*)",

        # Wilhelm Schneider Clauss with Heimersdorfer Kinderchor
        # Wilhelm Schneider Clauss & Heimersdorfer Kinderchor
        r"(?P<title>.+?)(?:\s+?(&|with))(?P<dash>.*)",
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
            m = exp.match(text)
            if m is not None:
                cleaned = m.groups()[0]

                # This is ugly.
                # Could be avoided by using a list of keywords which indicate crap and e.g. baking them into a lookahead expression.
                if i == 2:
                    if hyphen_split_check(text, cleaned):
                        return cleaned
                    else:
                        return text
                else:
                    return cleaned

        return text

    def clean_artist(self, text: str):

        cleaned = self.clean_recording(text)
        if text != cleaned:
            return cleaned

        for i, exp in enumerate(self.artist_expressions):
            m = exp.match(text)
            if m is not None:
                return m.groups()[0]

        return text
