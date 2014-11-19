## libmysmartgrid - a C++ library for interfacing with the mySmartGrid API

  https://github.com/mysmartgrid/libmysmartgrid

This library is a C++ implementation of the mySmartGrid API as documented at
  http://developer.mysmartgrid.de/doku.php?id=webserviceinterface21
It is based on libcurl (http://curl.haxx.se/) and libjsoncpp (https://github.com/open-source-parsers/jsoncpp)

## Building libmysmartgrid

Libmysmartgrid depends on the following libraries:

1. Boost version 1.46 or later
2. libcurl version 7.19 or later
3. NaCl verson 1.0 or later
4. JsonCpp version 0.6.0 or later

On Ubuntu 12.04 LTS you can run the following command to install all dependencies

    sudo apt-get install libnacl-dev libviennacl-dev nacl-tools libcurl4-openssl-dev libboost1.48-dev

To build the code run the following lines from the source directory

    mkdir -p build
    cd build
    cmake ../
    make

To install the libmysmartgrid to /usr/local run

    make install

You can also change the install prefix by running

    cmake -DCMAKE_INSTALL_PREFIX=<prefix> ../

in a clean build directory.

If you prefer debian packages or tarballs run

    make release
