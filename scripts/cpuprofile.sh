#!/bin/bash

LD_LIBRARY_PATH=../src/.libs/ LD_PRELOAD="/usr/lib64/libprofiler.so.0" CPUPROFILE=../examples/.libs/ui.prof ../examples/.libs/ui
pprof --gv ../examples/.libs/ui ../examples/.libs/ui.prof
rm -f ../examples/.libs/ui.prof

