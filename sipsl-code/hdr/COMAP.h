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

#define COMAP_H

#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif

//map to store callId_X to call object
typedef map<string, CALL_OSET *> COMAP_MM;
//map to associate callId_Y to callId_X
typedef map<string, string> CALL_ID_Y2X;


//Call Object Map
class COMAP {

	private:
		COMAP_MM comap_mm;
		CALL_ID_Y2X call_id_y2x;

	public:
		CALL_OSET* getCALL_OSET_XMain(string callId_X);
		CALL_OSET* getCALL_OSET_YDerived(string callId_Y);
		void setCALL_OSET(string callId_X, CALL_OSET*);
		void setY2XCallId(string callId_Y, string callId_X);
		void deleteCALL_OSET(string callId_X);

		COMAP(void);
		~COMAP(void);

};
