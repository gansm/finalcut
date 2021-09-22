#!/bin/sh

SAVE_DIR="$PWD"

run_gcov ()
{
  FILENAME="$1"
  GCDAFILE="${FILENAME%.cpp}.gcda"

  if test -f "$GCDAFILE"
  then
    gcov -lp "$FILENAME"
  elif test -f "./.libs/$GCDAFILE"
  then
    gcov -lp -o "./.libs/" "$FILENAME"
  fi
}

find "./test/" \
     "./final/" \
     -type f \
     -name "*.cpp" \
     -print \
| while read -r FILENAME
do
  cd "$(dirname "$FILENAME")" \
  && run_gcov "$(basename "$FILENAME")"
  cd "$SAVE_DIR" || exit
done

