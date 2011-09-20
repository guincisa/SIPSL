//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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

//Vendor ALO: the final application
//overrides ALO call backs
#ifndef VALO_H
#define VALO_H

class VALO : public ALO {

	private:

		// the To Tag that B sends back in 200ok
		// actually it is sent before in DE or 180 Ring but we store it here
		// since VALO only intercepts the 200 ok

		map<string, void*> ctxt_store;

		int dummy;
	public:
		VALO(ENGINE*, CALL_OSET*);
		~VALO(void);
		void onInvite(MESSAGE*);
		void onAck(MESSAGE*);
		void onBye(MESSAGE*);
		void on200Ok(MESSAGE*);
		void onAckNoTrnsct(MESSAGE*);

};
#endif
