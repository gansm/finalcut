#!/bin/sh

$PREFIX="/usr"

case "$1" in
  "--help"|"help")
    echo "Usage: $0 {help|debug|release}"
    exit 1
    ;;
  "--debug"|"debug")
    ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -pedantic"
    ;;
  "--profile"|"profile")
    ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-pg -O0 -DDEBUG -W -Wall -pedantic"
    ;;
  "--release"|"release"|*)
    ./configure --prefix="$PREFIX"
    ;;
esac

make
# make install
