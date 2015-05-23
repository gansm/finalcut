#!/bin/sh

case "$1" in
  "--help"|"help")
    echo "Usage: $0 {help|debug|release}"
    exit 1
    ;;
  "--debug"|"debug")
    ./configure CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -pedantic"
    ;;
  "--profile"|"profile")
    ./configure CPPFLAGS="-DDEBUG" CXXFLAGS="-pg -O0 -DDEBUG -W -Wall -pedantic"
    ;;
  "--release"|"release"|*)
    ./configure
    ;;
esac

make
# make install
