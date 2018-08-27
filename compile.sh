#!/bin/sh
g++ main.cpp VMManager.cpp R216.cpp PeripheralManager.cpp -o r2vm -O3 -fexpensive-optimizations -ldl
