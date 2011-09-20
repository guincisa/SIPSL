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
#ifndef ACTION_H
#define ACTION_H

class MESSAGE;


class SingleAction {
	private:
		MESSAGE *message;

	public:
		//Default is to send the message
		SingleAction(MESSAGE*);

		MESSAGE* getMessage(void);

};

class ACTION {

	private:

		stack<SingleAction> act_item;

	public:

		void addSingleAction(SingleAction);
		stack<SingleAction> getActionList(void);
		ACTION(void);

};
#endif
