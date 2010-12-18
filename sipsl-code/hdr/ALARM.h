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

class ALARM {

	private:
		unsigned long long int fireTime;

		//fireTime.tv.tv_sec*1000000 + fireTime+endtime.tv.tv_usec

		//Are sent to SL_CC by default
		MESSAGE* message;

		bool active;

		string cidbranch;


	public:
		ALARM(MESSAGE* message, unsigned long long int fireTime);
		unsigned long long int getTriggerTime(void);
		void cancel(void);
		MESSAGE* getMessage(void);
		bool isActive(void);
		string getCidbranch(void);
};
//struct cmpr_longlong {
//    bool operator()(long long int const a, long long int const b){
//        return a < b;
//    }
//};
class ALMGR {

	private:

		//Store alarm in multimap, the key is the time rounded to next trigger time
	    //Example if trigger time is every 20 ms and the next alarm are at a1=t0+2,  a2=t0+7, a3=t0+25
		// the key are:
		// a1 -> 20
		// a2 -> 20
		// a3 -> 40


		priority_queue<const unsigned long long int, vector<unsigned long long int>, greater<const unsigned long long int> > alarm_pq;


		//TODO need to change this to
		//multimap<time, cidstring>
		multimap<const unsigned long long int, ALARM*, less<const unsigned long long int> > time_alarm_mumap;

		timespec sleep_time;
		SL_CC* sl_cc;


		map<string, ALARM*> cidbranch_alm_map;

        ThreadWrapper *listenerThread;

        pthread_mutex_t mutex;

        void internalCancelAlarm(string cidbranch);


	public:
		void alarmer(void);

		//Alarm manager in a separate thread
		ALMGR(SL_CC* sl_cc, timespec sleep_time);
		ALMGR(SL_CC* sl_cc, __time_t sec, long int nsec);

		void initAlarm(void);
		void insertAlarm(MESSAGE* message, unsigned long long int fireTime);

		void cancelAlarm(string cid_branch);
		//void cancelAlarm(MESSAGE* message);

		void purgeAlarm(ALARM*);

};





