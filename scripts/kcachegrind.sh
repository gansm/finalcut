#!/bin/bash

LD_LIBRARY_PATH=../src/.libs/ valgrind --tool=callgrind -v ../examples/.libs/ui 2>/dev/null
kcachegrind
rm -f callgrind.out.*

