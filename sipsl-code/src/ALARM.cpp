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

	int counter;
	DEBOUT("ALMGR::alarmer", "begin")
	for(;;){

		nanosleep(&sleep_time,NULL);

		GETLOCK(&mutex,"mutex");

		//extract from priority queue
		SysTime mytime;
		GETTIME(mytime);
		unsigned long long int curr = ((unsigned long long int) mytime.tv.tv_sec)*1000000+(unsigned long long int)mytime.tv.tv_usec;
		unsigned long long int tcu = 0;
		if (!alarm_pq.empty()) {

			tcu = alarm_pq.top();



			while (!alarm_pq.empty() && curr >= tcu){

				DEBOUT("ALARM exists", alarm_pq.size())

				// now get a list of alarms from the multi map

				multimap<const unsigned long long int, ALARM*>::iterator iter;
				pair<multimap<const unsigned long long int,ALARM*>::iterator,multimap<const unsigned long long int ,ALARM*>::iterator> ret;
				map<ALARM*,int> delmap;
				DEBOUT("map<ALARM*,int> delmap", &delmap)

				ret = time_alarm_mumap.equal_range(tcu);

			    for (iter=ret.first; iter!=ret.second; ++iter){
					DEBY
					ALARM* tmal = iter->second;
					if (tmal->isActive()){

						MESSAGE* _tmpMess = tmal->getMessage();

						//ALMGR shall not care about message or internalop
						//SL_CC does it but here if for debug purposes

						if ( _tmpMess->typeOfInternal == TYPE_MESS ){
							DEBOUT("ALMGR::alarmer operation TYPE_MESS", _tmpMess)
							_tmpMess->setHeadSipRequest("INVITE sip:ALLARME@172.21.160.117:5062 SIP/2.0");
							_tmpMess->compileMessage();
							_tmpMess->dumpVector();

							RELLOCK(&mutex,"mutex");

							sl_cc->p_w(_tmpMess);
						} else {
							DEBOUT("ALMGR::alarmer operation TYPE_OP", _tmpMess)

							RELLOCK(&mutex,"mutex");

							sl_cc->p_w(_tmpMess);
						}
					}
					//mess_alm_map.erase(tmal->getMessage());
					delmap.insert(pair<ALARM*,int>(tmal,0));
					DEBOUT("ALARM CHECK INTERVAL", tmal << "][curr " << (unsigned long long int)curr << "][tcu " <<  (unsigned long long int)tcu)
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
		RELLOCK(&mutex,"mutex");

	}
}
void ALMGR::insertAlarm(MESSAGE* _message, SysTime _fireTime){

	//check if message already exists and cancel related alarm
	//do note remove it from multimap and from mess_alm map

	DEBOUT("ALMGR::insertAlarm", (unsigned long long int)_fireTime.tv.tv_sec*1000000 + (unsigned long long int)_fireTime.tv.tv_usec )

	GETLOCK(&mutex,"mutex");

	//DEBOUT("ALMGR::insertAlarm", _fireTime.tv.tv_sec*1000000+_fireTime.tv.tv_usec)
	SysTime mytime;
	GETTIME(mytime);
	unsigned long long int curr = ((unsigned long long int) mytime.tv.tv_sec)*1000000+(unsigned long long int)mytime.tv.tv_usec;
	DEBOUT("ALMGR::insertAlarm in ms", (double) ((unsigned long long int)_fireTime.tv.tv_sec*1000000 + (unsigned long long int)_fireTime.tv.tv_usec  - curr) / 1000000)


	map<MESSAGE*, ALARM*>::iterator p;

	NEWPTR(ALARM*, alm, ALARM(_message, _fireTime),"ALARM")

	// insert into priority q
	alarm_pq.push(alm->getTriggerTime());


	//insert into map time - alarm
	time_alarm_mumap.insert(pair<unsigned long long int const, ALARM*>(alm->getTriggerTime(), alm));


	string cidbranch_alarm;
	DEBY
	((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getContent();
	DEBY
	DEBOUT("((C_HeadVia&) _message->getSTKHeadVia().top()).getC_AttVia().getContent()",((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getContent())
	DEBY
	DEBOUT("((C_HeadVia&) _message->getSTKHeadVia().top()).getC_AttVia().getViaParms().getContent()",((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().getContent())
	cidbranch_alarm = _message->getHeadCallId().getContent() + ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") + "#" + _message->orderOfOperation+ "#";
	DEBOUT("Alarm id (cid+branch", cidbranch_alarm);
	cidbranch_alm_map.insert(pair<string, ALARM*>(cidbranch_alarm, alm));

	RELLOCK(&mutex,"mutex");

	return;

}
void ALMGR::cancelAlarm(string _cidbranch){

	// alarm is deactivated and the related message may have been
	// purged so

	GETLOCK(&mutex,"mutex");

	DEBOUT("ALMGR::cancelAlarm", _cidbranch)

	map<string, ALARM*> ::iterator p;
	p = cidbranch_alm_map.find(_cidbranch);
	ALARM* tmp = 0x0;
	if (p != cidbranch_alm_map.end()){
		DEBOUT("ALMGR::cancelAlarm", "found")
		tmp = (ALARM*)p->second;
		tmp->cancel();
	}
	else {
		DEBOUT("ALMGR::cancelAlarm", "not found")
	}
	RELLOCK(&mutex,"mutex");


}
//**********************************************************************************
ALARM::ALARM(MESSAGE *_message, SysTime _fireTime){

	message = _message;
	fireTime = _fireTime;
	active = true;
	fireTime_c = ((unsigned long long int) fireTime.tv.tv_sec)*1000000+(unsigned long long int)fireTime.tv.tv_usec;
	DEBOUT("ALARM::ALARM firetime", fireTime_c)
	cidbranch = _message->getHeadCallId().getContent() + ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _message->orderOfOperation+ "#";

}
unsigned long long int ALARM::getTriggerTime(void){
	return fireTime_c;
}
string ALARM::getCidbranch(void){
	return cidbranch;
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

