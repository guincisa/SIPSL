export OPT="-I./../hdr" 
rm ../../build/HEADERS_TEST
g++  $OPT -c HEADERS_TEST.cpp -o ../../build/HEADERS_TEST.o
g++  $OPT -o ../../build/HEADERS_TEST ../../build/CS_HEADERS.o ../../build/P_HEADERS.o ../../build/HEADERS_TEST.o

