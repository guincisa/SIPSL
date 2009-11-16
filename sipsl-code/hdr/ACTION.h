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

#define ACTION_H

#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif

#define ACT_SEND 1
#define ACT_TIMERON 2
#define ACT_TIMEROFF 3

class SingleAction {
	private:
		MESSAGE *message;

		// 1 send embedded message
		// 2 start timer
		// 3 stop timer
		int driver;

		//For other actions
		//not a pointer
		//EMPTYMESSAGE emessage;

	public:
//		SingleAction(MESSAGE*);
//		SingleAction(EMPTYMESSAGE*);
		SingleAction(MESSAGE*, int driver);
		MESSAGE* getMessage(void);
		//EMPTYMESSAGE* getEmptyMessage(void);
		int getDriver(void);
};

class ACTION {

	private:

		stack<SingleAction> act_item;

	public:

		void addSingleAction(SingleAction);
		stack<SingleAction> getActionList(void);
		ACTION(void);

};
