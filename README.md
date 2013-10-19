glbbackend
==========

A custom backend for PowerDNS that does global load balancing.


INSTALLATION

1. Download PowerDNS Source: http://downloads.powerdns.com/releases/pdns-3.3.tar.gz
2. git clone this repo into the modules directory.
3. cd to the source code's root directory
4. edit configure.ac
   Add this " modules/glbbackend/Makefile" without quotes at the end of the file inside the last parentheses
4. ./configure --with-modules="glb"
5. make
6. sudo make install
7. sudo echo "launch=glb" >> /usr/local/etc/pdns.conf
8. sudo pdns/pdns stop
9. sudo pdns/pdns start

DEBUG

Set these environment variables, then run the configure, make commands.

export CFLAGS="-O0 -ggdb -Wall"

export CXXFLAGS="-O0 -ggdb -Wall"

export LDFLAGS="-O0 -ggdb -Wall"

