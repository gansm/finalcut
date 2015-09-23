#!/bin/sh

PREFIX="/usr"

case "$1" in
  "--help"|"help")
    echo "Usage: $0 {help|debug|fulldebug|profile|gcov|release}"
    exit 1
    ;;
  "--debug"|"debug")
    ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -pedantic"
    ;;
  "--fulldebug"|"fulldebug")
    ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -Weffc++ -pedantic -pedantic-errors -Wextra -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wpadded -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wswitch-enum -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wsign-promo -Woverloaded-virtual -Wstrict-null-sentinel -fext-numeric-literals -Wreorder -Wnoexcept -Wnarrowing -Wliteral-suffix -Wctor-dtor-privacy"
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
