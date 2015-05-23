#!/bin/bash

find ../src/ \
     ../test/ \
     -regextype posix-egrep \
     -regex ".*\.(cpp|h)$" \
     -exec sed -i 's/ *$//' "{}" \;

