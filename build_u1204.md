To Build on ubuntu 12.04
========================

install the required build pacakges
-----------------------------------
```
apt-get install git build-essential automake liblua5.1-0-dev \
libboost1.48-all-dev libmysqld-dev libldap2-dev libtool pkg-config
```

Get a working copy of pdns source and the glbbackup configure.ac patch
----------------------------------------------------------------------
wget http://utils.rackexp.org/pdns-3.3.tar
wget http://utils.rackexp.org/glbbackend.patch

untar and apply the patch
-------------------------
tar -xvf pdns-3.3.tar
patch -p0 < glbbackend.patch

download the glbbackend code from github
----------------------------------------
cd pdns-3.3/modules
git clone https://github.com/lbrackspace/glbbackend.git

bootstrap and build the source
------------------------------
```
cd ..
./bootstrap
autoreconf -i
./configure --prefix="/opt/pdns" --with-modules="glb geo gmysql pipe random"
make
make install
```
