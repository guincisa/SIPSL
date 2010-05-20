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

class TRNSCT_SM;

//Umbrella class which hosts states machines and call object
class CALL_OSET {

	private:

		SL_CO* sl_co;
		ALO* alo;

		//SL_SM_SV* sl_sm_sv;

		//map callId_y and related states machines
		//v4 map<string, SL_SM_CL*> mm_sl_sm_cl;
		ENGINE* engine;
		string callId_X;

		//The final call_y of the only confirmed invite
		string callId_Y_v4;

		// New client State Machine v4
		// map of call_y message, first list of outgoing invites
		// map<string, MESSAGE*> mm_genMessage_CL_v4;
		// The unique state machine client
		//SL_SM_CL* sl_sm_cl;

		// TRANSACTION MANAGEMENT
		// there are server and client side state machines
		// a machine for every new request even inside the same call
		// KEY is like this
		// <REQUESTTYPE>#<SIDE>#<SEQUENCE>
		// example 1#A#1
		map<string, TRNSCT_SM*> trnsctSmMap;

		// Sequence map
		// used to store used sequences and return new unused sequences
		map<string, int> sequenceMap;
		// current sequence, shared among all request
		int currentSequence;

		// Get new transaction state machine
		TRNSCT_SM* newTrnsct(string method, string side);

	public:

		// TRANSACTION MANAGEMENT
		CALL_OSET(ENGINE*);

		// add transaction state machine
		void addTrnsctSm(string key, TRNSCT_SM* trnsctSm);
		// get transaction state machine
		TRNSCT_SM* addTrnsctSm(string key);







		void setSL_X(string callId_X, SL_CO*, SL_SM_SV*, ALO*);
		SL_CO* getSL_CO(void);
		SL_SM_SV* getSL_SM_SV(void);
		//v4 void addSL_SM_CL(string callId_Y, SL_SM_CL*);

		string getCallIdX(void);

		ENGINE* getENGINE(void);


		//v4 SL_SM_CL* getSL_SM_CL(string callId_Y);
		ALO* getALO(void);
		//New client State Machine v4
		//add call_y and message to the map
		//void addGenMess_CL_v4(string callIdY, MESSAGE*);
		//get MESSAGE* from the list using cally
		//MESSAGE* findGenMess_CL_v4(string callIdY);
		//get the only client state machine
		SL_SM_CL* getSL_SM_CL(void);
		//set the only sm
		void setSL_SM_CL(SL_SM_CL*);
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

	public:

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
		PREDICATE_ACTION P7_8CL;



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
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// V5
// new state machine top class
// transaction state machine
// call state machine
//**********************************************************************************
//**********************************************************************************
class PREDICATE_ACTION_V5;
class SM_V5 {

	private:
		pthread_mutex_t mutex;
		multimap< int, PREDICATE_ACTION_V5*> move_sm;

		ENGINE* sl_cc;
	    SL_CO* sl_co;

	public:
	    int State; // initial 0, final -1
		void insert_move(int, PREDICATE_ACTION_V5*);
		ACTION* event(MESSAGE*);

		ENGINE* getSL_CC(void);
		SL_CO* getSL_CO(void);
		SM_V5(ENGINE* sl_cc, SL_CO* sl_co);

};
class PREDICATE_ACTION_V5 {

	private:
	SM_V5* machine;

	public:

	bool (*predicate)(SM_V5*, MESSAGE*);
	ACTION* (*action)(SM_V5*, MESSAGE*);

	PREDICATE_ACTION_V5(SM_V5*);

};
//**********************************************************************************
//**********************************************************************************
// Transaction state machine
//**********************************************************************************
//**********************************************************************************
class TRNSCT_SM  : SM_V5{

	private:

		int requestType;
		MESSAGE* Matrix;

	public:

		void setMatrixMessage(MESSAGE*);
		MESSAGE* getMatrixMessage(void);

		//#define REGISTER_REQUEST 1
		//#define INVITE_REQUEST 2
		//#define ACK_REQUEST 3
		//#define BYE_REQUEST 4
		//#define CANCEL_REQUEST 5

		//Depending on the request the SM will be different
		TRNSCT_SM(int requestType, MESSAGE* matrixMess, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
//**********************************************************************************
// Call/Dialog state machine
//**********************************************************************************
//**********************************************************************************
class CALL_SM  : SM_V5{

	private:

	vector<TRNSCT_SM*> trnsct_sm_array;

	public:

};

