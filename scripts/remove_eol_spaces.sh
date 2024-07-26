#!/bin/sh

find ../final/ \
     ../final/util/ \
     ../final/menu/ \
     ../final/widget/ \
     ../final/dialog/ \
     ../final/input/ \
     ../final/vterm/ \
     ../final/output/ \
     ../final/output/tty/ \
     ../examples/ \
     ../test/ \
     -regextype posix-egrep \
     -regex ".*\\.(cpp|h)$" \
     -exec sed -i 's/ *$//' "{}" \;

