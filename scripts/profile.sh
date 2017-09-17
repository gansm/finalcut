#!/bin/bash

DIR="$PWD"
cd ../examples/.libs/ || exit
LD_LIBRARY_PATH=../../src/.libs/ ./ui
gprof ./ui >./profile.txt
rm ./gmon.out
less ./profile.txt
rm ./profile.txt
cd "$DIR" || exit

