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

    DEBOUT("ALMGR priority_queue<const long long int, vector<long long int>, greater<const long long int> > alarm_pq", &alarm_pq)
    DEBOUT("ALMGR multimap<const long long int, ALARM*, less<const long long int> > time_alarm_mumap", &time_alarm_mumap)
    //DEBOUT("ALMGR map<MESSAGE*, ALARM*> mess_alm_map", &mess_alm_map);
    DEBOUT("ALMGR map<string, ALARM*> callid_alm_map", &cidbranch_alm_map)
//    DEBOUT("ALMGR map<string, MESSAGE*> callid_message", &cidbranch_message)
//    DEBOUT("ALMGR map<MESSAGE*, string> message_callid", &message_cidbranch)


    //TODO check consistency!!!
    pthread_mutex_init(&mutex, NULL);

	DEBOUT("ALMGR::initAlarm", "started")
    return;
}
void ALMGR::alarmer(void){

	int jumper = 0;
	DEBOUT("ALMGR::alarmer", "begin")
	for(;;){

		nanosleep(&sleep_time,NULL);

		GETLOCK(&mutex,"mutex");

		//extract from priority queue
		SysTime mytime;
		GETTIME(mytime);
		unsigned long long int curr = ((unsigned long long int) mytime.tv.tv_sec)*1000000+(unsigned long long int)mytime.tv.tv_usec;
		unsigned long long int tcu = 0;

		DEBCODE(jumper ++;
		if (jumper == 1000){
			DUMPMESSTABLE
			DEBOUT("ALARM tables pq", alarm_pq.size() << "] time_alarm_mumap [" << time_alarm_mumap.size() << "] cidbranch_alm_map [" << cidbranch_alm_map.size())
		    jumper = 0;
		})

		if (!alarm_pq.empty()) {

			tcu = alarm_pq.top();


			while (!alarm_pq.empty() && curr >= tcu){

				DEBOUT("ALARM exists", alarm_pq.size())

				// now get a list of alarms from the multi map

				multimap<const unsigned long long int, ALARM*>::iterator iter;
				pair<multimap<const unsigned long long int,ALARM*>::iterator,multimap<const unsigned long long int ,ALARM*>::iterator> ret;
				map<ALARM*,int> delmap;

				ret = time_alarm_mumap.equal_range(tcu);
				//DEBOUT("ALARM Triggerable", time_alarm_mumap.count(tcu))

			    for (iter=ret.first; iter!=ret.second; iter++){
					DEBY
					ALARM* tmal = iter->second;
					DEBOUT("ALARM* tmal", tmal)
					if (tmal->isActive()){

						MESSAGE* _tmpMess = tmal->getMessage();

						DEBOUT("ALARM found message", _tmpMess)

						//Autofix code
						//This code fixes mistakes made in sm
						map<const MESSAGE*, MESSAGE*>::iterator p;
						int i = getModulus(_tmpMess);
						pthread_mutex_lock(&messTableMtx[i]);
						p = globalMessTable[i].find(_tmpMess);
						bool delmess=false;
						if (p ==globalMessTable[i].end()){
							delmess = true;
							DEBWARNING("ALARM found with deleted message",_tmpMess)

						}
						pthread_mutex_unlock(&messTableMtx[i]);

						//ALMGR shall not care about message or internalop
						//SL_CC does it but here if for debug purposes
						if (delmess){

						}
						else if ( _tmpMess->getTypeOfInternal() == TYPE_MESS ){
							DEBOUT("ALMGR::alarmer operation TYPE_MESS", _tmpMess)
							_tmpMess->setHeadSipRequest("INVITE sip:ALLARME@172.21.160.117:5062 SIP/2.0");
							_tmpMess->compileMessage();
							_tmpMess->dumpVector();

							RELLOCK(&mutex,"mutex");

							sl_cc->p_w(_tmpMess);
						} else {
							DEBOUT("ALMGR::alarmer sending alarm: operation TYPE_OP", _tmpMess->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMess->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") << "#" << _tmpMess->getOrderOfOperation() << "#")

							RELLOCK(&mutex,"mutex");

							DEBY
							bool ret = sl_cc->p_w(_tmpMess);
							DEBOUT("bool ret = sl_cc->p_w(_tmpMess);", ret)
							if(!ret){
								DEBOUT("ALARM::alarmer message rejected, put in rejection queue",_tmpMess)
								bool ret2 = sl_cc->p_w_s(_tmpMess);
								if (!ret2){
									if (!_tmpMess->getLock()){
										PURGEMESSAGE(_tmpMess)
									}
								}
							}

						}
					}
					//The alarm if inactive may carry a deleted message, don't access it
					//mess_alm_map.erase(tmal->getMessage());
					delmap.erase(tmal);
					delmap.insert(pair<ALARM*,int>(tmal,0));
					DEBOUT("ALARM CHECK INTERVAL", tmal << "][curr " << (unsigned long long int)curr << "][tcu " <<  (unsigned long long int)tcu)
					PRTIME
					PRTIME_F((unsigned long long int)tcu)
					cidbranch_alm_map.erase(tmal->getCidbranch());
					DEBY
				}
			    for (map<ALARM*,int>::iterator iter2 = delmap.begin(); iter2!=delmap.end(); iter2++){
			    	DELPTR((ALARM*)(iter2->first),"ALARM")

			    }
			    time_alarm_mumap.erase(tcu);

				alarm_pq.pop();
				if (!alarm_pq.empty()){
					tcu = alarm_pq.top();
				}
			}
		}
		else{
			//Autofix code
			//This code fixes mistakes
			//the pq can be empty nut the other structures may still contain something.
			if(!time_alarm_mumap.empty()){
				DEBOUT("ALARM pq empty, time_alarm_mumap not empty","")
			}
			if (!cidbranch_alm_map.empty()){
				DEBOUT("ALARM pq empty, cidbranch_alm_map not empty","")
			}
		}
		RELLOCK(&mutex,"mutex");

	}
}
void ALMGR::insertAlarm(MESSAGE* _message, unsigned long long int _fireTime){

	//check if message already exists and cancel related alarm
	//do note remove it from multimap and from mess_alm map

	DEBCODE(
			SysTime afterT;
			GETTIME(afterT);
			unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec;
			DEBOUT("ALMGR::insertAlarm", _fireTime )
			DEBOUT("ALMGR::insertAlarm delta", (unsigned long long int) (_fireTime  - firetime) )
			)


	GETLOCK(&mutex,"mutex");

	//map<MESSAGE*, ALARM*>::iterator p;

	NEWPTR(ALARM*, alm, ALARM(_message, _fireTime),"ALARM")



	string cidbranch_alarm;
	DEBY
	((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getContent();
	DEBY
	DEBOUT("((C_HeadVia&) _message->getSTKHeadVia().top()).getC_AttVia().getContent()",((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getContent())
	DEBY
	DEBOUT("((C_HeadVia&) _message->getSTKHeadVia().top()).getC_AttVia().getViaParms().getContent()",((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().getContent())
	cidbranch_alarm = _message->getHeadCallId().getContent() + ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") + "#" + _message->getOrderOfOperation()+ "#";
	DEBOUT("Inserting Alarm id (cid+branch", cidbranch_alarm << "]["<<alm);

	map<string, ALARM*>::iterator p;
	p = cidbranch_alm_map.find(cidbranch_alarm);
	//Autofix code
	//This code fixes mistakes made in sm
	if ( p!= cidbranch_alm_map.end()){
		//Same alarm id already exists
		DEBOUT("Same alarm id already exists", cidbranch_alarm<<"]["<<alm<<"]["<<alm->getTriggerTime())
			if( ((ALARM*)p->second)->isActive()){
				DEBWARNING("Active alarm replaced",cidbranch_alarm<<"]["<<alm<<"]["<<alm->getTriggerTime())
				((ALARM*)p->second)->cancel();
			}
	}
	cidbranch_alm_map.erase(cidbranch_alarm);
	cidbranch_alm_map.insert(pair<string, ALARM*>(cidbranch_alarm, alm));

	// insert into priority q
	alarm_pq.push(alm->getTriggerTime());


	//insert into map time - alarm
	time_alarm_mumap.insert(pair<unsigned long long int const, ALARM*>(alm->getTriggerTime(), alm));


	RELLOCK(&mutex,"mutex");

	return;

}

void ALMGR::cancelAlarm(string _cidbranch){

	// alarm is deactivated and the related message may have been
	// purged so

	GETLOCK(&mutex,"mutex");
		internalCancelAlarm(_cidbranch);
	RELLOCK(&mutex,"mutex");


}
void ALMGR::internalCancelAlarm(string _cidbranch){

	map<string, ALARM*> ::iterator p;
	p = cidbranch_alm_map.find(_cidbranch);
	ALARM* tmp = 0x0;
	if (p != cidbranch_alm_map.end()){
		DEBOUT("ALMGR::internalCancelAlarm", "found")
		tmp = (ALARM*)p->second;
		if(!tmp->isActive()){
			DEBOUT("ALMGR::internalCancelAlarm", _cidbranch <<  " is not active")
		}
		tmp->cancel();
	}
	else {
		DEBOUT("ALMGR::internalCancelAlarm", "not found")
	}

}
//**********************************************************************************
ALARM::ALARM(MESSAGE *_message, unsigned long long int _fireTime){

	message = _message;
	fireTime = _fireTime;
	active = true;
	DEBOUT("ALARM::ALARM firetime", fireTime)
	cidbranch = _message->getHeadCallId().getContent() + ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _message->getOrderOfOperation()+ "#";

}
unsigned long long int ALARM::getTriggerTime(void){
	return fireTime;
}
string ALARM::getCidbranch(void){
	return cidbranch;
}

void ALARM::cancel(void){
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

