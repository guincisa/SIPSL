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

/****************************************************
  ENGINE.h
*****************************************************/
#define ENGINE_H

#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif

class ThreadWrapper {
    public:
        pthread_t thread;
        pthread_mutex_t mutex;
        ThreadWrapper();
};



using namespace std;

class ENGINE;

typedef struct _ENGtuple {
    ENGINE * ps;
    int id;
} ENGtuple;

class ENGINE {

	private:

		ENGINE * instance;



		ThreadWrapper * parsethread[5];

    public:

#ifdef TESTING
		virtual void parse(MESSAGE message);
#else
		virtual void parse(MESSAGE) = 0;
#endif


    	void p_w(MESSAGE message);

    	SPINB sb;

		ENGINE(void);
};


