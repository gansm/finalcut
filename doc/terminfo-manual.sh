#!/bin/bash

# set the xterm window title
TERMTITLE="man 5 terminfo"
echo -ne "\\033]0;${TERMTITLE}\\007"

man 5 terminfo

