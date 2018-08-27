#!/bin/sh
<<<<<<< HEAD
g++ -shared -fpic -lncurses R2TERM.cpp -o R2TERM.so
g++ -shared -fpic diagper.cpp -o diagper.so
cp R2TERM.so ..
cp diagper.so ..

=======
g++ -shared -fpic R2TERM.cpp -o R2TERM.so
cp R2TERM.so ..
>>>>>>> e5a3600436c7c0d7219637cbca14820ce6fbb663
