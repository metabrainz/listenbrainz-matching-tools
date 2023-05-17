Some scatchpad notes about this project
=======================================

TODO:

- A dash in the first third of a string should not split the string. (implemented but could be better)
- Never split inside brackets


These are the queries that collected the test data:

```
select distinct name into dirty_recordings from recording where name ~ ('[[*|()<>{}_'
                                                                    || U&'\2010'
                                                                    || U&'\2012'
                                                                    || U&'\2013'
                                                                    || U&'\2014'
                                                                    || '-]+')
                                                                    or name ~ '^[0-9]{1,2}\s{0,1}\.{1}\s+';
```

```
select distinct name into dirty_artists from artist where name ~ ('[[*|()<>{}_'
                                                                  || U&'\2010'
                                                                  || U&'\2012'
                                                                  || U&'\2013'
                                                                  || U&'\2014'
                                                                  || '-]+')
                                                                  or name ~ '^[0-9]{1,2}\s{0,1}\.{1}\s+';
```
