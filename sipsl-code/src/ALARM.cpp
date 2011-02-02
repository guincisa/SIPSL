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
} ALMGRtuple;
//**********************************************************************************
extern "C" void* ALARMSTACK (void*);

void * ALARMSTACK(void *_tgtObject) {

    DEBOUT("ALARMSTACK start","")

    ALMGRtuple *tgtObject = (ALMGRtuple *)_tgtObject;

    tgtObject->st->alarmer();

    DEBOUT("ALARMSTACK started","")

    return (NULL);
}
//**********************************************************************************
ALMGR::ALMGR(SL_CC* _sl_cc, __time_t _sec, long int _nsec){
	sleep_time.tv_sec = _sec;
	sleep_time.tv_nsec = _nsec;
	sl_cc = _sl_cc;
	DEBOUT("ALMGR::ALMGR", "alarm created")
}
ALMGR::ALMGR(SL_CC* _sl_cc, timespec _sleep_time){
	sleep_time = _sleep_time;
	sl_cc = _sl_cc;
	DEBOUT("ALMGR::ALMGR", "alarm created")
}
void ALMGR::initAlarm(void){

	DEBOUT("ALMGR::initAlarm", "init")

    NEWPTR2(listenerThread,ThreadWrapper, "ThreadWrapper");
    ALMGRtuple *t1;
    NEWPTR2(t1,ALMGRtuple, "ALMGRtuple");
    t1->st = this;
    int res;
    res = pthread_create(&(listenerThread->thread), NULL, ALARMSTACK, (void *) t1 );

    //TODO check consistency!!!
    pthread_mutex_init(&mutex, NULL);

	DEBOUT("ALMGR::initAlarm", "started")
    return;
}
void ALMGR::alarmer(void){

#ifdef DEBCODE
	int jumper = 0;
#endif

    DEBOUT("ALMGR::alarmer", "begin")
    for(;;){

        nanosleep(&sleep_time,NULL);

        GETLOCK(&mutex,"mutex");

        //extract from priority queue
        SysTime mytime;
        GETTIME(mytime);
        lli curr = ((lli) mytime.tv.tv_sec)*1000000+(lli)mytime.tv.tv_usec;
        triple trip;
        map<string,ALARM*>::iterator cid_iter;


#ifdef DEBCODE
        jumper ++;
        if (jumper == 1000){
                DUMPMESSTABLE
                DEBOUT("ALARM tables pq", pq.size() << "] cidmap [" << cidmap.size())
            jumper = 0;
        }
#endif

        if (!pq.empty()) {
            trip = pq.top();
            while ( trip.time <= curr){
                DEBOUT("Found and alarm ready for trigger", trip.time << "][" << trip.cid << "][" << trip.alarm)
                if ( trip.alarm->isActive()){
                    DEBOUT("Alarm is active", trip.cid << "][" << trip.alarm)
                    cid_iter = cidmap.find(trip.cid);
                    if ( cid_iter == cidmap.end()){
                        DEBOUT("Inconsistency 1 in ALARM, alarm was found active but cid is not in cidmap",trip.cid << "][" << trip.alarm)
                        DEBASSERT("ALARM inconsistency 1")
                    }else {
                        if ( (ALARM*)(cid_iter->second) != trip.alarm){
                            DEBOUT("Inconsistency 2 in ALARM, alarm was found active but cidmap points to different alarm",trip.cid << "][" << trip.alarm <<"]["<<(ALARM*)(cid_iter->second))
                            DEBASSERT("ALARM inconsistency 2")
                        } else {//Alarm structures ok
#ifdef DEBCODE
                            MESSAGE* _tmpMess = trip.alarm->getMessage();
                            DEBOUT("ALARM found message", _tmpMess << "][" << trip.alarm)
                            //Selfcheck code
                            //This code fixes mistakes made in sm
                            map<const MESSAGE*, MESSAGE*>::iterator p;
                            int i = getModulus(_tmpMess);
                            pthread_mutex_lock(&messTableMtx[i]);
                            p = globalMessTable[i].find(_tmpMess);
                            bool delmess=false;
                            if (p ==globalMessTable[i].end()){
                                    delmess = true;
                                    DEBWARNING("Inconsistency 3 in ALARM Active ALARM found with deleted message",_tmpMess <<"]["<<trip.alarm)
                                    DEBASSERT("Inconsistency 3 in ALARM")
                            }
                            pthread_mutex_unlock(&messTableMtx[i]);

                            if ( trip.alarm->getMessage()->getTypeOfInternal() == TYPE_MESS ){
                                DEBOUT("Inconsistency 4 in ALARM invalid operation TYPE_MESS", _tmpMess->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMess->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") << "#" << _tmpMess->getOrderOfOperation() << "#")
                                DEBOUT("Inconsistency 4 in ALARM invalid operation TYPE_MESS cid",trip.cid)
                                DEBASSERT("Inconsistency 4 in ALARM")
                            }
#endif
                            //Message is ok now
                            DEBOUT("ALMGR::alarmer sending alarm: operation TYPE_OP", trip.cid)
                            bool ret = sl_cc->p_w(trip.alarm->getMessage());
                            DEBOUT("bool ret = sl_cc->p_w(_tmpMess);", ret)
                            if(!ret){
                                DEBOUT("ALARM::alarmer message rejected, put in rejection queue",trip.alarm->getMessage())
                                bool ret2 = sl_cc->p_w_s(trip.alarm->getMessage());
                                if (!ret2){
                                    if (!trip.alarm->getMessage()->getLock()){
                                        MESSAGE* ttt = trip.alarm->getMessage();
                                        PURGEMESSAGE( ttt )
                                    }
                                }
                            }
                            //Alarm was sent now clean up
                            cidmap.erase(trip.cid);
                            DELPTR(trip.alarm,"ALARM")
                        }
                    }
                } else{// Not active
                    DEBOUT("Alarm is not active", trip.cid << "][" << trip.alarm)
                    cid_iter = cidmap.find(trip.cid);
                    if ( (cid_iter->second) == trip.alarm ){
                            cidmap.erase(trip.cid);
                            DELPTR(trip.alarm,"ALARM")
                    }else {
                         DEBOUT("Alarm is not active and active replacement exists", trip.cid << "][" << trip.alarm << "][" << (cid_iter->second))
                    }
                }
                pq.pop();
                if (!pq.empty()){
                    trip = pq.top();
                }
            }
        }else {//empty pq
        }
        RELLOCK(&mutex,"mutex");
    }
}

void ALMGR::insertAlarm(MESSAGE* _message, lli _fireTime){

    DEBOUT("ALMGR::insertAlarm message", _message)


#ifdef DEBCODE
    SysTime afterT;
    GETTIME(afterT);
    lli firetime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec;
    DEBOUT("ALMGR::insertAlarm", _fireTime )
    DEBOUT("ALMGR::insertAlarm delta", (lli) (_fireTime  - firetime) )
#endif

    if ( _message->getTypeOfInternal() == TYPE_MESS ){
        DEBOUT("ALMGR: invalid operation TYPE_MESS", _message->getHeadCallId().getContent() << ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") << "#" << _message->getOrderOfOperation() << "#")
        DEBASSERT("Invalid alarm")
    }

    GETLOCK(&mutex,"mutex");

    NEWPTR(ALARM*, alm, ALARM(_message, _fireTime),"ALARM")

    string cidbranch_alarm = alm->getCidbranch();
    DEBOUT("Inserting Alarm id (cid+branch)", cidbranch_alarm << "]["<<alm);

    map<string, ALARM*>::iterator p;
    p = cidmap.find(cidbranch_alarm);

    if (p != cidmap.end()){
        DEBOUT("Inserting an alarm, cid already exists",p->first <<"][" << (ALARM*)(p->second))
        ((ALARM*)(p->second))->cancel();
         DEBOUT("Deactivating alarm",p->first <<"][" << (ALARM*)(p->second))
        //PROBLEM: the existing alarm may be more in the future, it is a problem??

        //remove from cidmap
        DEBOUT("Erasing from cidmap alarm",p->first <<"][" << (ALARM*)(p->second))
        cidmap.erase(cidbranch_alarm);  
    }
    triple trip;
    trip.time = _fireTime;
    trip.cid = cidbranch_alarm;
    trip.alarm = alm;

    pq.push(trip);
    cidmap.insert(pair<string,ALARM*>(cidbranch_alarm,alm));

    RELLOCK(&mutex,"mutex");

    DEBOUT("Alarm Inserterd",_fireTime <<"][" << cidbranch_alarm << "]["<<alm)
    return;
}


void ALMGR::cancelAlarm(string _cidbranch){

    DEBOUT("ALMGR::cancelAlarm", _cidbranch)
    // alarm is deactivated and the related message may have been
    // purged so

    GETLOCK(&mutex,"mutex");
            internalCancelAlarm(_cidbranch);
    RELLOCK(&mutex,"mutex");


}
void ALMGR::internalCancelAlarm(string _cidbranch){
    
    map<string, ALARM*>::iterator p;
    p = cidmap.find(_cidbranch);

    if (p != cidmap.end()){
         ((ALARM*)(p->second))->cancel();
         DEBOUT("Deactivating alarm",p->first <<"][" << (ALARM*)(p->second))
    }
    else{
        DEBOUT("Cancel alarm does not exists",_cidbranch)
    }
}
//**********************************************************************************
ALARM::ALARM(MESSAGE *_message, lli _fireTime){

	message = _message;
	fireTime = _fireTime;
	active = true;
	DEBOUT("ALARM::ALARM firetime", fireTime)
	cidbranch = _message->getHeadCallId().getContent() + ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _message->getOrderOfOperation()+ "#";

}
ALARM::ALARM(void){
    DEBASSERT("ALARM empty constructor called")
}
lli ALARM::getTriggerTime(void){
	return fireTime;
}
string ALARM::getCidbranch(void){
	return cidbranch;
}

void ALARM::cancel(void){
    DEBOUT("ALARM::cancel",this)
    active = false;
}
MESSAGE* ALARM::getMessage(void){
	if (!active){
		DEBASSERT("Break rule: accessing the message of an inactive ALARM")
		return 0x0;
	}
	else{
		return message;
	}
}
bool ALARM::isActive(void){
	return active;
}





//
//            while (!alarm_pq.empty() && curr >= tcu){
//
//                DEBOUT("Trigger time exists", alarm_pq.size() << "] curr [" << curr << "] tcu["<<tcu << "] diff [" << curr - tcu)
//
//                triple trip =
//                multimap<const lli, string>::iterator iter;
//                pair<multimap<const lli,string>::iterator,multimap<const lli ,string>::iterator> ret;
//                map<ALARM*,int> delmap;
//                map<string, ALARM*>::iterator cidAlm_iter;
//
//                ret = time_cid_mumap.equal_range(tcu);
//                DEBOUT("ALARM Triggerables", time_cid_mumap.count(tcu))
//
//                for (iter=ret.first; iter!=ret.second; iter++){
//#ifdef DEBCODE
//                    {//DUMP cidbranch
//                        map<string, ALARM*>::iterator ps;
//                        for ( ps = cidbranch_alm_map.begin(); ps !=cidbranch_alm_map.end(); ps++){
//                            DEBOUT("**** DUMP CIDBRANCH", (string)ps->first)
//                        }
//                    }
//#endif
//                    string tcid = iter->second;
//                    cidAlm_iter = cidbranch_alm_map.find(tcid);
//                    if (cidAlm_iter == cidbranch_alm_map.end()){
//                        DEBOUT("ALARM inconsystency, found in mumap but not in cidbranch_alam", tcid)
//                        DEBASSERT("ALARM inconsystency, found in mumap but not in cidbranch_alam")
//                    }
//                    ALARM* tmal = cidAlm_iter->second;
//                    if (tmal->isActive()==ALMNEW){
//
//                        //Selfcheck code
//                        //Takes the branch of current alarm
//                        //read alarm* in the masp
//                        //if alarm* is different than current then there is a newer alarm
//                        //so this shoudl hev been deactivated!
//
//                        MESSAGE* _tmpMess = tmal->getMessage();
//
//                        DEBOUT("ALARM found message", _tmpMess)
//
//                        //Selfcheck code
//                        //This code fixes mistakes made in sm
//                        map<const MESSAGE*, MESSAGE*>::iterator p;
//                        int i = getModulus(_tmpMess);
//                        pthread_mutex_lock(&messTableMtx[i]);
//                        p = globalMessTable[i].find(_tmpMess);
//                        bool delmess=false;
//                        if (p ==globalMessTable[i].end()){
//                                delmess = true;
//                                DEBWARNING("Active ALARM found with deleted message",_tmpMess)
//                        }
//                        pthread_mutex_unlock(&messTableMtx[i]);
//
//                        //ALMGR shall not care about message or internalop
//                        //SL_CC does it but here if for debug purposes
//                        if (delmess){
//
//                        }
//                        else if ( _tmpMess->getTypeOfInternal() == TYPE_MESS ){
//                            DEBOUT("ALMGR: invalid operation TYPE_MESS", _tmpMess->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMess->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") << "#" << _tmpMess->getOrderOfOperation() << "#")
//                            DEBASSERT("Inactive code")
//                        } else {
//                            DEBOUT("ALMGR::alarmer sending alarm: operation TYPE_OP", _tmpMess->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMess->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") << "#" << _tmpMess->getOrderOfOperation() << "#")
//                            bool ret = sl_cc->p_w(_tmpMess);
//                            DEBOUT("bool ret = sl_cc->p_w(_tmpMess);", ret)
//                            if(!ret){
//                                DEBOUT("ALARM::alarmer message rejected, put in rejection queue",_tmpMess)
//                                bool ret2 = sl_cc->p_w_s(_tmpMess);
//                                if (!ret2){
//                                    if (!_tmpMess->getLock()){
//                                        PURGEMESSAGE(_tmpMess)
//                                    }
//                                }
//                            }
//                        }
//                        DEBOUT("ALARM CHECK INTERVAL", tmal << "][curr " << (lli)curr << "][tcu " <<  (lli)tcu)
//                        PRTIME
//                        PRTIME_F((lli)tcu)
//                        cidbranch_alm_map.erase(tcid);
//                        DELPTR(tmal,"ALARM")
//                        DEBY
//                    }else if(tmal->isActive()==ALMREP){
//                        //Alarm is not active
//                        //don't delete from cidbranch
//                    }else if (tmal->isActive()==ALMCAN){
//                        cidbranch_alm_map.erase(tcid);
//                        DELPTR(tmal,"ALARM")
//                    }
//
//                }
//
//#ifdef DEBCODE
//                {//DUMP multimap
//                    multimap<lli,string,less<lli> >::iterator iter;
//                    for ( iter = time_cid_mumap.begin(); iter !=time_cid_mumap.end(); iter++){
//                        DEBOUT("**** DUMP MUMAP before", (lli)iter->first << "]["<< (string)iter->second )
//                    }
//                }
//#endif
//                int i = time_cid_mumap.erase(tcu);
//                DEBOUT("Deleting from multimap",i << "] tcu["<< tcu)
//
//#ifdef DEBCODE
//                {//DUMP multimap
//                    multimap<lli,string,less<lli> >::iterator iter;
//                    for ( iter = time_cid_mumap.begin(); iter !=time_cid_mumap.end(); iter++){
//                        DEBOUT("**** DUMP MUMAP after", (lli)iter->first << "]["<< (string)iter->second )
//                    }
//                }
//#endif
//                alarm_pq.pop();
//                if (!alarm_pq.empty()){
//                    tcu = alarm_pq.top();
//                }
//            }
//        }
//        else{
//            //Autofix code
//            //This code fixes mistakes
//            //the pq can be empty nut the other structures may still contain something.
//            if(!time_cid_mumap.empty()){
//                    DEBWARNING("ALARM pq empty, time_alarm_mumap not empty","")
//            }
//            if (!cidbranch_alm_map.empty()){
//                    DEBWARNING("ALARM pq empty, cidbranch_alm_map not empty","")
//            }
//        }
//        RELLOCK(&mutex,"mutex");
//    }
//}
//void ALMGR::insertAlarm(MESSAGE* _message, lli _fireTime){
//
//    DEBOUT("ALMGR::insertAlarm message", _message)
//    //check if message already exists and cancel related alarm
//    //do not remove it from multimap and from mess_alm map
//
//#ifdef DEBCODE
//    SysTime afterT;
//    GETTIME(afterT);
//    lli firetime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec;
//    DEBOUT("ALMGR::insertAlarm", _fireTime )
//    DEBOUT("ALMGR::insertAlarm delta", (lli) (_fireTime  - firetime) )
//#endif
//
//    if ( _message->getTypeOfInternal() == TYPE_MESS ){
//        DEBOUT("ALMGR: invalid operation TYPE_MESS", _message->getHeadCallId().getContent() << ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") << "#" << _message->getOrderOfOperation() << "#")
//        DEBASSERT("Invalid alarm")
//    }
//
//    GETLOCK(&mutex,"mutex");
//
//    NEWPTR(ALARM*, alm, ALARM(_message, _fireTime),"ALARM")
//
//    string cidbranch_alarm = alm->getCidbranch();
//    DEBOUT("Inserting Alarm id (cid+branch)", cidbranch_alarm << "]["<<alm);
//
//    map<string, ALARM*>::iterator p;
//    p = cidbranch_alm_map.find(cidbranch_alarm);
//#ifdef DEBCODE
//    {//DUMP cidbranch
//        map<string, ALARM*>::iterator ps;
//        for ( ps = cidbranch_alm_map.begin(); ps !=cidbranch_alm_map.end(); ps++){
//            DEBOUT("**** DUMP CIDBRANCH", (string)ps->first)
//        }
//    }
//#endif
//    //Autofix code
//    //This code fixes mistakes made in sm
//    if ( p!= cidbranch_alm_map.end()){
//        //Same alarm id already exists
//        DEBOUT("Same alarm id already exists", cidbranch_alarm<<"]["<<alm<<"]["<<((ALARM*)p->second)<<"]["<<((ALARM*)p->second)->alarmState())
//        DEBOUT("Same alarm id already exists: new time", alm->getTriggerTime() <<"] old time ["<<((ALARM*)p->second)->getTriggerTime() <<"] diff > 0["<< alm->getTriggerTime() -  ((ALARM*)p->second)->getTriggerTime())
//        ((ALARM*)p->second)->replace();
////        if( ((ALARM*)p->second)->isActive() ){
////            DEBOUT("Active alarm replaced",cidbranch_alarm<<"]["<<alm<<"]["<<alm->getTriggerTime())
////            DEBWARNING("Active alarm replaced",cidbranch_alarm<<"]["<<alm<<"]["<<alm->getTriggerTime())
////            ((ALARM*)p->second)->cancel();
////        }
//    }
//    //Alarm will remain in alarm_pq
//    //and in mulultimap!!!
//    cidbranch_alm_map.erase(cidbranch_alarm);
//    cidbranch_alm_map.insert(pair<string, ALARM*>(cidbranch_alarm, alm));
//
//    // insert into priority q
//    alarm_pq.push(alm->getTriggerTime());
//
//    //insert into map time - alarm
//    time_cid_mumap.insert(pair<lli const, string>(alm->getTriggerTime(), cidbranch_alarm));
//
//
//    RELLOCK(&mutex,"mutex");
//    DEBOUT("Inserted Alarm id (cid+branch", cidbranch_alarm << "]["<<alm);
//
//    return;
//
//}
//
//void ALMGR::cancelAlarm(string _cidbranch){
//
//	// alarm is deactivated and the related message may have been
//	// purged so
//
//	GETLOCK(&mutex,"mutex");
//		internalCancelAlarm(_cidbranch);
//	RELLOCK(&mutex,"mutex");
//
//
//}
//void ALMGR::internalCancelAlarm(string _cidbranch){
//
//#ifdef DEBCODE
//    {//DUMP cidbranch
//        map<string, ALARM*>::iterator ps;
//        for ( ps = cidbranch_alm_map.begin(); ps !=cidbranch_alm_map.end(); ps++){
//            DEBOUT("**** DUMP CIDBRANCH", (string)ps->first)
//        }
//    }
//#endif
//    DEBOUT(" ALMGR::internalCancelAlarm", _cidbranch)
//    map<string, ALARM*> ::iterator p;
//    p = cidbranch_alm_map.find(_cidbranch);
//    ALARM* tmp = 0x0;
//    if (p != cidbranch_alm_map.end()){
//        DEBOUT("ALMGR::internalCancelAlarm", "found")
//        tmp = (ALARM*)p->second;
//
//        DEBOUT("ALMGR::internalCancelAlarm", _cidbranch <<  " is" << tmp->alarmState() )
//
//    }
//    else {
//        DEBOUT("ALMGR::internalCancelAlarm", "not found")
//    }
//
//}
////**********************************************************************************
//ALARM::ALARM(MESSAGE *_message, lli _fireTime){
//
//	message = _message;
//	fireTime = _fireTime;
//	active = ALMNEW;
//	DEBOUT("ALARM::ALARM firetime", fireTime)
//	cidbranch = _message->getHeadCallId().getContent() + ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _message->getOrderOfOperation()+ "#";
//
//}
//ALARM::ALARM(void){
//    DEBASSERT("ALARM empty constructor called")
//}
//lli ALARM::getTriggerTime(void){
//	return fireTime;
//}
//string ALARM::getCidbranch(void){
//	return cidbranch;
//}
//
//void ALARM::cancel(void){
//    DEBOUT("ALARM::cancel",this)
//    active = ALMCAN;
//}
//void ALARM::replace(void){
//    DEBOUT("ALARM::replace",this)
//    active = ALMREP;
//}
//MESSAGE* ALARM::getMessage(void){
//	if (active!=ALMNEW){
//		DEBASSERT("Break rule: accessing the message of an inactive ALARM")
//		return 0x0;
//	}
//	else{
//		return message;
//	}
//}
//int ALARM::isActive(void){
//	return active;
//}
//
//
//
