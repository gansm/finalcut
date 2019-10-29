#!/bin/bash

function create_code_file ()
{
  ucs2any -d "$FONTFILE" "$MAP" "${REGISTRY}-${ENCODING}"

  (
    echo -e "// ${1}\\n"
    echo -e "#ifndef ${INCLUDE_GUARD}"
    echo -e "#define ${INCLUDE_GUARD}\\n"
    echo -e "namespace finalcut\\n{\\n"
    echo -e "namespace fc\\n{\\n"
    echo -e "static unsigned char ${NAME}[] =\\n{"

    grep -A"${HEIGHT}" "^BITMAP" "$NEWFONTFILE" \
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
    echo -e "\\n#endif  // ${INCLUDE_GUARD}"
  ) > "${1}"

  rm -f "$NEWFONTFILE"
}

#-------------------------#
# Convert newfont to code #
#-------------------------#
N=0
HEIGHT=16
FONTFILE="8x16graph.bdf"
MAP="map-newfont"
REGISTRY="nf"
ENCODING="fontspecific"
INCLUDE_GUARD="FNEWFONT_H"
NAME="__8x16graph"
NEWFONTFILE="8x16graph-nf-fontspecific.bdf"

create_code_file "newfont.h"

#--------------------------#
# Convert VGA font to code #
#--------------------------#
N=0
HEIGHT=16
FONTFILE="8x16graph.bdf"
MAP="map-vga"
REGISTRY="cp437"
ENCODING="1"
INCLUDE_GUARD="FVGAFONT_H"
NAME="__8x16std"
NEWFONTFILE="8x16graph-cp437-1.bdf"

create_code_file "vgafont.h"
