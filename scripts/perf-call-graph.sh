#!/bin/bash

test -z "$EUID" && EUID=$(id -u)

if [ "$EUID" -ne 0 ]
then
  echo "You have to be root to run perf!"
  exit 1
fi

# Get number of logical processor cores
if command -v getconf >/dev/null 2>&1
then
  CPU_COUNT="$(getconf _NPROCESSORS_ONLN 2>/dev/null || getconf NPROCESSORS_ONLN 2>/dev/null)" || CPU_COUNT="0"
fi

if [ "$CPU_COUNT" -eq 0 ]
then
  if command -v nproc >/dev/null 2>&1
  then
    CPU_COUNT="$(nproc 2>/dev/null)" || CPU_COUNT="0"
  fi
fi

test "$CPU_COUNT" -eq 0 && CPU_COUNT=1

SAVE_DIR="$PWD"
cd .. || exit
make distclean
./configure --prefix="/usr" CXXFLAGS="-O3 -fno-omit-frame-pointer"
make -j$JOBS
cd "$SAVE_DIR"

echo 0 > /proc/sys/kernel/kptr_restrict

if [ $# -gt 0 ]
then
  LD_LIBRARY_PATH="../final/.libs/" perf record -g "$@"
else
  EXECUTABLE_FILE="../examples/.libs/ui"
  LD_LIBRARY_PATH="../final/.libs/" perf record -g "$EXECUTABLE_FILE"
fi

perf report -g 'graph,0.5,caller'
rm -f perf.data perf.data.old 2>/dev/null
cd .. || exit
make distclean
cd "$SAVE_DIR"

