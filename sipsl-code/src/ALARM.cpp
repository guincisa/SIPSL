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
ALMGR::ALMGR(void){
	DEBOUT("ALMGR::ALMGR", "alarm created")
}
void ALMGR::initAlarm(SL_CC* _sl_cc, timespec _sleep_time){

	DEBOUT("ALMGR::initAlarm", "init")
	sleep_time = _sleep_time;
	sl_cc = _sl_cc;

    listenerThread = new ThreadWrapper;
    ALMGRtuple *t1;
    t1 = new ALMGRtuple;
    t1->st = this;
    int res;
    res = pthread_create(&(listenerThread->thread), NULL, ALARMSTACK, (void *) t1 );
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
			DEBY
			tcu = alarm_pq.top();
			DEBY
			while (!alarm_pq.empty() && curr >= tcu){

				alarm_pq.pop();

				// now get a list of alarms from the multi map
				DEBY
				multimap<long long int, ALARM*>::iterator iter = time_alarm_mumap.find(tcu);
				DEBY
				while( iter != time_alarm_mumap.end() ) {
					DEBY
					ALARM* tmal = iter->second;
					tmal->getMessage()->setHeadSipRequest("INVITE sip:STRONZONE@172.21.160.117:5062 SIP/2.0");
					tmal->getMessage()->compileMessage();
					tmal->getMessage()->dumpVector();
					if (tmal->isActive()){
						DEBY
						sl_cc->p_w(tmal->getMessage());
					} else {
					}
					//else
					time_alarm_mumap.erase(iter);
					mess_alm_map.erase(tmal->getMessage());
					delete tmal;
					iter++;
				}
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
		p = mess_alm_map.find(_message);
		ALARM* tmp = (ALARM*)p->second;
		tmp->cancel();
	}

	ALARM* alm = new ALARM(_message, _fireTime);
	// insert into priority q
	alarm_pq.push(alm->getTriggerTime());

	//insert into map time - alarm
	time_alarm_mumap.insert(pair<long long int, ALARM*>(alm->getTriggerTime(), alm));

	//insert or replace into map message - alarm
	//used to cancel an alarm by using the message pointer
	mess_alm_map.insert(pair<MESSAGE*, ALARM*>(_message, alm));

	return;

}
void ALMGR::cancelAlarm(MESSAGE* _message){

	// alarm is deactivated and the related message may have been
	// purged so

	//lookup alarm into map
	map<MESSAGE*, ALARM*> ::iterator p;
	p = mess_alm_map.find(_message);
	ALARM* tmp = 0x0;
	if (p != mess_alm_map.end()){
		tmp = (ALARM*)p->second;
		tmp->cancel();
	}
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

