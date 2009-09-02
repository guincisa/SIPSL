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

#ifndef ALARM_H
#include "ALARM.h"
#endif

ALARM::ALARM(MESSAGE *_message, SysTime _fireTime){

	message = _message;
	fireTime = _fireTime;
	active = true;
	long long int _fireTime_c = ((long long int) fireTime.tv.tv_sec)*1000000+(long long int)fireTime.tv.tv_usec;

}
long long int ALARM::getTriggerTime(void){

	return _fireTime;
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
void ALMGR::initAlarm(SL_CC* _sl_cc, long int _nano_delta){

	sleep_time = _nano_delta;
	sl_cc = _sl_cc;

    //res = pthread_create(&alarmer, NULL, NULL, NULL );

}
void ALMGR::alarmer(void){

	for(;;){
		// sleep

		//extract from priority queue
		SysTime mytime;
		GETTIME(mytime);
		long long int curr = ((long long int) mytime.tv.tv_sec)*1000000+(long long int)mytime.tv.tv_usec;
		long long int tcu = 0;
		while (!alarm_pq.empty() && curr >= tcu){
			tcu = alarm_pq.top();

			// now get a list of alarms from the multi map
				// for every alarm check if is it active
				// check if message != 0x0
				// if active invoke sl_cc->p_w(message)
				// if not active free ALARM pointer
				// remove from the multimap all the loaded alarms

			alarm_pq.pop();
		}
	}
}
void ALMGR::insertAlarm(MESSAGE* _message, SysTime _fireTime){

	//check if message already exists and cancel related alarm
	//??? non ricordo....
	//do note remove it from multimap and from mess_alm map
	ALARM* alm = new ALARM(_message, _fireTime);
	// insert into priority q
	alarm_pq.push(alm->getTriggerTime());

	//insert into map time - alarm
	time_alarm_mumap.insert(pair<long long int, ALARM*>(alm->getTriggerTime(), alm));

	(???)
	mess_alm_map.insert(pair<ALARM*, MESSAGE*>(alm, _message));


	//mess_alm_map.insert(make_pair(_message, alm));
	// calculate real fire time
	// insert into multimap


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

