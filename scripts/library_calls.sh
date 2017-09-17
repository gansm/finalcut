#!/bin/bash

# Protokoliert Funktionsaufrufe in dynamisch hinzugelinkten Bibliotheken

PROG="ui"

LD_LIBRARY_PATH=../src/.libs/ ltrace -o ./$PROG.ltrace ../examples/.libs/$PROG
less ./$PROG.ltrace
rm -f ./$PROG.ltrace ./gmon.out

