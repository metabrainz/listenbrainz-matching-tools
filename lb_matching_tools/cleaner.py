import re
import regex

WORD_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyzªµºÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþÿ"
SYMBOLS = "1234567890!@#$%^&*()-=_+[]{};\"|;'\\<>?/.,~`"
GUFF_PAREN_WORDS = [
    "a_cappella",
    "acoustic",
    "bonus",  
    "censored",
    "clean",
    "club",                                                                     
    "clubmix",
    "composition",
    "cut",
    "dance",
    "demo",
    "dialogue",
    "dirty",                                  
    "edit",
    "excerpt",
    "explicit",
    "extended",
    "instrumental",
    "interlude",
    "intro",
    "karaoke",
    "live",
    "long",                   
    "main",
    "maxi",
    "megamix",
    "mix",
    "mono",
    "official",
    "orchestral",
    "original",
    "outro",
    "outtake",
    "outtakes",
    "piano",
    "quadraphonic",
    "radio",
    "rap",
    "re_edit",
    "reedit",
    "refix",
    "rehearsal",
    "reinterpreted",
    "release"
    "released"
    "remake",
    "remastered",
    "remaster",
    "master",  
    "remix",
    "remixed",
    "remode",
    "reprise",                                 
    "rework",
    "reworked",
    "rmx",
    "session",
    "short",
    "single",
    "skit",
    "stereo",
    "studio",
    "take",
    "takes",
    "tape",
    "track",
    "uncensored",
    "unknown",
    "unplugged",
    "untitled",
    "version",
    "video",
    "vocal",
    "vs",                             
    "with",
    "without"
]


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
        r"(?P<title>.+?)(?:\s+?[\u2010\u2012\u2013\u2014~/-])(?P<dash>.*)",

        # Kikagaku Moyo/幾何学模様
        r"(?P<title>.+?)(?:\s*?[~/])(?P<dash>.*)",
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
        self.paren_guff_expression = regex.compile(r"(20[0-9]{2}|19[0-9]{2})")

    def drop_foreign_chars(self, text: str):
        return regex.sub(self.foreign_script_expression, "", text).strip()

    def is_paren_text_likely_guff(self, paren_text):
        
        paren_text = paren_text.lower()
        
        print(f"start: {paren_text}") 
        
        before_len = len(paren_text)
    
        # Remove guff words from text 
        for guff in GUFF_PAREN_WORDS:
            paren_text = paren_text.replace(guff, "")

        print(f"  mid: {paren_text}") 
    
        # Check to see if we find a year (19## or 20##). If we do, remove it    
        paren_text = self.paren_guff_expression.sub("", paren_text)

        replaced = before_len - len(paren_text)
        
        print(f"  end: {paren_text} (replaced {replaced})")        
    
        # Calculate the number of "characters" vs "symbols" for what is leftover
        chars = 0
        guff_chars = replaced 
        for ch in paren_text.lower():

            if ch in SYMBOLS:
                guff_chars += 1

            # Does anyone how to include non-ascii word characters in python?
            # \p{Letter} !!
            if ch in WORD_CHARS:
                chars += 1

        if guff_chars >= chars:
            return True

        return False


    def clean_recording(self, text: str):
        text = self.drop_foreign_chars(text)

        for i, exp in enumerate(self.recording_expressions):
            m = self.recording_expressions[i].match(text)
            if m is not None:
                is_guff_text = False
                try:
                    paren_text = m.group('enclosed')
                    is_guff_text = self.is_paren_text_likely_guff(paren_text[1:-1])
                    if is_guff_text:
                        print(f"text {paren_text} -> {is_guff_text}")
                        continue
                except IndexError:
                    pass

                cleaned = m.groups()[0]

                # This is ugly.
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
            m = self.artist_expressions[i].match(text)
            if m is not None:
                return m.groups()[0]

        return text


