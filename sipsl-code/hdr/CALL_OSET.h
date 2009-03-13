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

#define CALL_OSET_H

/**********************************************************************************
 * CALL_OSET is :
 * 1:1 SL_CO
 * 1:1 SL_SM_SV
 * 0:N SL_SM_CL (N is because it may have more associated call legs, it's a multimap
 * with key = callid
 *
 **********************************************************************************/
class SL_SM_CL;
class SL_SM_SV;
class SL_CO;
class ALO;

class CALL_OSET {

	private:
		SL_CO* sl_co;
		ALO* alo;
		SL_SM_SV* sl_sm_sv;
		multimap<string, SL_SM_CL*> mm_sl_sm_cl;
		ENGINE* engine;
		string callId_X;

	public:
		CALL_OSET(ENGINE*);
		void setSL_X(string callId_X, SL_CO*, SL_SM_SV*, ALO*);
		SL_CO* getSL_CO(void);
		SL_SM_SV* getSL_SM_SV(void);
		void addSL_SM_CL(string callId_Y, SL_SM_CL*);
		SL_SM_CL* getSL_SM_SL(string callId_Y);
		ALO* getALO(void);
};
//**********************************************************************************
//**********************************************************************************
// Call object
//**********************************************************************************
//**********************************************************************************
class SL_CO {
	public:
		int placeholder;

		SL_CO(CALL_OSET*);

		CALL_OSET* call_oset;

		void call(MESSAGE*);
};
//**********************************************************************************
//**********************************************************************************
// State machine
//**********************************************************************************
//**********************************************************************************
class SL_SM {
	public:
		int placeholder;

		ACTION event(MESSAGE*);

//    protected:
//
//        int State;
//    public:
//        SL_SM();
//        int getState (void);
//
//        virtual MESSAGE * event(MESSAGE&);
};
//**********************************************************************************
//**********************************************************************************
// State machine client (b side)
//**********************************************************************************
//**********************************************************************************
class SL_SM_CL : public SL_SM {
	public:
		int placeholder;

//    public:
//    SL_SM_CL();
//	// array of pointers to messages
//    MESSAGE ** event(MESSAGE&);
//    int dummy;
};
//**********************************************************************************
//**********************************************************************************
// State machine server (a side)
//**********************************************************************************
//**********************************************************************************
class SL_SM_SV : public SL_SM {
	public:
		int placeholder;
//    public:
//    SL_SM_SV();
//	// array of pointers to messages
//    MESSAGE ** event(MESSAGE&);
//    int dummy;
};
