#!/bin/sh

#CXX="clang++"
PREFIX="/usr"

case "$1" in
  "--debug"|"debug")
    ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -pedantic"
    ;;

  "--fulldebug"|"fulldebug")
    ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -Weffc++ -pedantic -pedantic-errors -Wextra -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wpadded -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -fstack-protector -Wstrict-aliasing -Wstrict-aliasing=3 -Wswitch -Wswitch-enum -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wsign-promo -Woverloaded-virtual -Wstrict-null-sentinel -fext-numeric-literals -Wreorder -Wnoexcept -Wnarrowing -Wliteral-suffix -Wctor-dtor-privacy"
    ;;

  "--profile"|"profile")
    ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -pg -O0 -DDEBUG -W -Wall -pedantic"
    ;;

  "--cpu-profiler"|"cpu-profiler")
    ./configure --prefix="$PREFIX" --with-profiler
    ;;

  "--gcov"|"gcov")
    ./configure --prefix="$PREFIX" CXXFLAGS="-fprofile-arcs -ftest-coverage"
    ;;

  "--release"|"release")
    ./configure --prefix="$PREFIX"
    ;;

  "--help"|"help"|*)
    echo "Usage:"
    echo "  $(basename "$0") {COMMAND}"
    echo ""
    echo "Commands:"
    echo "  help          Show this help"
    echo "  debug         Compile with debug option"
    echo "  fulldebug     Compile with all warning options"
    echo "  profile       Compile with profile option (analysis with gprof)"
    echo "  cpu-profiler  Link with Google cpu performance profiler"
    echo "  gcov          Compile with options for coverage analysis with gcov"
    echo "  release       Compile for release"
    exit 1
    ;;
esac

make V=1 -j10
# make install
