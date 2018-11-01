#!/bin/bash

# set the xterm window title
TERMTITLE="man 4 console_ioctl"
echo -ne "\\033]0;${TERMTITLE}\\007"

man 4 console_ioctl
