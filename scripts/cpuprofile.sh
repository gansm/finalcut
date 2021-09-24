#!/bin/sh

EXECUTABLE_FILE="../examples/.libs/ui"

if [ $# -gt 0 ]
then
  EXECUTABLE_FILE="$1"
  LD_LIBRARY_PATH=../final/.libs/ LD_PRELOAD="/usr/lib/x86_64-linux-gnu/libprofiler.so.0" CPUPROFILE=profile.prof "$@"
else
  LD_LIBRARY_PATH=../final/.libs/ LD_PRELOAD="/usr/lib/x86_64-linux-gnu/libprofiler.so.0" CPUPROFILE=profile.prof "$EXECUTABLE_FILE"
fi

google-pprof --pdf $EXECUTABLE_FILE profile.prof >output.pdf
xdg-open ./output.pdf
sleep 2
rm -f output.pdf profile.prof

