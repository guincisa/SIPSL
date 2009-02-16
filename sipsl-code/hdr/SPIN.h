//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
// Copyright (C) 2009 Guglielmo Incisa di Camerana
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

#define SPIN_H
#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif


//SPINBUFFER
#define SPIN_WW  2
#define SPIN_RR  1
#define SPIN_FF  0
#define ARR 30

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//SPINBUFFER
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SPINB;
///////////////////////////////////////////////////////////////////////////////
class ROTQ {
    private:
        MESSAGE *Q[ARR];
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
