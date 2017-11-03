#!/bin/bash

test -f 8x16graph.pcf.gz && rm 8x16graph.pcf.gz
bdftopcf -o 8x16graph.pcf 8x16graph.bdf
gzip -9 8x16graph.pcf

