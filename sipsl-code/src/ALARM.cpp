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

#include <pthread.h>
#include <assert.h>


#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
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

    listenerThread = new ThreadWrapper;
    ALMGRtuple *t1;
    t1 = new ALMGRtuple;
    t1->st = this;
    int res;
    res = pthread_create(&(listenerThread->thread), NULL, ALARMSTACK, (void *) t1 );


    //TODO check consistency!!!
    pthread_mutex_init(&mutex, NULL);

	DEBOUT("ALMGR::initAlarm", "started")
    return;
}
void ALMGR::alarmer(void){

	int counter;
	DEBOUT("ALMGR::alarmer", "begin")
	for(;;){

//		counter++;
//		counter = counter % 1000;
//		if (counter == 0)
//			DEBOUT("ALMGR::alarmer", "sleep 1000")

		nanosleep(&sleep_time,NULL);

		//extract from priority queue
		SysTime mytime;
		GETTIME(mytime);
		long long int curr = ((long long int) mytime.tv.tv_sec)*1000000+(long long int)mytime.tv.tv_usec;
		long long int tcu = 0;
		if (!alarm_pq.empty()) {
			tcu = alarm_pq.top();
			while (!alarm_pq.empty() && curr >= tcu){

				alarm_pq.pop();

				// now get a list of alarms from the multi map

				multimap<long long int, ALARM*>::iterator iter;
				pair<multimap<long long int,ALARM*>::iterator,multimap<long long int,ALARM*>::iterator> ret;

				ret = time_alarm_mumap.equal_range(tcu);

			    for (iter=ret.first; iter!=ret.second; ++iter){
					DEBY
					ALARM* tmal = iter->second;
					if (tmal->isActive()){

						MESSAGE* _tmpMess = tmal->getMessage();

						//ALMGR shall not care about message or internalop
						//SL_CC does it but here if for debug purposes

						if ( _tmpMess->typeOfInternal == TYPE_MESS ){
							DEBY
							_tmpMess->setHeadSipRequest("INVITE sip:ALLARME@172.21.160.117:5062 SIP/2.0");
							_tmpMess->compileMessage();
							_tmpMess->dumpVector();
							sl_cc->p_w(_tmpMess);
						} else {
							DEBOUT("ALMGR::alarmer operation", _tmpMess)
							sl_cc->p_w(_tmpMess);
						}
					} else {
						DEBY
						//TODO purge alarms
					}
					//else
					//time_alarm_mumap.erase(iter);
					mess_alm_map.erase(tmal->getMessage());
					delete tmal;
					DEBY
				}
			    time_alarm_mumap.erase(tcu);
				tcu = alarm_pq.top();
			}
		}
	}
}
void ALMGR::insertAlarm(MESSAGE* _message, SysTime _fireTime){

	//check if message already exists and cancel related alarm
	//do note remove it from multimap and from mess_alm map

	DEBOUT("ALMGR::insertAlarm", _fireTime.tv.tv_sec*1000000+_fireTime.tv.tv_usec)
	map<MESSAGE*, ALARM*>::iterator p;

	if (!mess_alm_map.empty()){
		DEBY
		p = mess_alm_map.find(_message);
		ALARM* tmp = (ALARM*)p->second;
		if (tmp != 0x0)
			tmp->cancel();
	}
	DEBY

	ALARM* alm = new ALARM(_message, _fireTime);
	// insert into priority q
	alarm_pq.push(alm->getTriggerTime());
	DEBY


	//insert into map time - alarm
	time_alarm_mumap.insert(pair<long long int, ALARM*>(alm->getTriggerTime(), alm));
	DEBY

	//insert or replace into map message - alarm
	//used to cancel an alarm by using the message pointer
	mess_alm_map.insert(pair<MESSAGE*, ALARM*>(_message, alm));
	DEBY

	string callid = _message->getHeadCallId().getNormCallId() +
			_message->getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");

	//pthread_mutex_lock(&mutex);
	callid_alm_map.insert(pair<string, ALARM*>(callid, alm));
	//pthread_mutex_unlock(&mutex);


	callid_message.insert(pair<string, MESSAGE*>(callid, _message));
	message_callid.insert(pair<MESSAGE*, string>(_message, callid));
	DEBY

	return;

}
//void ALMGR::cancelAlarm(MESSAGE* _message){
//
//	// alarm is deactivated and the related message may have been
//	// purged so
//
//	DEBOUT("ALMGR::cancelAlarm", _message)
//	//lookup alarm into map
//	map<MESSAGE*, ALARM*> ::iterator p;
//	p = mess_alm_map.find(_message);
//	ALARM* tmp = 0x0;
//	if (p != mess_alm_map.end()){
//		DEBOUT("ALMGR::cancelAlarm", "found")
//		tmp = (ALARM*)p->second;
//		tmp->cancel();
//	}
//}
void ALMGR::cancelAlarm(string _callid){

	// alarm is deactivated and the related message may have been
	// purged so

	DEBOUT("ALMGR::cancelAlarm", _callid)
	//lookup alarm into map
	//pthread_mutex_lock(&mutex);

	map<string, ALARM*> ::iterator p;
	p = callid_alm_map.find(_callid);
	ALARM* tmp = 0x0;
	if (p != callid_alm_map.end()){
		DEBOUT("ALMGR::cancelAlarm", "found")
		tmp = (ALARM*)p->second;
		tmp->cancel();
	}
	//pthread_mutex_unlock(&mutex);

}
//**********************************************************************************
ALARM::ALARM(MESSAGE *_message, SysTime _fireTime){

	message = _message;
	fireTime = _fireTime;
	active = true;
	fireTime_c = ((long long int) fireTime.tv.tv_sec)*1000000+(long long int)fireTime.tv.tv_usec;

}
long long int ALARM::getTriggerTime(void){
	return fireTime_c;
}
void ALARM::cancel(void){
	active = false;
}
MESSAGE* ALARM::getMessage(void){
	if (!active)
		return 0x0;
	else
		return message;
}
bool ALARM::isActive(void){
	return active;
}

