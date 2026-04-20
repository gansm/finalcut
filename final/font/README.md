# FINAL CUT newfont
## *A graphical font for X11 and the Linux console*


### How can I use the graphical font on the Linux text console?

Simply launch your program using the `--newfont` argument.

* [newfont-9x16](../../doc/newfont-9x16.txt)
* [newfont-8x16](../../doc/newfont-8x16.txt)


### How can I use the graphical font in X11?

The graphical font requires a terminal emulator capable of changing 
fonts via escape sequences, such as 
[xterm](http://invisible-island.net/xterm/xterm.html)
or
[urxvt](http://software.schmorp.de/pkg/rxvt-unicode.html).
If you are using xterm, ensure the "[Allow Font Ops](../../doc/xterm.txt)" 
option is enabled.


#### Install the pcf bitmap font (8x16graph.pcf.gz) on your X server:

1. Create the font directory (requires root privileges)<br />
`mkdir /usr/share/fonts/X11/misc/finalcut/`

2. Copy the font files, including fonts.dir and fonts.alias, into the new directory<br />
`cp 8x16graph.pcf.gz fonts.dir fonts.alias /usr/share/fonts/X11/misc/finalcut/`

3. Add the directory to your X font path<br />
`xset fp+ /usr/share/fonts/X11/misc/finalcut/`<br />
*(To make this change persistent, add this command to your ~/.xinitrc file)*

4. Verify that the font is available<br />
`xlsfonts | grep 8x16graph`

5. Run your program with the `--newfont` argument<br />
`ui --newfont`

