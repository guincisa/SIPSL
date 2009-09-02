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

//**********************************************************************************
void ALMGR::initAlarm(SL_CC* _sl_cc, timespec _sleep_time){

	sleep_time = _sleep_time;
	sl_cc = _sl_cc;

    listenerThread = new ThreadWrapper;
    ALMGRtuple *t1;
    t1 = new ALMGRtuple;
    t1->st = this;
    int res;
    res = pthread_create(&(listenerThread->thread), NULL, ALARMSTACK, (void *) t1 );
    return;


}
void ALMGR::alarmer(void){

	for(;;){

		nanosleep(&sleep_time,NULL);

		//extract from priority queue
		SysTime mytime;
		GETTIME(mytime);
		long long int curr = ((long long int) mytime.tv.tv_sec)*1000000+(long long int)mytime.tv.tv_usec;
		long long int tcu = 0;
		tcu = alarm_pq.top();
		while (!alarm_pq.empty() && curr >= tcu){

			alarm_pq.pop();
			tcu = alarm_pq.top();

			// now get a list of alarms from the multi map
				// for every alarm check if is it active
				// check if message != 0x0
				// if active invoke sl_cc->p_w(message)
				// if not active free ALARM pointer
				// remove from the multimap all the loaded alarms

		}
	}
}
void ALMGR::insertAlarm(MESSAGE* _message, SysTime _fireTime){

	//check if message already exists and cancel related alarm
	//do note remove it from multimap and from mess_alm map

	map<MESSAGE*, ALARM*>::iterator p;
	p = mess_alm_map.find(_message);
	if (p != mess_alm_map.end()){
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

