#!/bin/sh
g++ main.cpp VMManager.cpp R216.cpp PeripheralManager.cpp -o r2vm -g -O3 -fexpensive-optimizations -ldl -std=c++17
