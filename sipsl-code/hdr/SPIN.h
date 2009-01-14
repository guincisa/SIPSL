//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer 
// Copyright (C) 2007 Guglielmo Incisa di Camerana
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************

#define SIPN
//#include <pthread.h>
//#include <string>
//#include <iostream>
//#include <memory>
//#include <sys/time.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>

//SPINBUFFER
#define WW  2
#define RR  1
#define FF  0

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//SPINBUFFER
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SPINB;
///////////////////////////////////////////////////////////////////////////////
class ROTQ {
    private:
        MESSAGE Q[ARR];
        int top,bot; // da scrivere, da leggere 
        int state;
        SPINB *sb;

    public:
        ROTQ(void);
        void setSpinb(SPINB *);
        void setState(int);
        int getState(void);
        void put(MESSAGE);
        MESSAGE get(void);
        bool isEmpty(void);
};
///////////////////////////////////////////////////////////////////////////////
class SPINB {
    private:

    //queue<MESSAGE> Q0,Q1,Q2;
    ROTQ Q[3];

    //int state[3]; // 0 free, 1 write, 2 read
    int readbuff, writebuff, freebuff;

    public:
    
    int DIM;
    pthread_mutex_t readmu;
    pthread_mutex_t writemu;

    pthread_mutex_t mudim;
    pthread_mutex_t condvarmutex;
    pthread_cond_t condvar;

    SPINB(void);
    void put(MESSAGE);
    MESSAGE get(void);
    void move(void);
    bool isEmpty(void);
};
///////////////////////////////////////////////////////////////////////////////
/*
template<class MESS> class SPINBTMP;
template<class MESS> class ROTQTMP {
    private:
        MESS Q[ARR];
        int top,bot; // da scrivere, da leggere 
        int state;
        SPINBTMP *sb;

    public:
        ROTQTMP(void);
        void setSpinb(SPINBTMP *);
        void setState(int);
        int getState(void);
        void put(MESS);
        MESS get(void);
        bool isEmpty(void);
};
///////////////////////////////////////////////////////////////////////////////
template<class MESS> class SPINBTMP {
    private:

    //queue<MESSAGE> Q0,Q1,Q2;
    ROTQTMP Q[3];

    //int state[3]; // 0 free, 1 write, 2 read
    int readbuff, writebuff, freebuff;

    public:
    
    int DIM;
    pthread_mutex_t readmu;
    pthread_mutex_t writemu;

    pthread_mutex_t mudim;
    pthread_mutex_t condvarmutex;
    pthread_cond_t condvar;

    SPINBTMP(void);
    void put(MESS);
    MESS get(void);
    void move(void);
    bool isEmpty(void);
};
*/
//
