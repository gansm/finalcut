Benchmark
=========

The [Rotozoomer example](../examples/rotozoomer.cpp) can perform a 
benchmark run with the parameter "-b" to evaluate the FINAL CUT 
character speed in the terminal. The measured time and frame rate 
are highly dependent on the hardware used.


See the Rotozoomer benchmark in action:

[![rotozoomer-benchmark](https://asciinema.org/a/316531.svg)](https://asciinema.org/a/316531?size=medium&autoplay=1)


Speed tests on an old PC
------------------------

| Terminal           | Size  | Time   | Loops | Frame rate |
|--------------------|-------|--------|-------|------------|
| XTerm              | 80x24 | 2.693s | 314   | 116.598fps |
| PuTTY              | 80x24 | 2.711s | 314   | 115.824fps |
| Mintty             | 80x24 | 2.799s | 314   | 112.182fps |
| Cygwin (cmd)       | 80x24 | 2.99s  | 314   | 105.016fps |
| rxvt-cygwin-native | 80x24 | 2.836s | 314   | 110.719fps |
| rxvt               | 80x24 | 3.064s | 314   | 102.480fps |
| rxvt-unicode       | 80x24 | 2.853s | 314   | 110.059fps |
| Tera Term          | 80x24 | 3.154s | 314   | 99.5561fps |
| Konsole            | 80x24 | 2.727s | 314   | 115.144fps |
| GNOME-Terminal     | 80x24 | 2.683s | 314   | 117.033fps |
| Linux console      | 80x25 | 2.757s | 314   | 113.891fps |
| FreeBSD console    | 80x25 | 2.726s | 314   | 115.187fps |
| NetBSD console     | 80x25 | 2.747s | 314   | 114.306fps |
| OpenBSD console    | 80x25 | 2.751s | 314   | 114.140fps |
| Solaris console    | 80x34 | 3.072s | 314   | 102.213fps |

