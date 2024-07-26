#!/bin/sh

if [ $# -gt 0 ]
then
  eval cppcheck --force --language=c++ --std=c++14 --enable=all -I../ "$@"
else
  eval cppcheck --force --language=c++ --std=c++14 --enable=all -I../ ../final/ ../examples/
fi

