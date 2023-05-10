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
from a metadata string:

   "Tabula Rasa (feat. Lorraine Weiss)" -> "Tabula Rasa"       
   "Don't Give up - 2001 remaster" -> "Don't Give up"

When a piece of metadata fails to be identified the metadata should be run through the
metadata cleaner in an effort to remove this useless gunk that might be preventing a match.
If the cleaner sucessfully removes gunk, the cleaned metadata should be looked up again, hoping
that a match will be found this time.
