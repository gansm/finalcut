#!/bin/bash

# Protokoliert Funktionsaufrufe in dynamisch hinzugelinkten Bibliotheken

PROG="ui"

LD_LIBRARY_PATH=../src/.libs/ ltrace -o ./$PROG.ltrace ../test/.libs/$PROG
less ./$PROG.ltrace
rm -f ./$PROG.ltrace ./gmon.out

