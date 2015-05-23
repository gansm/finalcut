#!/bin/bash

LD_PRELOAD="/usr/lib64/libprofiler.so.0" CPUPROFILE=../text/ui.prof ../test/ui
pprof --gv ../test/ui ../test/ui.prof

