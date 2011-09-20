//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer 
// Copyright (C) 2011 Guglielmo Incisa di Camerana
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
#define ALARM_H

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



//New double linked list for alarms
// inserts from higher element
// reads from lower element
// mutex only for insert/cancel
// no mutex for trigger because insert/cancel will not interfere

struct pqelm{
    triple element;
    pqelm* left;
    pqelm* right;
};

class PQ {
    public:
        pqelm* L;
        pqelm* R;

        void pop(void);
        triple top(void);
        void push(triple);

        bool empty(void);

        PQ();
};



#ifdef ALARMENGINE
class ALMGR : public ENGINE {
#else
class ALMGR {
#endif

    private:

        priority_queue<triple, vector<triple>, CompareTriple> pq[ALARMMAPS];
        //PQ pq;
        map<string,  ALARM* > cidmap[ALARMMAPS];

        timespec sleep_time;
        SL_CC* sl_cc;

        ThreadWrapper *listenerThread[ALARMMAPS];

        pthread_mutex_t mutex[ALARMMAPS];

        void internalCancelAlarm(string cidbranch, int modulus);

        int insertAlarmPrivate(MESSAGE* message, lli fireTime, int modulus);

   public:
        void alarmer(int);

       //Alarm manager in a separate thread
#ifdef ALARMENGINE
       ALMGR(int th, int map, string obname, SL_CC* sl_cc, timespec sleep_time);
       ALMGR(int th, int map, string obname, SL_CC* sl_cc, __time_t sec, long int nsec);
#else
       ALMGR(SL_CC* sl_cc, timespec sleep_time);
       ALMGR(SL_CC* sl_cc, __time_t sec, long int nsec);
#endif

        void initAlarm(void);

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
#ifdef ALARMENGINE
        void parse(void *, int);
        void cancelAlarm(MESSAGE* message);
#else
        void cancelAlarm(string cid_branch, int modulus);

#endif
        void insertAlarm(MESSAGE* message, lli fireTime);
};
#endif
