#!/bin/bash

# Finds uninitialized memory, buffer overflows, memory leaks and discovered access to deallocated memory

if [ -z "$1" ]
then
  PROG="../examples/.libs/ui"
else
  PROG="$1"
  shift
fi

# Is the file executable?
test ! -x "$PROG" && echo "No executable file not found" && exit -1

# ELF executable file?
ELFMAGIC="$(echo -e "\\x7fELF")"
MAGIC="$(dd bs=1 count=4 if="$PROG" 2>/dev/null)"
test "$MAGIC" != "$ELFMAGIC" && echo "No ELF executable file" && exit -2

LD_LIBRARY_PATH=../src/.libs/ valgrind --tool=memcheck --suppressions=../doc/ncurses.supp --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes --log-file=./valgrind.txt "$PROG" "$@"

less ./valgrind.txt
rm -f ./valgrind.txt
