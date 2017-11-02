#!/bin/bash

bdftopcf -o 8x16graph.pcf 8x16graph.bdf
gzip -9 8x16graph.pcf

