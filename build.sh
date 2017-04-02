#!/bin/sh

#CXX="clang++"
PREFIX="/usr"

# Get number of logical processor cores
if command -v getconf >/dev/null 2>&1
then
  CPU_COUNT="$(getconf _NPROCESSORS_ONLN 2>/dev/null || getconf NPROCESSORS_ONLN 2>/dev/null)" || CPU_COUNT="0"
fi

if [ "$CPU_COUNT" -eq 0 ]
then
  if command -v nproc >/dev/null 2>&1
  then
    CPU_COUNT="$(nproc 2>/dev/null)" || CPU_COUNT="0" 
  fi
fi

test "$CPU_COUNT" -eq 0 && CPU_COUNT=1

# Build commands
case "$1" in
  "--release"|"release")
    ./configure --prefix="$PREFIX"
    ;;

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

  "--help"|"help"|*)
    echo "Usage:"
    echo "  $(basename "$0") {COMMAND}"
    echo ""
    echo "Commands:"
    echo "  release       Compile for release"
    echo "  debug         Compile with debug option"
    echo "  fulldebug     Compile with all warning options"
    echo "  profile       Compile with profile option (analysis with gprof)"
    echo "  cpu-profiler  Link with Google cpu performance profiler"
    echo "  gcov          Compile with options for coverage analysis with gcov"
    echo "  help          Show this help"
    exit 1
    ;;
esac

JOBS="$((CPU_COUNT/2))"
test "$JOBS" -eq 0 && JOBS=1
make V=1 -j$JOBS
# make install
