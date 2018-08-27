#!/bin/sh
g++ -shared -fpic -lncurses R2TERM.cpp -o R2TERM.so
g++ -shared -fpic diagper.cpp -o diagper.so
cp R2TERM.so ..
cp diagper.so ..
