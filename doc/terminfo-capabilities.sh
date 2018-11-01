#!/bin/bash

# set the xterm window title
TERMTITLE="infocmp: $TERM"
echo -ne "\\033]0;${TERMTITLE}\\007"

infocmp -x | less
