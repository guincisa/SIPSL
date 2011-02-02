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
		lli fireTime;

		//Are sent to SL_CC by default
		MESSAGE* message;

		bool active;

                string cidbranch;

	public:
            ALARM(void);
            ALARM(MESSAGE* message, lli fireTime);
            lli getTriggerTime(void);
            void cancel(void);
            MESSAGE* getMessage(void);
            bool isActive(void);
            string getCidbranch(void);

};

struct triple {
    lli time;
    string cid;
    ALARM* alarm;
};

class CompareTriple {
    public:
    bool operator() (triple& t1, triple& t2) {
        if (t1.time >= t2.time){
            return true;
        }
        else {
            return false;
        }
    }
};

class ALMGR {

    private:

        priority_queue<triple, vector<triple>, CompareTriple> pq;
        map<string,  ALARM* > cidmap;

        timespec sleep_time;
        SL_CC* sl_cc;

        ThreadWrapper *listenerThread;

        pthread_mutex_t mutex;

        void internalCancelAlarm(string cidbranch);

   public:
        void alarmer(void);

       //Alarm manager in a separate thread
       ALMGR(SL_CC* sl_cc, timespec sleep_time);
       ALMGR(SL_CC* sl_cc, __time_t sec, long int nsec);

        void initAlarm(void);
        void insertAlarm(MESSAGE* message, lli fireTime);

        void cancelAlarm(string cid_branch);
        //void cancelAlarm(MESSAGE* message);

        void purgeAlarm(ALARM*);


        //Case in cui il messaggio è MainMessage
        //1. inserisco un allarme: alarm map <xyz,a1>
        //   - inserisco il trigger time nella priority queue pq<time1>
        //   - inserisco il trigger time e allarme nella multimap mmu<time1,a1>
        //2. reinserisco xyz
        //   a1 è disattivato a1->cancel
        //3. la sl_co va in cancellazione
        //   l'allarme è disattivato solo se il messaggio da cancellare è un type_op
        //   tutti i type_op gia eliminati non possono disattivare l'allarme
        //
};





