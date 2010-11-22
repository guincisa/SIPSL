//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
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

/****************************************************
  ENGINE.h
*****************************************************/
#define ENGINE_H

//**********************************************************************************
using namespace std;
//**********************************************************************************

//**********************************************************************************
class ENGINE;
class SUDP;
class SPINB;
class SPINC;


//**********************************************************************************
typedef struct _ENGtuple {
    ENGINE * ps;
    int id;
} ENGtuple;
//**********************************************************************************
//**********************************************************************************
//ENGINE
//
//**********************************************************************************
//**********************************************************************************
class ENGINE {

	private:

		ThreadWrapper* parsethread[64];

        SUDP* sudp;

    public:

#ifdef TESTING
		virtual void parse(MESSAGE* message);
#else
		virtual void parse(MESSAGE* message) = 0;
#endif

        void linkSUDP(SUDP*);
        SUDP* getSUDP(void);

    	void p_w(MESSAGE* message);
#ifdef USE_SPINB
    	SPINB sb;
#else
    	SPINC sb;
#endif

		ENGINE(int numthreads, int spintype);
};


