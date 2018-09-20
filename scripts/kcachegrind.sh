#!/bin/bash

if [ -z "$1" ]
then
  PROG="../examples/.libs/ui"
else
  PROG="$1"
  shift
fi

# Is the file executable?
test ! -x "$PROG" && echo "No executable file not found" && exit -1


LD_LIBRARY_PATH=../src/.libs/ valgrind --tool=callgrind -v "$PROG" 2>/dev/null
kcachegrind
rm -f callgrind.out.*

