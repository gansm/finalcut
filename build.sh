#!/bin/sh

#CXX="clang++"
PREFIX="/usr"
RED="\\033[0;31m"
GREEN="\\033[0;32m"
NORMAL="\\033[m"

SRCDIR="$(dirname "$0")"
test -n "$SRCDIR" || SRCDIR=.
cd "$SRCDIR" || exit

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

if [ -n "$1" ]
then
  if [ ! -f ./configure ]
  then
    if command -v autoreconf >/dev/null
    then
      autoreconf --install --force
    else
      echo "Build failed, please install autoconf first"
      exit -1
    fi
  fi
fi

# Build commands
case "$1" in
  "--release"|"release")
    if ! ./configure --prefix="$PREFIX" CXXFLAGS="-O2" # "-O3 -fno-rtti"
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit -1
    fi
    ;;

  "--debug"|"debug")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -pedantic"
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit -1
    fi
    ;;

  "--fulldebug"|"fulldebug")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -Weffc++ -pedantic -pedantic-errors -Wextra -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wpadded -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -fstack-protector -Wstrict-aliasing -Wstrict-aliasing=3 -Wswitch -Wswitch-enum -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wsign-promo -Woverloaded-virtual -Wstrict-null-sentinel -fext-numeric-literals -Wreorder -Wnoexcept -Wnarrowing -Wliteral-suffix -Wctor-dtor-privacy"
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit -1
    fi
    ;;

  "--profile"|"profile")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -pg -O0 -DDEBUG -W -Wall -pedantic"
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit -1
    fi
    ;;

  "--cpu-profiler"|"cpu-profiler")
    if ! ./configure --prefix="$PREFIX" --with-profiler
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit -1
    fi
    ;;

  "--unit-test"|"unit-test")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG" --with-unit-test
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit -1
    fi
    ;;

  "--coverage"|"coverage")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG" --with-unit-test --with-gcov
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit -1
    fi
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
    echo "  unit-test     Compile with CppUnit testing"
    echo "  cpu-profiler  Link with Google cpu performance profiler"
    echo "  coverage      Compile with options for coverage analysis with gcov"
    echo "  help          Show this help"
    exit
    ;;
esac

JOBS="$((CPU_COUNT/2))"
test "$JOBS" -eq 0 && JOBS=1

if make -h 2<&1 | grep -q "\\-\\-jobs"
then
  MAKE="make V=1 -j$JOBS"
else
  MAKE="make V=1"
fi


if $MAKE
then
  printf '%bSuccessful compiled%b\n' "${GREEN}" "${NORMAL}"
else
  printf '%bError on compile!%b\n' "${RED}" "${NORMAL}" 1>&2
  exit 1
fi

if [ "$1" = "--unit-test" ] \
|| [ "$1" = "unit-test" ] \
|| [ "$1" = "--coverage" ] \
|| [ "$1" = "coverage" ]
then
  cd test && make check-TESTS
  cat ./*.log 2>/dev/null
  cd .. || exit
fi

# make install
