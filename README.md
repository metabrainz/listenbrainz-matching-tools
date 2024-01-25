ListenBrainz matching tools
===========================

In the ListenBrainz project we've created a number of tools and techniques that allowed us to 
create the MBID Mapping sub-project. This project takes an artist name and a recording (track)
name and attempts to find the best matching recording in MusicBrainz.

This library will contain code to facilitate matching non-MusicBrainz metadata to MusicBrainz
(tag a music collection) and from MusicBrainz to non-MusicBrainz metadata (content resolution).

Metadata cleaner
----------------

One component in this toolkit is the metadata cleaner, which removes (often) useless gunk
from a metadata recording string:

   "Tabula Rasa (feat. Lorraine Weiss)" -> "Tabula Rasa"       
   "Don't Give up - 2001 remaster" -> "Don't Give up"

This library also supports artist name cleaning:

   "Amy Winhouse, Lorraine Weiss" -> "Amy Winehouse"

When a piece of metadata fails to be identified the metadata should be run through the
metadata cleaner in an effort to remove this useless gunk that might be preventing a match.
If the cleaner sucessfully removes gunk, the cleaned metadata should be looked up again, hoping
that a match will be found this time.

To use this class, instantiate the MetadataCleaner class and call the clean_recording or
clean_artist methods:

```
from lb_matching_tools.cleaner import MetadataCleaner

mc = MetadataCleaner()
mc.clean_recording("Tabula Rasa (feat. Lorraine Weiss)")
mc.clean_artist("Amy Winhouse, Lorraine Weiss")
```

Installation
------------

To install this python module, simply do:

```
pip install .
```

Development
-----------

Create virtual environment

```
python -m venv .venv
```

Install dependencies

```
pip install .
```

Running tests
-------------

To run the internal unit tests, run:

```
pip install .
./test.py
```

