//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer 
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

#define ALARM_H

#include <queue>
#include <map>
#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef SL_CC_H
#include "SL_CC.h"
#endif


class ALARM {

	private:
		SysTime fireTime;
		long long int fireTime_c;

		//fireTime.tv.tv_sec*1000000 + fireTime+endtime.tv.tv_usec

		//Are sent to SL_CC by default
		MESSAGE* message;

		bool active;

	public:
		ALARM(MESSAGE* message, SysTime fireTime);
		long long int getTriggerTime(void);
		void cancel(void);
		MESSAGE* getMessage(void);
		bool isActive(void);
};

class ALMGR {

	private:

		//Store alarm in multimap, the key is the time rounded to next trigger time
	    //Example if trigger time is every 20 ms and the next alarm are at a1=t0+2,  a2=t0+7, a3=t0+25
		// the key are:
		// a1 -> 20
		// a2 -> 20
		// a3 -> 40

		priority_queue<long long int, vector<long long int>, greater<long long int> > alarm_pq;

		multimap<long long int, ALARM*> time_alarm_mumap;
		timespec sleep_time;
		SL_CC* sl_cc;

		// This is used when I was to clear the alarm related to message
		// to cancel an alarm I use the MESSAGE*
		map<MESSAGE*, ALARM*> mess_alm_map;

		//string callidy = _message->getHeadCallId().getNormCallId() +
		//		_message->getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");

		map<string, ALARM*> callid_alm_map;
		map<string, MESSAGE*> callid_message;
		map<MESSAGE*, string> message_callid;

        ThreadWrapper *listenerThread;


	public:
		void alarmer(void);

		//Alarm manager in a separate thread
		ALMGR(void);
		void initAlarm(SL_CC* sl_cc, timespec sleep_time);
		void insertAlarm(MESSAGE* message, SysTime fireTime);

		void cancelAlarm(string call_id);
		void cancelAlarm(MESSAGE* message);

		void purgeAlarm(ALARM*);

};



//#include <pthread.h>
//#include <string>
//#include <iostream.h>
//#include <memory>
//#include "COMM.h"
//
//class ALARM {
//    private:
//        // alarmId is not unique, in this case the one with lower timetrigger is
//        // to be discarded
//        //  original timeout
//        int timeout;
//        // real time to trigger
//        int timetrigger;
//
//        // one or both
//        CO * callobject;
//        string callid;
//
//    public:
//        //unique ID machine time when it has bee generated
//        double int alarmId;
//        // first parm is alarmId
//        void setAlarm(double int,int,CO*);
//        void setAlarm(double int,int,string);
//
//        void setTimeTrigger(int);
//        int getTimeout(void);
//        int getTimeTrigger(void);
//        CO* getCO(void);
//        string getCID(void);
//}
//
//class CMPALM {
//  public:
//    bool operator()(const ALARM x, const ALARM y) const{ return x.getTimeTrigger() > y.getTimeTrigger(); }
//};
//// alarmId,  Alarm*
//// alarmId may correspond to more than one alarm
//// but the only valid alarm is the one pointed in the table
////typedef map<double int, ALARM*> ValidAlarmMap;
////timetrigger
//typedef map<double int, int> ValidAlarmMap;
//
//typedef ValidAlarmMap::value_type VAMvalue;
//
//class ALARMQ {
//
//    ValidAlarmMap vam;
//    priority_queue<ALARM,vector<ALARM>,CMPALM> pq;
//
//
//    public:
//    void insertAlarm(ALARM);
//    ALARM getAlarm(double int);
//};
//
//void ALARMQ::insertAlarm(ALARM a){
//    int now = gethrtime()/1000/1000/1000;
//    int to = now + a.getTimeout();
//    cout << "Inserting " << to << " ID " << a.getAlarmId() <<endl;
//    a.setTimeTrigger(to);
//    pq.push(a);
//    vam.insert(a.getAlarmId(),&a);
//    return;
//}
//ALARM ALARMQ::getAlarm(double int now){
//
//    ALARM tal;
//    MESSAGE mtal;
//
//    if (pq.empty()){
//        ex.id = 0;
//        ex.message="";
//        throw ex;
//    }
//    while (true) {
//        tal = pq.top();
//        if (tal.to <= now) {
//            cout << "Popping " << tal.to << " " << tal.id << endl;
//            pq.pop();
//            int al = vam.get(tal.getAlarmId());
//            if (al != tal.timetrigger) {
//                //skip alarm
//
//        }
//        else {
//            throw ex;
//        }
//    }
//}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////SPINBUFFER ALARM
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//class SPINBALM;
/////////////////////////////////////////////////////////////////////////////////
//class ROTQALM {
//    private:
//        ALARM Q[ARR];
//        int top,bot; // da scrivere, da leggere
//        int state;
//        SPINBALM *sb;
//
//    public:
//        ROTQALM(void);
//        void setSpinb(SPINBALM *);
//        void setState(int);
//        int getState(void);
//        void put(ALARM);
//        ALARM get(void);
//        bool isEmpty(void);
//};
/////////////////////////////////////////////////////////////////////////////////
//class SPINBALM {
//    private:
//
//    //queue<MESSAGE> Q0,Q1,Q2;
//    ROTQALM Q[3];
//
//    //int state[3]; // 0 free, 1 write, 2 read
//    int readbuff, writebuff, freebuff;
//
//    public:
//
//    int DIM;
//    pthread_mutex_t readmu;
//    pthread_mutex_t writemu;
//
//    pthread_mutex_t mudim;
//    pthread_mutex_t condvarmutex;
//    pthread_cond_t condvar;
//
//    SPINBALM(void);
//    void put(ALARM);
//    ALARM get(void);
//    void move(void);
//    bool isEmpty(void);
//
//};
/////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////
//// mono threaded RATYPE for alarms
////////////////////////////////////////////////////////////
//class MTRATYPE;
//typedef struct tuple2 {
//    MTRATYPE * ps;
//    double int id;
//} MTRTAtuple;
//
////Mono Thread Ratype
//class MTRATYPE {
//    //TODO construtor mutex
//    public:
//    MTRATYPE();
//
//    //SPINBUFFER
//    SPINBALM sb;
//
//    //MESSAGE message[5];
//    void prepareandwork(ALARM);
//
//    ThreadWrapper * parsethread;
//    //virtual void parse(int);
//    void parse(ALARM);
//};
//

