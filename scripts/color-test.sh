#!/bin/sh

case "$1" in
  -h|--help)
    echo "Usage: ${0##*/} [OPTIONS]"
    echo ""
    echo "Tests and displays the terminal's color capabilities up to 256 colors."
    echo "It uses 'tput' to dynamically query the terminfo database based on \$TERM."
    echo ""
    echo "Options:"
    echo "  -h, --help    Show this help message and exit"
    echo ""
    echo "Environment Variables:"
    echo "  TERM          Set this to test different terminal profiles"
    echo "                (e.g., TERM=wyse-325 ${0##*/})"
    exit 0
    ;;
esac

COLORS=$(tput colors 2>/dev/null) || COLORS=2
[ "$COLORS" -lt 2 ] && COLORS=2
[ "$COLORS" -gt 256 ] && COLORS=256

REV=$(tput rev 2>/dev/null)
RESET=$(tput sgr0 2>/dev/null)

newline()
{
  printf "%s\n\n" "$RESET"
}

echo "TERM=$TERM"
echo "Colors=$COLORS"
echo ""

if [ "$COLORS" -eq 2 ]
then
  if [ -n "$REV" ]
  then
    echo "System colors:"
    printf "  %s  %s\n" "$REV" "$RESET"
  fi

  exit 0
fi

SYSCOLORS=$COLORS
[ "$SYSCOLORS" -gt 16 ] && SYSCOLORS=16
i=0
echo "System colors:"

while [ "$i" -lt "$SYSCOLORS" ]
do
  tput setab "$i" 2>/dev/null
  printf "  "

  if [ "$i" -eq 7 ]
  then
    printf "%s (0..7)\n" "$RESET"
  elif [ "$i" -eq 15 ]
  then
    printf "%s (8..15)\n" "$RESET"
  fi

  i=$((i + 1))
done

newline
[ "$COLORS" -le 16 ] && exit 0

case "$COLORS" in
  256)
    echo "Color cube, 6x6x6:"
    SIZE=6
    MUL=36
    STEP=6
    LABELS="  (16..51)     (52..87)     (88..123)   (124..159)   (160..195)   (196..231)"
    GRAY_START=232
    GRAY_COUNT=24
    GRAY_LABEL=" (232..254)"
    ;;
  88)
    echo "Color cube, 4x4x4:"
    SIZE=4
    MUL=16
    STEP=4
    LABELS="(16..31) (32..47) (48..63) (64..79)"
    GRAY_START=80
    GRAY_COUNT=8
    GRAY_LABEL=" (80..87)"
    ;;
esac

if [ -n "$SIZE" ]
then
  line=0

  while [ "$line" -lt "$SIZE" ]
  do
    cube=0

    while [ "$cube" -lt "$SIZE" ]
    do
      first=$((16 + (cube * MUL) + (line * STEP)))     
      step_i=0

      while [ "$step_i" -lt "$SIZE" ]
      do
        tput setab "$((first + step_i))" 2>/dev/null
        printf "  "
        step_i=$((step_i + 1))
      done
      
      printf "%s " "$RESET"
      cube=$((cube + 1))
    done

    printf "%s\n" "$RESET"
    line=$((line + 1))
  done

  echo "$LABELS"
fi

newline
echo "Grayscale:"

if [ -n "$GRAY_START" ]
then
  g=0

  while [ "$g" -lt "$GRAY_COUNT" ]
  do
    tput setab "$((GRAY_START + g))" 2>/dev/null
    printf "  "
    g=$((g + 1))
  done

  printf "%s%s\n" "$RESET" "$GRAY_LABEL"
fi

