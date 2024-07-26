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
    echo -e "constexpr unsigned char ${NAME}[] =\\n{"

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

#------------------------------#
# Convert newfont 8x16 to code #
#------------------------------#
N=0
HEIGHT=16
FONTFILE="8x16graph.bdf"
MAP="map-newfont"
REGISTRY="nf"
ENCODING="fontspecific"
INCLUDE_GUARD="FNEWFONT_8x16_H"
NAME="F_8x16graph"
NEWFONTFILE="8x16graph-nf-fontspecific.bdf"

create_code_file "newfont_8x16.h"

#------------------------------#
# Convert newfont 9x16 to code #
#------------------------------#
N=0
HEIGHT=16
FONTFILE="9x16graph.bdf"
MAP="map-newfont"
REGISTRY="nf"
ENCODING="fontspecific"
INCLUDE_GUARD="FNEWFONT_9x16_H"
NAME="F_9x16graph"
NEWFONTFILE="9x16graph-nf-fontspecific.bdf"

./bdfmerge.sh 8x16graph.bdf 9x16graph_patch.bdf >9x16graph.bdf
create_code_file "newfont_9x16.h"
rm -f 9x16graph.bdf

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
NAME="F_8x16std"
NEWFONTFILE="8x16graph-cp437-1.bdf"

create_code_file "vgafont.h"

