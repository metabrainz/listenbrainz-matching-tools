from lb_matching_tools.cleaner import MetadataCleaner

mc = MetadataCleaner()
print(mc.clean_recording("Tabula Rasa (feat. Lorraine Weiss)"))
print(mc.clean_artist("Amy Winhouse, Lorraine Weiss"))
