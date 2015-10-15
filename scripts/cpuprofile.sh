#!/bin/bash

LD_LIBRARY_PATH=../src/.libs/ LD_PRELOAD="/usr/lib64/libprofiler.so.0" CPUPROFILE=../test/.libs/ui.prof ../test/.libs/ui
pprof --gv ../test/.libs/ui ../test/.libs/ui.prof
rm -f ../test/.libs/ui.prof

