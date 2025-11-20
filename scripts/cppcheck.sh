#!/bin/sh

if [ $# -gt 0 ]
then
  cppcheck --force --language=c++ --std=c++14 --enable=all -I../ "$@"
else
  cppcheck --force --language=c++ --std=c++14 --enable=all -I../ ../final/ ../examples/
fi

