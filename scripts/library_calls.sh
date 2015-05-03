#!/bin/bash

# Protokoliert Funktionsaufrufe in dynamisch hinzugelinkten Bibliotheken

PROG="ui"

ltrace -o ./$PROG.ltrace ../test/.libs/$PROG
less ./$PROG.ltrace
rm -f ./$PROG.ltrace

