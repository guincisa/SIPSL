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

/***********************************************************
  LOGGER.h
  writes to a rotative log
  must be single thread and single queue
************************************************************/
#define LOGGER_H
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
using namespace std;
class LOGGER;
class SPINC;

//**********************************************************************************
typedef struct _LOGtuple {
    LOGGER * ps;
    int id;
    const char* type;
} LOGtuple;
//**********************************************************************************
//**********************************************************************************
class LOGGER {

	private:

		int lines;
		int rotations;

		int currline;
		int currotation;

		ThreadWrapperLog* parsethread;

		ofstream* logFile[MAXFILES];

		//only some will be always defined in UTIL and can be activated on line
		//others will require compilation
		//bool activeLog[MAXLOGTYPES];

    public:

		virtual void logit(char* message);


    	void p_w(char* message);

    	SPINC* sb;

		LOGGER(int lines, int rotations);

		//log methods
		//...

};


