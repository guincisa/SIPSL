export OPT="-I./hdr" 
g++  $OPT -c src/CS_HEADERS.cpp -o ../build/CS_HEADERS.o
g++  $OPT -c src/MESSAGE.cpp -o ../build/MESSAGE.o
exit
g++  $OPT -c src/SPIN.cpp -o ../build/SPIN.o
exit
g++  $OPT -c src/ENGINE.cpp -o ../build/ENGINE.o
g++  $OPT -c src/SUDP.cpp -o ../build/SUDP.o
