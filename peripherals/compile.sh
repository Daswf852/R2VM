#!/bin/sh
g++ -shared -fpic R2TERM.cpp -o R2TERM.so
cp R2TERM.so ..
