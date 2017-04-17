all: GNU-amd64-Linux/server

GNU-amd64-Linux/server:  GNU-amd64-Linux/server.o
        g++  -O -s -m64 -o GNU-amd64-Linux/server GNU-amd64-Linux/server.o

GNU-amd64-Linux/server.o : src/server.cpp
        g++  -O -s -m64  -c -o GNU-amd64-Linux/server.o src/server.cpp -I hdr -std=gnu++0x

