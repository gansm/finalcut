#!/bin/sh

PREFIX="/usr"

case "$1" in
  "--help"|"help")
    echo "Usage: $0 {help|debug|profile|gcov|release}"
    exit 1
    ;;
  "--debug"|"debug")
    ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -pedantic"
    ;;
  "--profile"|"profile")
    ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-pg -O0 -DDEBUG -W -Wall -pedantic"
    ;;
  "--gcov"|"gcov")
    ./configure --prefix="$PREFIX" CXXFLAGS="-fprofile-arcs -ftest-coverage"
    ;;
  "--release"|"release"|*)
    ./configure --prefix="$PREFIX"
    ;;
esac

make
# make install
