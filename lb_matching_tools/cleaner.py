import re
import regex

from lb_matching_tools.guff import GUFF_PAREN_WORDS

SYMBOLS = "1234567890!@#$%^&*()-=_+[]{};\"|;'\\<>?/.,~`"


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

    def __init__(self, preferred_script="Latin"):
        self.recording_expressions = [re.compile(exp, re.IGNORECASE) for exp in self.RECORDING_EXPRESSIONS]
        self.artist_expressions = [re.compile(exp, re.IGNORECASE) for exp in self.ARTIST_EXPRESSIONS]
        self.foreign_script_expression = regex.compile(r"[^\p{Script=Common}\p{Script=" + preferred_script + r"}]+")
        self.paren_guff_expression = re.compile(r"(20[0-9]{2}|19[0-9]{2})")
        self.letter_expression = regex.compile(r"\p{Letter}")

    def drop_foreign_chars(self, text: str):
        remaining_text = regex.sub(self.foreign_script_expression, "", text).strip()
        # only return the remaining text if it still contains at least one letter
        if regex.search(self.letter_expression, remaining_text):
            return remaining_text
        else:
            return text

    def is_paren_text_likely_guff(self, paren_text):
        """
            Given some text found in a paren, guess to see if its guff (useless meta info) or relevant info for matching a track.
            Return True if the given text is considered guff.
        """

        paren_text = paren_text.lower()
        before_len = len(paren_text)

        # Remove guff words from text
        for guff in GUFF_PAREN_WORDS:
            paren_text = paren_text.replace(guff, "")

        # Check to see if we find a year (19## or 20##). If we do, remove it
        paren_text = self.paren_guff_expression.sub("", paren_text)
        replaced = before_len - len(paren_text)

        # Calculate the number of "characters" vs "symbols" for what is leftover
        chars = 0
        guff_chars = replaced
        for ch in paren_text.lower():

            if ch in SYMBOLS:
                guff_chars += 1

            if regex.match(self.letter_expression, ch):
                chars += 1

        if guff_chars <= chars:
            return False

        return True

    def paren_checker(self, text):
        """
            Check to see if the cleaned fragment contains a split paren (), {}, [], <>. If it does, return False, otherwise True.
        """

        if text.count("(") != text.count(")"):
            return False

        if text.count("<") != text.count(">"):
            return False

        if text.count("[") != text.count("]"):
            return False

        if text.count("{") != text.count("}"):
            return False

        return True

    def clean_recording(self, text: str):
        """
            Run the metadata cleaner against a recording name. Returns the cleaned string, which may be unchanged from the given string.
        """
        cleaned = ""
        for i, exp in enumerate(self.recording_expressions):
            m = exp.match(text)
            if m is not None:
                is_guff_text = False
                try:
                    paren_text = m.group('enclosed')
                    is_guff_text = self.is_paren_text_likely_guff(paren_text[1:-1])
                    if is_guff_text:
                        return m.groups()[0]
                    else:
                        continue
                except IndexError:
                    cleaned = m.groups()[0]

                # Check to make sure the regexes didn't produce crap
                # This is ugly.
                # Could be avoided by using a list of keywords which indicate crap and e.g. baking them into a lookahead expression.
                if i == 2:
                    if not hyphen_split_check(text, cleaned):
                        cleaned = text

                if not self.paren_checker(cleaned):
                    cleaned = text

        if not len(cleaned):
            cleaned = text

        return self.drop_foreign_chars(cleaned)

    def clean_artist(self, text: str):
        """
            Run the metadata cleaner against an artist name. Returns the cleaned string, which may be unchanged from the given string.
        """

        cleaned = self.clean_recording(text)
        if text != cleaned:
            return cleaned

        for i, exp in enumerate(self.artist_expressions):
            m = exp.match(text)
            if m is not None:
                return m.groups()[0]

        return text
