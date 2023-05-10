import re


class MetadataCleaner:

    EXPRESSIONS = [
        # Track name (guff #1) (guff #2)
        # Tabula Rasa (feat. Lorraine Weiss)
        # TO STAY ALIVE [Feat. SkullyOSkully]
        r"(?P<title>.+?)\s*?(?P<feat>(?:\[|\()?(?:feat(?:uring)?|ft)(?=\b)\.?)\s*?(?P<artists>.+)",

        # For The Love feat. Amy True
        # For The Love ft. Amy True
        # For The Love ft Amy True
        # Birds Without a Feather -> Nothing!
        r"\s*?(?P<title>.+?)\s*?(?P<feat>\(?(?:feat(?:uring)?|ft)(?=\b)\.?)\s*?(?P<artists>.+)\s*",

        # Don't Give up - 2001 remaster
        r"\s*?(?P<title>.+?)(?:\s*?-)(?P<dash>.*)",
    ]

    def __init__(self):
        self.compiled_expressions = [re.compile(exp, re.IGNORECASE) for exp in self.EXPRESSIONS]

    def clean(self, text: str):

        for i, exp in enumerate(self.compiled_expressions):
            m = self.compiled_expressions[i].match(text)
            if m is not None:
                return m.groups()[0]

        return text
