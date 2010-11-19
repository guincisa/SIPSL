//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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


#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef SPIN_H
#include "SPIN.h"
#endif
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef SIPENGINE_H
#include "SIPENGINE.h"
#endif
#ifndef SL_CC_H
#include "SL_CC.h"
#endif
#ifndef ACTION_H
#include "ACTION.h"
#endif
#ifndef DOA_H
#include "DOA.h"
#endif
#ifndef SUDP_H
#include "SUDP.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif
#ifndef COMAP_H
#include "COMAP.h"
#endif
#ifndef ALO_H
#include "ALO.h"
#endif
#ifndef VALO_H
#include "VALO.h"
#endif
#ifndef SIP_PROPERTIES_H
#include "SIP_PROPERTIES.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
#endif
#ifndef SIPUTIL_H
#include "SIPUTIL.h"
#endif

ROTQ::ROTQ(void) {

     top = 0;
    bot = 0;
    full = false;

    pthread_mutex_init(&fullq, NULL);


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
void ROTQ::put_trashing(MESSAGE* m) {

    if (state != SPIN_WW) {
        DEBOUT("ERROR not write buffer","")
        return;
    }
    Q[top] = m;

    top ++ ;
    top = top % ARR;
    if (top == bot) {
        //DEBASSERT("FULL TRASHING")
        DEBOUT("QUEUE FULL, TRASHING MESSAGES",sb)
        GETLOCK(&(sb->mudim),"sb->mudim");
        (sb->DIM)--;
        RELLOCK(&(sb->mudim),"sb->mudim");
        bot ++;
        bot = bot % ARR;
    }
}
//New
void ROTQ::put_block(MESSAGE* m) {

    if (state != SPIN_WW) {
        DEBOUT("ERROR not write buffer","")
        return;
    }
    Q[top] = m;

    top ++ ;
    top = top % ARR;
    if (top == bot) {
        //DEBASSERT("FULL TRASHING")
        DEBOUT("QUEUE FULL, WAITING",sb)
        full = true;
		GETLOCK(&fullq,"fullq")
//        GETLOCK(&(sb->mudim),"sb->mudim");
//        (sb->DIM)--;
//        RELLOCK(&(sb->mudim),"sb->mudim");
//        bot ++;
//        bot = bot % ARR;
    }
}

MESSAGE* ROTQ::get(void) {

    if (state != SPIN_RR) {
        DEBOUT("ERROR not read buffer","")
        return NULL;
    }
    if (bot == top && !full) {
        DEBOUT(" top = bot, empty", "");
        return NULL;
    }else {
    	MESSAGE* m = Q[bot];
        bot ++;
        bot = bot % ARR;
        full = false;
        RELLOCK(&fullq,"fullq")
        return m;
    }
}
bool ROTQ::isEmpty(void) {
    return bot == top;
}

SPINB::SPINB(int _type) {

    DEBOUT("SPINB::SPINB",this)

	type = _type;

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
void SPINB::put(MESSAGE* m) {
    // mutex if multi thread
    //
    //cout << "         PUT writebuff " << writebuff << " mess " << m.id << endl;

    int nextbuff = (writebuff +1);
    nextbuff = nextbuff % 3;

    GETLOCK(&writemu,"writemu");
    if (type == SPIN_TRASH ){
        Q[writebuff].put_trashing(m);

    }else if (type == SPIN_BLOCK ){
        Q[writebuff].put_block(m);
    }


    if (Q[nextbuff].getState() == SPIN_FF) {
        //cout <<" PUT spin" << endl;
        Q[nextbuff].setState(SPIN_WW);
        Q[writebuff].setState(SPIN_FF);
        writebuff = nextbuff;
    }
    RELLOCK(&writemu,"writemu");
    GETLOCK(&mudim,"mudim");
    DIM++;
    RELLOCK(&mudim,"mudim");
}
MESSAGE* SPINB::get(void) {
    // MUTEX

    int nextbuff = (readbuff + 1);
    nextbuff = nextbuff % 3;
    GETLOCK(&readmu,"readmu");
    if (Q[readbuff].isEmpty() && Q[nextbuff].getState() == SPIN_FF) {
        //cout <<" Get spin" << endl;
            Q[nextbuff].setState(SPIN_RR);
            Q[readbuff].setState(SPIN_FF);
            readbuff = nextbuff;
    }
    MESSAGE* m = Q[readbuff].get();
    RELLOCK(&readmu,"readmu");
    if (m != NULL) {
        GETLOCK(&mudim,"mudim");
        DIM--;
        RELLOCK(&mudim,"mudim");
    }


    return m;
}

void SPINB::move(void) {
    GETLOCK(&writemu,"writemu")

    int nextbuff = (writebuff +1 ) ;
    nextbuff = nextbuff % 3;

    if (Q[nextbuff].getState() == SPIN_FF) {
        Q[nextbuff].setState(SPIN_WW);
        Q[writebuff].setState(SPIN_FF);
        writebuff = nextbuff;
    }
    RELLOCK(&writemu,"writemu");
    DEBOUT("MOVE","")

}
