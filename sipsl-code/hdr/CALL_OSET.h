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

#ifndef ACTION_H
#include "ACTION.h"
#endif
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

//Umbrella class which hosts states machines and call object
class CALL_OSET {

	private:

		SL_CO* sl_co;
		ALO* alo;
		SL_SM_SV* sl_sm_sv;

		//map callId_y and related states machines
		//v4 map<string, SL_SM_CL*> mm_sl_sm_cl;
		ENGINE* engine;
		string callId_X;
	    //MESSAGE* genMessage;
	    MESSAGE* inviteA;

		//New client State Machine v4
		//map of call_y message, first list of outgoing invites
		//map<string, MESSAGE*> mm_genMessage_CL_v4;
		//The unique state machine client
		SL_SM_CL* sl_sm_cl;
		//The final call_y of the only confirmed invite
		string callId_Y_v4;
		//The final confirmed invite
		MESSAGE* inviteB;
		//MESSAGE* genMessage_CL_v4;


	public:
		CALL_OSET(ENGINE*, MESSAGE*);
		void setSL_X(string callId_X, SL_CO*, SL_SM_SV*, ALO*);
		SL_CO* getSL_CO(void);
		SL_SM_SV* getSL_SM_SV(void);
		//v4 void addSL_SM_CL(string callId_Y, SL_SM_CL*);

		string getCallIdX(void);

		//v4 SL_SM_CL* getSL_SM_CL(string callId_Y);
		ALO* getALO(void);
		ENGINE* getENGINE(void);

		MESSAGE* getInviteA(void);

		//New client State Machine v4
		//add call_y and message to the map
		//void addGenMess_CL_v4(string callIdY, MESSAGE*);
		//get MESSAGE* from the list using cally
		//MESSAGE* findGenMess_CL_v4(string callIdY);
		//get the only client state machine
		SL_SM_CL* getSL_SM_CL(void);
		//set the only sm
		void setSL_SM_CL(SL_SM_CL*);
		//set the final invite
		void setInviteB(MESSAGE*);
		//get the final invite
		MESSAGE* getInviteB(void);
		//clear all the other non confirmed invites
		void purgeGenMess_CL_v4(void);

		void setCall_IdY_v4(string _cally);
		string getCallId_Y_v4(void);
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
//class SL_SM2 {
//
//	public:
//		//SL_SM(ENGINE* sl_cc, SL_CO* sl_co, MESSAGE* generator);
//		SL_SM2(ENGINE* sl_cc, SL_CO* sl_co);
//
//		ENGINE* getSL_CC(void);
//		//MESSAGE* getGenerator(void);
//		SL_CO* getSL_CO(void);
//
//#ifdef TESTING
//		virtual ACTION* event(MESSAGE*);
//#else
//		virtual ACTION* event(MESSAGE*) = 0;
//#endif
//    protected:
//
//		//The Request message that has triggered the creation of the state machine
//		//MESSAGE* messageGenerator;
//
//        int State;
//		ENGINE* sl_cc;
//        SL_CO* sl_co;
//
//        //Mutex the state machine
//        //First option when every SM has its own mutex.
//        pthread_mutex_t mutex;
//};
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// New State machine
//**********************************************************************************
//**********************************************************************************
class SL_SM;
class PREDICATE_ACTION {

	private:
	SL_SM* machine;

	public:

	bool (*predicate)(SL_SM*, MESSAGE*);
	ACTION* (*action)(SL_SM*, MESSAGE*);

	PREDICATE_ACTION(SL_SM*);

};
class SL_SM {

	private:

    pthread_mutex_t mutex;

	multimap< int, PREDICATE_ACTION*> move_sm;

	ENGINE* sl_cc;
    SL_CO* sl_co;

    //Transaction control
    //CSeq
    //In case of server it is copied from the incoming Invite
    //In case of client it is set to 1
	int controlSequence;


	public:

	int getControlSequence(void);
	void setControlSequence(int);

	ENGINE* getSL_CC(void);
	//MESSAGE* getGenerator(void);
	SL_CO* getSL_CO(void);

	SL_SM(ENGINE* sl_cc, SL_CO* sl_co);

    int State; // initial 0, final -1

	void insert_move(int, PREDICATE_ACTION*);

	ACTION* event(MESSAGE*);

};
//**********************************************************************************
//**********************************************************************************
// State machine client (b side)
//**********************************************************************************
//**********************************************************************************
class SL_SM_CL : public SL_SM {

	public:
		int placeholder;

		int resend_invite;

		PREDICATE_ACTION P0_1CL;
		PREDICATE_ACTION P1_2CL;
		PREDICATE_ACTION P1_3CL;
		PREDICATE_ACTION P1_4CL;
		PREDICATE_ACTION P3_3CL;
		PREDICATE_ACTION P3_4CL;
		PREDICATE_ACTION P2_3CL;
		PREDICATE_ACTION P2_4CL;
		PREDICATE_ACTION P4_5CL;
		PREDICATE_ACTION P5_7CL;


		SL_SM_CL(ENGINE*, SL_CO*);

};
//**********************************************************************************
//**********************************************************************************
// State machine server (a side)
//**********************************************************************************
//**********************************************************************************
class SL_SM_SV : public SL_SM {

	public:
		int placeholder;

		PREDICATE_ACTION P0_1SV;
		PREDICATE_ACTION P1_2SV;
		PREDICATE_ACTION P2_2SV;
		PREDICATE_ACTION P2_3SV;
		PREDICATE_ACTION P1_3SV;
		PREDICATE_ACTION P3_4SV;
		PREDICATE_ACTION P4_6SV;


		SL_SM_SV(ENGINE*, SL_CO*);

};

