#!/bin/sh

DIR="$PWD"
cd ../examples/.libs/ || exit
LD_LIBRARY_PATH=../../final/.libs/ ./ui
gprof ./ui >./profile.txt
rm ./gmon.out
less ./profile.txt
rm ./profile.txt
cd "$DIR" || exit

