#!/bin/sh

#CXX="clang++"
PREFIX="/usr"
RED="\\033[0;31m"
GREEN="\\033[0;32m"
NORMAL="\\033[m"

PLATFORM="$(uname -s || echo "unknown")"
ARCH="$(uname -m || echo "unknown")"

SRCDIR="$(dirname "$0")"
test -n "$SRCDIR" || SRCDIR=.
cd "$SRCDIR" || exit

print_systeminfo ()
{
  test -z "$CXX" && eval "$(grep '^CXX = ' "Makefile" | cut -d' ' -f1-3 | sed -e 's/ //g')"
  CXX_VERSION="$($CXX -dumpfullversion -dumpversion || echo "unknown version")"
  echo "-------------------------"
  echo "      Platform: $PLATFORM"
  echo "  Architecture: $ARCH"
  echo "      Compiler: $CXX $CXX_VERSION"
  echo "         Build: $1"
  echo "Number of jobs: $JOBS"
  echo "-------------------------"
}

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
      exit 255
    fi
  fi
fi

# Build commands
case "$1" in
  "--release"|"release")
    if ! ./configure --prefix="$PREFIX" CXXFLAGS="-O3"  # "-flto -g1 -gz -march=native -fno-rtti"
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit 255
    fi
    ;;

  "--debug"|"debug")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -pedantic"
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit 255
    fi
    ;;

  "--fulldebug"|"fulldebug")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -W -Wall -Wextra -Weffc++ -pedantic -pedantic-errors -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -fstack-protector -Wstrict-aliasing -Wstrict-aliasing=3 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wsign-promo -Woverloaded-virtual -Wstrict-null-sentinel -fext-numeric-literals -Wreorder -Wnoexcept -Wnarrowing -Wliteral-suffix -Wctor-dtor-privacy -ftree-loop-distribute-patterns -Wmemset-transposed-args -Wno-format-nonliteral"
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit 255
    fi
    ;;

  "--profile"|"profile")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -pg -O0 -DDEBUG -W -Wall -pedantic"
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit 255
    fi
    ;;

  "--cpu-profiler"|"cpu-profiler")
    if ! ./configure --prefix="$PREFIX" --with-profiler
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit 255
    fi
    ;;

  "--unit-test"|"unit-test")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -DUNIT_TEST" --with-unit-test
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit 255
    fi
    ;;

  "--coverage"|"coverage")
    if ! ./configure --prefix="$PREFIX" CPPFLAGS="-DDEBUG" CXXFLAGS="-g -O0 -DDEBUG -DUNIT_TEST" --with-unit-test --with-gcov
    then
      echo "${RED}Configure failed!${NORMAL}" 1>&2
      exit 255
    fi
    ;;

  "--clean"|"clean")
    make distclean
    exit
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
    echo "  clean         Clean up the project"
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
  print_systeminfo "$1"
  printf '%bSuccessful compiled%b\n' "${GREEN}" "${NORMAL}"
else
  print_systeminfo "$1"
  printf '%bError on compile!%b\n' "${RED}" "${NORMAL}" 1>&2
  exit 1
fi

if [ "$1" = "--unit-test" ] \
|| [ "$1" = "unit-test" ] \
|| [ "$1" = "--coverage" ] \
|| [ "$1" = "coverage" ]
then
  rm test/*.log 2>/dev/null
  cd test && make check-TESTS
  cat ./*.log 2>/dev/null
  cd .. || exit
fi

# make install
