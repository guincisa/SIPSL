//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
// Copyright (C) 2011 Guglielmo Incisa di Camerana
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
#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <unordered_map>
#include <math.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string>
#include <string.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>

#include "UTIL.h"
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
#ifndef ACTION_H
#include "ACTION.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif
#ifndef COMAP_H
#include "COMAP.h"
#endif
#ifndef SL_CC_H
#include "SL_CC.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
#endif
#ifndef UTIL_H
#include "UTIL.h"
#endif

//**********************************************************************************
typedef struct tuple2 {
	ALMGR* st;
	int mod;
} ALMGRtuple;
//**********************************************************************************
extern "C" void* ALARMSTACK (void*);

void * ALARMSTACK(void *_tgtObject) {
	DEBINF("void * ALARMSTACK(void *_tgtObject)",_tgtObject)
    DEBDEV("ALARMSTACK start","")

    ALMGRtuple *tgtObject = (ALMGRtuple *)_tgtObject;
	tgtObject->st->alarmer(tgtObject->mod);
	DEBDEV("ALARMSTACK started","")

    return (NULL);
}
//**********************************************************************************
#ifdef ALARMENGINE
ALMGR::ALMGR(int _th, int _map, string _obname, SL_CC* _sl_cc, __time_t _sec, long int _nsec):ENGINE(_th,_map,_obname){
#else
ALMGR::ALMGR(SL_CC* _sl_cc, __time_t _sec, long int _nsec){
#endif
	DEBINF("ALMGR::ALMGR(int _th, int _map, string _obname, SL_CC* _sl_cc, __time_t _sec, long int _nsec):ENGINE(_th,_map,_obname)",this<<"]["<<
			_th<<"]["<<_map<<"]["<<_obname<<"]["<<_sl_cc<<"]["<<_sec<<"]["<<_nsec)

	sleep_time.tv_sec = _sec;
	sleep_time.tv_nsec = _nsec;
	sl_cc = _sl_cc;


	DEBDEV("ALMGR::ALMGR", "alarm created")
}
#ifdef ALARMENGINE
ALMGR::ALMGR(int _th, int _map, string _obname, SL_CC* _sl_cc, timespec _sleep_time):ENGINE(_th,_map,_obname){
#else
ALMGR::ALMGR(SL_CC* _sl_cc, timespec _sleep_time){
#endif

	sleep_time = _sleep_time;
	sl_cc = _sl_cc;


	DEBDEV("ALMGR::ALMGR", "alarm created")
}
void ALMGR::initAlarm(void){
	DEBINF("void ALMGR::initAlarm(void)",this)
	DEBDEV("ALMGR::initAlarm", "init")

	ALMGRtuple *t1[ALARMMAPS];

	for (int i = 0 ; i <ALARMMAPS ; i++){

		NEWPTR2(listenerThread[i],ThreadWrapper, "ThreadWrapper "<<i);
		NEWPTR2(t1[i],ALMGRtuple, "ALMGRtuple");
		t1[i]->st = this;
		t1[i]->mod = i;

		pthread_mutex_init(&mutex[i], NULL);
    	pthread_create(&(listenerThread[i]->thread), NULL, ALARMSTACK, (void *) t1[i] );
    }

    //TODO check consistency!!!

	DEBDEV("ALMGR::initAlarm", "started")
    return;
}
void ALMGR::alarmer(int _mod){
	DEBINF("void ALMGR::alarmer(int _mod)",this<<"]["<<_mod)
#ifdef DEBCODE
	int jumper = 0;
#endif

	DEBDEV("ALMGR::alarmer", "begin"<<"] mod ["<<_mod)

//	timespec tsleep_time;

#ifdef INHIBITALARM
	return;
#endif

	int trymaxlock = 0;
    for(;;){

        nanosleep(&sleep_time,NULL);

        //extract from priority queue
        SysTime mytime;
        GETTIME(mytime);
        lli curr = ((lli) mytime.tv.tv_sec)*1000000+(lli)mytime.tv.tv_usec;
        triple trip;
        map<string,ALARM*>::iterator cid_iter;


//#ifdef DEBCODE
//        jumper ++;
//        if (jumper == 1000){
//                DUMPMESSTABLE
//                DEBDEV("ALARM tables pq", pq.size() << "] cidmap [" << cidmap.size())
//            jumper = 0;
//        }
//#endif

#ifdef USETRYLOCK

        int trylok;
        TRYLOCK(&mutex[_mod]," ALARM loog mutex"<<_mod, trylok)
        if(trylok != 0){
        	DEBDEV("ALARM mutex busy", _mod)
			trymaxlock++;
        	DEBOUT("ALARM mutex busy",_mod <<"]["<<trymaxlock)
        	if (trymaxlock > TRYMAXLOCKALARM){
        		DEBASSERT("ALARM mutex too busy" <<trymaxlock <<" mod "<< _mod)
        	}
//        	tsleep_time.tv_sec = 0;
//        	tsleep_time.tv_nsec = sleep_time.tv_nsec * (2^trymaxlock);
//        	nanosleep(&tsleep_time,NULL);
        	continue;
        }
        else{
        	trymaxlock = 0;
        }
#else
        GETLOCK(&mutex[_mod],"mutex "<<_mod,11);
#endif

        if (!pq[_mod].empty()) {
            trip = pq[_mod].top();
            while ( trip.time <= curr && trip.time > 0){
            	DEBDEV("Found and alarm ready for trigger", trip.time << "][" << trip.cid << "][" << trip.alarm <<"] mod ["<<_mod)
                if ( trip.alarm->isActive()){
                	DEBDEV("Alarm is active", trip.cid << "][" << trip.alarm<<"] mod ["<<_mod)
                    cid_iter = cidmap[_mod].find(trip.cid);
#ifdef CONSICHECK
                    if ( cid_iter == cidmap[_mod].end()){
                    	DEBDEV("Inconsistency 1 in ALARM, alarm was found active but cid is not in cidmap",trip.cid << "][" << trip.alarm<<"] mod ["<<_mod)
                        DEBASSERT("ALARM inconsistency 1")
                    }else {
                        if ( (ALARM*)(cid_iter->second) != trip.alarm){
                        	DEBDEV("Inconsistency 2 in ALARM, alarm was found active but cidmap points to different alarm",trip.cid << "][" << trip.alarm <<"]["<<(ALARM*)(cid_iter->second)<<"] mod ["<<_mod)
                            DEBASSERT("ALARM inconsistency 2"<<"] mod ["<<_mod)
                        } else {//Alarm structures ok
//#ifdef DEBCODEALARM1
//                            MESSAGE* _tmpMess = trip.alarm->getMessage();
//                            DEBOUT("ALARM found message", _tmpMess << "][" << trip.alarm)
//                            //Selfcheck code
//                            //This code fixes mistakes made in sm
//                            map<const MESSAGE*, MESSAGE*>::iterator p;
//                            int i = getModulus(_tmpMess);
//                            pthread_mutex_lock(&messTableMtx[i]);
//                            p = globalMessTable[i].find(_tmpMess);
//                            bool delmess=false;
//                            if (p ==globalMessTable[i].end() && _tmpMess != MainMessage){
//                                    delmess = true;
//                                    DEBWARNING("Inconsistency 3 in ALARM Active ALARM found with deleted message",_tmpMess <<"]["<<trip.alarm)
//                                    DEBASSERT("Inconsistency 3 in ALARM")
//                            }
//                            pthread_mutex_unlock(&messTableMtx[i]);
//
//                            if ( trip.alarm->getMessage()->getTypeOfInternal() == TYPE_MESS ){
//                                DEBOUT("Inconsistency 4 in ALARM invalid operation TYPE_MESS", _tmpMess->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMess->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") << "#" << _tmpMess->getOrderOfOperation() << "#")
//                                DEBOUT("Inconsistency 4 in ALARM invalid operation TYPE_MESS cid",trip.cid)
//                                DEBASSERT("Inconsistency 4 in ALARM")
//                            }
//#endif
                            //Message is ok now
#endif
                        	DEBDEV("ALMGR::alarmer sending alarm: operation TYPE_OP", trip.cid<<"] Message["<<trip.alarm->getMessage()<<"] mod ["<<_mod)
							if (trip.alarm->getMessage()->getLock()){
								 DEBASSERT("Locked alarm"<<trip.alarm->getMessage()<<"] mod ["<<_mod)
							}
                            sl_cc->p_w(trip.alarm->getMessage());
//                            DEBDEV("bool ret = sl_cc->p_w(_tmpMess);", ret)
//                            if(!ret){
//                                DEBDEV("ALARM::alarmer message rejected, put in rejection queue",trip.alarm->getMessage())
//                                bool ret2 = sl_cc->p_w_s(trip.alarm->getMessage());
//                                if (!ret2){
//                                    if (!trip.alarm->getMessage()->getLock()){
//                                        MESSAGE* ttt = trip.alarm->getMessage();
//                                        PURGEMESSAGE( ttt )
//                                    }
//                                }
//                            }
                            //Alarm was sent now clean up
                            cidmap[_mod].erase(trip.cid);
                            DELPTR(trip.alarm,"ALARM")
#ifdef CONSICHECK
                        }
                    }
#endif
                } else{// Not active
                	DEBDEV("Alarm is not active", trip.cid << "][" << trip.alarm<<"] mod ["<<_mod)
                    cid_iter = cidmap[_mod].find(trip.cid);
                    if ( (cid_iter->second) == trip.alarm ){
                            cidmap[_mod].erase(trip.cid);
                    }else {
                    	DEBDEV("Alarm is not active and active replacement exists", trip.cid << "][" << trip.alarm << "][" << (cid_iter->second)<<"] mod ["<<_mod)
                    }

                    //MLF2 delete message
                    MESSAGE* _tmpMess = trip.alarm->getMessage();
                    PURGEMESSAGE(_tmpMess)

                    DELPTR(trip.alarm,"ALARM")

                }
                pq[_mod].pop();
                if (!pq[_mod].empty()){
                    trip = pq[_mod].top();
                }
                else {
                    trip.time = -1; //trick to exit the loop
                }
            }

        }
        else {//empty pq
        }
        RELLOCK(&mutex[_mod],"mutex");
    }
}
void ALMGR::insertAlarm(MESSAGE* _message, lli _fireTime){
	DEBINF("void ALMGR::insertAlarm(MESSAGE* _message, lli _fireTime)",this<<"]["<<_fireTime)

#ifdef INHIBITALARM
	PURGEMESSAGE(_message)
	return;
#endif

	_message->setFireTime(_fireTime);
	p_w((void*)_message);
	return;
}
void ALMGR::parse(void *__mess,int _mod){
	DEBINF("void ALMGR::parse(void *__mess,int _mod)",this<<"]["<<__mess<<"]["<<_mod)
    RELLOCK(&(sb[_mod]->condvarmutex),"sb[" <<_mod<<"]->condvarmutex");

    PROFILE("ALMGR::parse start")
    TIMEDEF
    SETNOW

	MESSAGE* _mess = (MESSAGE*)__mess;

	if (_mess->getFireTime() ==  (lli)0){
		string callid_alarm = _mess->getGenericHeader("Call-ID:") +
				_mess->getViaBranch() +
				"#" + _mess->getOrderOfOperation()+ "#";
	    GETLOCK(&mutex[_mod],"mutex mod ["<<_mod,12);
	    internalCancelAlarm(callid_alarm, _mod);
	    RELLOCK(&mutex[_mod],"mutex"<<"] mod ["<<_mod);
		if(!_mess->getLock()){
			PURGEMESSAGE(_mess)
		}else {
			DEBDEV("Rule break timer off message found locked",_mess)
			DEBASSERT("Rule break timer off message found locked")
		}


	    return;
	}


    //Try lock here!
    if (insertAlarmPrivate(_mess, _mess->getFireTime(),_mod) == 0){
        PRINTDIFF("ALMGR::insertAlarm end")
		return;
    }else {
    	p_w(_mess);
    }

    return;

}

#ifdef ALARMENGINE
int ALMGR::insertAlarmPrivate(MESSAGE* _message, lli _fireTime, int _mod){
#else
void ALMGR::insertAlarm(MESSAGE* _message, lli _fireTime, int _mod){
#endif

    TIMEDEF
    SETNOW

    PROFILE("ALMGR::insertAlarm begin")


#ifdef DEBCODE
    SysTime afterT;
    GETTIME(afterT);
    lli firetime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec;
    DEBDEV("ALMGR::insertAlarm", _fireTime )
    DEBDEV("ALMGR::insertAlarm delta", (lli) (_fireTime  - firetime) )
#endif

    NEWPTR(ALARM*, alm, ALARM(_message, _fireTime),"ALARM")

    string cidbranch_alarm = alm->getCidbranch();

    triple trip;
    trip.time = _fireTime;
    trip.cid = cidbranch_alarm;
    trip.alarm = alm;

#ifdef USETRYLOCK
    int trylok;
    TRYLOCK(&mutex," ALARM loog mutex", trylok)
    if(trylok != 0){
    	return -1;
    }
#else
    GETLOCK(&mutex[_mod]," ALARM log mutex mod "<<_mod,13)
#endif

#ifdef CONSICHECK
    if ( _message->getTypeOfInternal() == TYPE_MESS ){
        DEBOUT("ALMGR: invalid operation TYPE_MESS mod",__mod<<"] ["<< _message->getHeadCallId().getContent() << ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") << "#" << _message->getOrderOfOperation() << "#")
        DEBASSERT("Invalid alarm")
    }
#endif

    pair<map<string, ALARM*>::iterator,bool> ret;

    pq[_mod].push(trip);

    ret = cidmap[_mod].insert(pair<string,ALARM*>(cidbranch_alarm,alm));
    if(!ret.second){
    	DEBDEV("Inserting an alarm, cid already exists", (ALARM*)ret.first->second <<"] mod ["<<_mod)
		((ALARM*)(ret.first->second))->cancel();
        cidmap[_mod].erase(ret.first);
        cidmap[_mod].insert(pair<string,ALARM*>(cidbranch_alarm,alm));
    }

    RELLOCK(&mutex[_mod],"mutex");

    DEBDEV("Alarm Inserterd",_fireTime <<"][" << cidbranch_alarm << "]["<<alm<<"] mod ["<<_mod)

    PROFILE("ALMGR::insertAlarm end"<<"] mod ["<<_mod)

    PRINTDIFF("ALMGR::insertAlarm() end")

    return 0;
}

#ifdef ALARMENGINE
void ALMGR::cancelAlarm(MESSAGE* _message){
	DEBINF("void ALMGR::cancelAlarm(MESSAGE* _message)",this<<"]["<<_message)
	insertAlarm(_message, (lli)0);
}

#else
void ALMGR::cancelAlarm(string _cidbranch, int _mod){
	DEBINF("void ALMGR::cancelAlarm(string _cidbranch, int _mod)",this<<"]["<<_cidbranch<<"]["<<_message)
    PROFILE("ALMGR::cancelAlarm begin mod "<< _mod)

	TIMEDEF
	SETNOW

    int __mod = _mod % ALARMMAPS;


    DEBDEV("ALMGR::cancelAlarm", _cidbranch<<"] mod ["<<_mod << "] alarm mod["<<__mod)
    // alarm is deactivated and the related message may have been
    // purged so
#ifdef USETRYLOCK
    int trylok;
    TRYLOCK(&mutex," ALARM log mutex", trylok)
    if(trylok != 0){
    	return -1;
    }
#else
    GETLOCK(&mutex[__mod],"mutex mod ["<<__mod);
#endif

    internalCancelAlarm(_cidbranch, __mod);
    RELLOCK(&mutex[__mod],"mutex"<<"] mod ["<<__mod);
    
    PROFILE("ALMGR::cancelAlarm end"<<"] mod ["<<__mod)

    PRINTDIFF("ALMGR::cancelAlarm end"<<"] mod ["<<__mod)


}
#endif
void ALMGR::internalCancelAlarm(string _cidbranch, int _mod){
    DEBINF("void ALMGR::internalCancelAlarm(string _cidbranch, int _mod)",this<<"]["<<_cidbranch<<"]["<<_mod)
    map<string, ALARM*>::iterator p;
    p = cidmap[_mod].find(_cidbranch);

    if (p != cidmap[_mod].end()){
         ((ALARM*)(p->second))->cancel();
         DEBDEV("Deactivating alarm",p->first <<"][" << (ALARM*)(p->second)<<"] mod ["<<_mod)
    }
    else{
    	DEBDEV("Cancel alarm does not exists",_cidbranch<<"] mod ["<<_mod)
    }
}
//**********************************************************************************
ALARM::ALARM(MESSAGE *_message, lli _fireTime){
	DEBINF("ALARM::ALARM(MESSAGE *_message, lli _fireTime)",this<<"]["<<_message<<"]["<<_fireTime)
	message = _message;
	fireTime = _fireTime;
	active = true;
	DEBDEV("ALARM::ALARM firetime", fireTime)
	//cidbranch = _message->getHeadCallId().getContent() + ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _message->getOrderOfOperation()+ "#";
	cidbranch = _message->getHeadCallId() + _message->getViaBranch()+ "#" + _message->getOrderOfOperation()+ "#";

}
ALARM::ALARM(void){
    DEBASSERT("ALARM empty constructor called")
}
lli ALARM::getTriggerTime(void){
	DEBINF("lli ALARM::getTriggerTime(void)",this)
	return fireTime;
}
string ALARM::getCidbranch(void){
	DEBINF("string ALARM::getCidbranch(void)",this)
	return cidbranch;
}

void ALARM::cancel(void){
	DEBINF("void ALARM::cancel(void)",this)
	DEBDEV("ALARM::cancel",this)
    active = false;
}
MESSAGE* ALARM::getMessage(void){
	DEBINF("MESSAGE* ALARM::getMessage(void)", this)
//	if (!active){
//		DEBASSERT("Break rule: accessing the message of an inactive ALARM")
//		return 0x0;
//	}
//	else{
		return message;
//	}
}
bool ALARM::isActive(void){
	DEBINF("bool ALARM::isActive(void)", this)

	return active;
}

bool PQ::empty(void){

    if ( L == 0x0)
        return true;
    else
        return false;
};

PQ::PQ(void){

	DEBDEV("PQ::PQ(void)","")
    L = (pqelm*) 0x0;
    R = (pqelm*) 0x0;

};
void PQ::pop(void){

    if (L != 0x0) {
        pqelm* temp = L;
        L = L->right;
        if (temp == R){
            R = 0x0;
            if (L != 0x0){
                DEBASSERT("PQ::next(void)");
            }
        }
        DELPTR(temp,"PQELM");
    }
};
triple PQ::top(void){
    if (L == 0x0){
        DEBASSERT("PQ::top")
    }else{
        return L->element;
    }
};

void PQ::push(triple _elm){

    pqelm* newins = 0x0;
    NEWPTR2(newins,pqelm,"PQELM")
    newins->element = _elm;
    newins->left = 0x0;
    newins->right = 0x0;

    pqelm* tmp = R;
    // empty
    if (tmp == 0x0){
        if (L!=0x0){
            //assert!!! inconsistenza
            assert(0);
        }
        L=newins;
        R=newins;
        return;
    }
    while (tmp != 0x0 && tmp->element.time > _elm.time){
        tmp = tmp->left;
    }
    if (tmp == 0x0){
        newins->right = L;
        newins->left = 0x0;
        L->left = newins;
        L = newins;
        return;
    }
    if (tmp != 0x0 && tmp != R) {
        newins->right = tmp->right;
        tmp->right  = newins;
        newins->right->left = newins;
        newins->left = tmp;
        return;
    }if (tmp == R){
        newins->right = 0x0;
        newins->left = R;
        R->right = newins;

        return;
    }
    DEBASSERT("PQ::push");

};
