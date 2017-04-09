#!/bin/bash

N=0
HEIGHT=16
FONTFILE="8x16std"

(
  echo -e "// vgafont.h\n"
  echo -e "#ifndef FVGAFONT_H"
  echo -e "#define FVGAFONT_H\n"

  xxd -g 1 -i -c $HEIGHT $FONTFILE \
  | sed -e 's/ {$/\n{/' \
  | sed -e 's/^unsigned/static unsigned/' \
  | sed -e '/len = /d' \
  | while IFS=$'\n'; read -r LINE
  do
    if [ "${LINE:0:1}" != " " ]
    then
      echo "$LINE"
    else
      echo "$LINE  /* $N */"  | sed -e 's/\([0-9]\)  \/\*/\1   \/*/'
      let N++
    fi
  done

  echo -e "\n#endif  // FVGAFONT_H"
) > vgafont.h
