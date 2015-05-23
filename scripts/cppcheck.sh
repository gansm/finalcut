#!/bin/bash

if [ $# -gt 0 ]
then
  eval cppcheck --enable=all "$@"
else
  eval cppcheck --enable=all ../src/ ../test/
fi

