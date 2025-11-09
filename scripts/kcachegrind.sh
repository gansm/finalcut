#!/bin/bash

if [[ -z "$1" ]]
then
  PROG="../examples/.libs/ui"
else
  PROG="$1"
  shift
fi

error_handling ()
{
  echo "No ELF executable!"
  exit 1
}

# Is the file executable?
[[ -x "$PROG" ]] || error_handling
file --brief "$PROG" | grep -q "ELF" || error_handling

LD_LIBRARY_PATH=../final/.libs/ valgrind --tool=callgrind -v "$PROG" "$@" 2>/dev/null
kcachegrind
rm -f callgrind.out.*

