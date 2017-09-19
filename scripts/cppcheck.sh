#!/bin/bash

if [ $# -gt 0 ]
then
  eval cppcheck --force --enable=all -I../include/ "$@"
else
  eval cppcheck --force --enable=all -I../include/ ../src/ ../examples/
fi

