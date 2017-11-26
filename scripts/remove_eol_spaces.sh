#!/bin/bash

find ../src/ \
     ../include/final/ \
     ../examples/ \
     -regextype posix-egrep \
     -regex ".*\\.(cpp|h)$" \
     -exec sed -i 's/ *$//' "{}" \;

