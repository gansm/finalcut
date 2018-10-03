#!/bin/bash

find ../src/ \
     ../src/include/final/ \
     ../examples/ \
     ../test/ \
     -regextype posix-egrep \
     -regex ".*\\.(cpp|h)$" \
     -exec sed -i 's/ *$//' "{}" \;

