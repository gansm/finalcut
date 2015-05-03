#!/bin/bash

DIR="$PWD"
cd ../src
make clean
make profile
./finalcut
gprof ./finalcut >./profile.txt
rm ./gmon.out
less ./profile.txt
rm ./profile.txt
cd "$DIR"
