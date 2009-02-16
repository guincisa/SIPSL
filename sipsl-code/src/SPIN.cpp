//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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

// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
//SPINBUFFER
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************


#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <map>

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stack>


#ifndef SPIN_H
#include "SPIN.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef ENGINE_H
#include "ENGINE.h"
#endif


ROTQ::ROTQ(void) {

     top = 0;
    bot = 0;

}
void ROTQ::setSpinb(SPINB * _sb) {
    sb = _sb;
}

int ROTQ::getState() {
    return state;
}
void ROTQ::setState(int s) {
    state = s;
}
void ROTQ::put(MESSAGE m) {

    if (state != SPIN_WW) {
        DEBOUT("ERROR not write buffer","")
        return;
    }
    Q[top] = &m;

    top ++ ;
    top = top % ARR;
    if (top == bot) {
        // discard old message
        DEBOUT("FULL TRASHING","")
        pthread_mutex_lock(&(sb->mudim));
        (sb->DIM)--;
        pthread_mutex_unlock(&(sb->mudim));
        bot ++;
        bot = bot % ARR;
    }
}
MESSAGE ROTQ::get(void) {
    MESSAGE *m;
    if (state != SPIN_RR) {
        m->id = -55555;
        DEBOUT("ERROR not read buffer","")
        return *m;
    }
    if (bot == top ) {
        DEBOUT(" top = bot ", "")
        m->id = -99999;
        return *m;
    }
    else {

        m = Q[bot];
        bot ++;
        bot = bot % ARR;
    }
    return *m;
}
bool ROTQ::isEmpty(void) {
    return bot == top;
}

SPINB::SPINB(void) {


    Q[0].setSpinb(this);
    Q[1].setSpinb(this);
    Q[2].setSpinb(this);

    Q[0].setState(SPIN_RR);
    Q[1].setState(SPIN_WW);
    Q[2].setState(SPIN_FF);
    readbuff = 0;
    writebuff = 1;
    freebuff = 2;
    DIM = 0;

    pthread_mutex_init(&mudim, NULL);
    pthread_mutex_init(&readmu, NULL);
    pthread_mutex_init(&writemu, NULL);
    pthread_mutex_init(&condvarmutex, NULL);
    pthread_cond_init(&condvar, NULL);

}

bool SPINB::isEmpty(void) {
    return (Q[0].isEmpty() && Q[1].isEmpty() && Q[2].isEmpty());
}
void SPINB::put(MESSAGE m) {
    // mutex if multi thread
    //
    //cout << "         PUT writebuff " << writebuff << " mess " << m.id << endl;

    int nextbuff = (writebuff +1 ) ;
    nextbuff = nextbuff % 3;

    pthread_mutex_lock(&writemu);
    Q[writebuff].put(m);

    if (Q[nextbuff].getState() == SPIN_FF) {
        //cout <<" PUT spin" << endl;
        Q[nextbuff].setState(SPIN_WW);
        Q[writebuff].setState(SPIN_FF);
        writebuff = nextbuff;
    }
    pthread_mutex_unlock(&writemu);
    pthread_mutex_lock(&mudim);
    DIM++;
    pthread_mutex_unlock(&mudim);
}
MESSAGE SPINB::get(void) {
    // MUTEX
    MESSAGE *m;

    int nextbuff = (readbuff + 1);
    nextbuff = nextbuff % 3;
    pthread_mutex_lock(&readmu);
    if (Q[readbuff].isEmpty() && Q[nextbuff].getState() == SPIN_FF) {
        //cout <<" Get spin" << endl;
            Q[nextbuff].setState(SPIN_RR);
            Q[readbuff].setState(SPIN_FF);
            readbuff = nextbuff;
    }
    m = &Q[readbuff].get();
    pthread_mutex_unlock(&readmu);
    if (m->id != -99999) {
        pthread_mutex_lock(&mudim);
        DIM--;
        pthread_mutex_unlock(&mudim);
        //cout << "         GET readbuff " << readbuff << " mess " << m.id << endl;
    }


    return *m;
}

void SPINB::move(void) {
    pthread_mutex_lock(&writemu);

    int nextbuff = (writebuff +1 ) ;
    nextbuff = nextbuff % 3;

    if (Q[nextbuff].getState() == SPIN_FF) {
        Q[nextbuff].setState(SPIN_WW);
        Q[writebuff].setState(SPIN_FF);
        writebuff = nextbuff;
    }
    pthread_mutex_unlock(&writemu);
    DEBOUT("MOVE","")

}
