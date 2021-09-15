#!/bin/sh

SAVE_DIR="$PWD"

find ./src/ \
     ./test/ \
     -type f \
     -name "*.cpp" \
     -print \
| while read -r FILENAME
do
  cd $(dirname "$FILENAME") \
  && gcov -lp $(basename "$FILENAME")
  cd $SAVE_DIR
done
