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

/****************************************************
  ENGINE.h
*****************************************************/
#ifndef ENGINE_H
#define ENGINE_H

//**********************************************************************************
using namespace std;
//**********************************************************************************

//**********************************************************************************
class ENGINE;
class SUDP;
class SPINB;
class SPINC;
class SPINS;


//**********************************************************************************
typedef struct _ENGtuple {
    ENGINE * ps;
    int id;
    int mmod;
    const char* type;
} ENGtuple;
//**********************************************************************************
//**********************************************************************************
//ENGINE
//
//**********************************************************************************
//**********************************************************************************
class ENGINE {

	private:

		ThreadWrapper* parsethread[MAXTHREADS];

        SUDP* sudp;

        int modEngineMap(MESSAGE*);

        int EngineMaps;

        string objectType;

    public:

#ifdef TESTING
		virtual void parse(void* message);
#else
		virtual void parse(void* message, int _mmod) = 0;

#endif

        void linkSUDP(SUDP*);
        SUDP* getSUDP(void);

    	bool p_w(void* message);

#ifdef USE_SPINB
    	SPINB sb;
#else
    	SPINC* sb[MAXMAPS];

#endif

		ENGINE(int numthreads, int enginemaps, string type);

	    void lockBuffer(void);
	    void unLockBuffer(void);

	    void changeEngineMaps(int);

};
#endif

