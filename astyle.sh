#!/bin/sh

astyle --style=ansi -T4 -C -S -N -L -p -U -O -o -k3 -n -Z -v "src/*.cpp" "src/*.h" "src/tests/*.cpp"
