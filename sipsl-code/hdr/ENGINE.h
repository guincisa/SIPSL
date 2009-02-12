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

using namespace std;

class ENGINE;

typedef struct _ENGtuple {
    ENGINE * ps;
    int id;
} ENGtuple;

class ENGINE {

	private:

		ENGINE * instance;
		ENGINE(void);

    	SPINB sb;
		ThreadWrapper * parsethread[5];
		virtual void parse(MESSAGE) = 0;

    public:
    	void p_w(MESSAGE message);

};


