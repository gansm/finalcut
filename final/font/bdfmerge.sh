#!/bin/bash

FONT1="$1"
FONT2="$2"
DELIMITER=$(echo -e "\x1d")

function getCode ()
{
  echo "$1" | grep "^ENCODING "
}

function getGlyph ()
{
  sed -n "/^STARTCHAR /,/^ENDCHAR/p" "$FONT2" \
  | sed -e "s/^ENDCHAR/ENDCHAR$DELIMITER/" \
  | while read -r -d "$DELIMITER" GLYPH
  do
    CODE=$(getCode "$GLYPH")

    if [ "$CODE" = "$1" ]
    then
      echo "$GLYPH"
      return
    else
      continue
    fi

  done
}

function replacementFound ()
{
  grep -q -E "${1}\$" "$FONT2"
  return $?
}

if [ $# != 2 ]
then
  echo "usage:  $(basename "$0") font1.bdf font2.bdf > newfont.bdf"
  exit 1
fi


# Get header
sed -n "1,/^CHARS /p" "$FONT2"

# Get glyphs
sed -n "/^STARTCHAR /,/^ENDCHAR/p" "$FONT1" \
| sed -e "s/^ENDCHAR/ENDCHAR$DELIMITER/" \
| while read -r -d "$DELIMITER" GLYPH
do
  CODE=$(getCode "$GLYPH")

  if replacementFound "$CODE"
  then
    getGlyph "$CODE"
  else
    echo "$GLYPH"
  fi
done

echo "ENDFONT"

