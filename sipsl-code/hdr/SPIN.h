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
#ifndef SPIN_H
#define SPIN_H

//SPINBUFFER
#define SPIN_WW  2
#define SPIN_RR  1
#define SPIN_FF  0
//#define USE_SPINB
#define SPIN_TRASH 1
//Block mode will block the sl_co because it constantly back feeds itself
//#define SPIN_BLOCK 2

#ifndef SPINSTL
class SPINC {

    private:
        void* BUFF[ARR];

        int l, s, DIM;

        pthread_mutex_t readmu;
        pthread_mutex_t writemu;
        pthread_mutex_t dimmu;
        pthread_mutex_t buffmu[SPINC_MOD];

        bool forcedState;

    public:
        bool put(void*);
        void* get(void);
        bool isEmpty(void);
        SPINC(void);

        pthread_mutex_t condvarmutex;
        pthread_cond_t condvar;

        void lockBuffer(void);

        void unLockBuffer(void);

};
class SPINS {

    private:
            void* BUFF[ARR_SHORT];

            bool forcedState;

            int l, s, DIM;

        pthread_mutex_t readmu;
        pthread_mutex_t writemu;
        pthread_mutex_t dimmu;
        pthread_mutex_t buffmu[SPINC_MOD];

    public:
        bool put(void*);
        void* get(void);
        bool isEmpty(void);
        SPINS(void);

        pthread_mutex_t condvarmutex;
        pthread_cond_t condvar;

        void lockBuffer(void);

        void unLockBuffer(void);


};
#else

//Using this currently
#include <queue>
using namespace std;
class SPINC {

    private:

		std::queue<void*> Q;
		//pthread_mutex_t spinm;

    public:
        bool put(void*);
        void* get(void);
        bool isEmpty(void);
        SPINC(void);

        pthread_mutex_t condvarmutex;
        pthread_cond_t condvar;

        void lockBuffer(void);

        void unLockBuffer(void);

};
class SPINS {

    private:

		std::queue<void*> Q;
		pthread_mutex_t spinm;

    public:
        bool put(void*);
        void* get(void);
        bool isEmpty(void);
        SPINS(void);

        pthread_mutex_t condvarmutex;
        pthread_cond_t condvar;

        void lockBuffer(void);

        void unLockBuffer(void);

};
#endif
#endif
