#!/bin/bash

# Findet uninitialisiertem Speicher, Pufferüberläufe, Speicherlecks und entdeckt Speicherzugriffe auf freigegebene Bereiche

LD_LIBRARY_PATH=../src/.libs/ valgrind --tool=memcheck --suppressions=../doc/ncurses.supp --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes --log-file=./valgrind.txt ../test/.libs/ui "$@"
less ./valgrind.txt
rm -f ./valgrind.txt
