# Mysql - Strike a Match
Strike A Match Mysql UDF library based on string matching algorithm by Simon White of Catalysoft.

[http://www.catalysoft.com/articles/StrikeAMatch.html](http://www.catalysoft.com/articles/StrikeAMatch.html)

###Steps

compile it:
> gcc -std=c++0x -fPIC -shared -o libstrike_a_match.so strike_a_match.cc

copy the libstrike_a_match.so to the mysql's plugin directory
> cp libstrike_a_match.so /usr/lib/mysql/plugin/

Log into mysql client and run
> mysql> CREATE FUNCTION strike_a_match RETURNS REAL SONAME 'libstrike_a_match.so';

###Sample Result


















