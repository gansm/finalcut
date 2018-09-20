#!/bin/bash

N=0
HEIGHT=16
FONTFILE="8x16graph.bdf"

(
  echo -e "// newfont.h\\n"
  echo -e "#ifndef FNEWFONT_H"
  echo -e "#define FNEWFONT_H\\n"
  echo -e "namespace finalcut\\n{\\n"
  echo -e "namespace fc\\n{\\n"
  echo -e "static unsigned char __8x16graph[] =\\n{"

  grep -A${HEIGHT} ^BITMAP "$FONTFILE" \
  | tr '\n' ',' \
  | sed -e 's/BITMAP,//g' \
  | sed -e 's/--,/\n/g' \
  | tr '[:upper:]' '[:lower:]' \
  | sed -e 's/,/, /g' \
  | sed -e 's/\([0-9a-f][0-9a-f]\)/0x\1/g' \
  | sed -e 's/^0/  0/g' \
  | sed -e '$s/, $/  \n/' \
  | while IFS=$'\n'; read -r LINE
  do
    echo "$LINE /* $N */"
    (( N++ ))
  done

  echo -e "};"
  echo -e "\\n}  // namespace fc"
  echo -e "\\n}  // namespace finalcut"
  echo -e "\\n#endif  // FNEWFONT_H"
) > newfont.h

