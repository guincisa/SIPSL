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

class SL_CO;
class ALO;
class SL_SM;
class TRNSCT_SM;

//Umbrella class which hosts states machines and call object
class CALL_OSET {

	private:

		SL_CO* sl_co;
		ALO* alo;
		ENGINE* engine;

		// TRANSACTION MANAGEMENT
		// KEY is like this
		// <REQUESTTYPE>#<SIDE>#<BRANCH>
		// example 1#A#1
		map<string, TRNSCT_SM*> trnsctSmMap;

		// Sequence map
		// used to store used sequences and return new unused sequences
		map<string, int> sequenceMap;
		// current sequence, shared among all request
		int currentSequence;

		//list of keys of messages created in call oset
		stack<string> messageKeys;

		// Get new transaction state machine
		TRNSCT_SM* newTrnsct(string method, string side);


		//From V4
		string callId_X;
		string callId_Y;



	public:

		void insertMessageKey(string key);

		// TRANSACTION MANAGEMENT
		CALL_OSET(ENGINE*);
		~CALL_OSET(void);

		// add transaction state machine
		void addTrnsctSm(string method, int sode, string branch, TRNSCT_SM* trnsctSm);
		// get transaction state machine
		TRNSCT_SM* getTrnsctSm(string method, int sode, string branch);

		void setSL_CO(SL_CO*);
		SL_CO* getSL_CO(void);

		ENGINE* getENGINE(void);

		ALO* getALO(void);
		void setALO(ALO*);

		//From V4
		void setCallId_Y(string _cally);
		string getCallId_Y(void);
		void setCallId_X(string callId_X);
		string getCallId_X(void);

		//TODO is wrong
		int getNextSequence(string method);
		int getCurrentSequence(string method);
		void insertSequence(string method, int cntr);


};
//**********************************************************************************
//**********************************************************************************
// Call object
//**********************************************************************************
//**********************************************************************************
class SL_CO {

	private:
	//pthread_mutex_t mutex;

	public:
		int placeholder;

		SL_CO(CALL_OSET*);

		CALL_OSET* call_oset;

		void call(MESSAGE*);

};

//**********************************************************************************
//**********************************************************************************
// V5 Transaction State machines
//**********************************************************************************
//**********************************************************************************
class PREDICATE_ACTION_V5;
class SM_V5 {

	private:
		pthread_mutex_t mutex;
		multimap< const int, PREDICATE_ACTION_V5*> move_sm;

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
//**********************************************************************************
class TRNSCT_SM  :  public SM_V5{

	private:

		int requestType;
		//Matrix is the request which triggers the creation of the State Machine
		MESSAGE* Matrix;
		//A Matrix is the a-side request which triggers the above request
		//it differs from the above message only in case of client sm
		MESSAGE* A_Matrix;

	public:

		MESSAGE* getMatrixMessage(void);

		MESSAGE* getA_Matrix(void);


		//#define REGISTER_REQUEST 1
		//#define INVITE_REQUEST 2
		//#define ACK_REQUEST 3
		//#define BYE_REQUEST 4
		//#define CANCEL_REQUEST 5

		TRNSCT_SM(int requestType, MESSAGE* matrixMess, MESSAGE* A_Matrix, ENGINE* sl_cc, SL_CO* sl_co);


};
//**********************************************************************************
class PREDICATE_ACTION_V5 {

	private:
	SM_V5* machine;

	public:

	bool (*predicate)(SM_V5*, MESSAGE*);
	ACTION* (*action)(SM_V5*, MESSAGE*);

	PREDICATE_ACTION_V5(SM_V5*);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE INVITE SERVER
//**********************************************************************************
class TRNSCT_SM_INVITE_SV : public TRNSCT_SM {

	public:

		PREDICATE_ACTION_V5 PA_INV_0_1SV;
		PREDICATE_ACTION_V5 PA_INV_1_2SV;
		PREDICATE_ACTION_V5 PA_INV_1_3SV;
		PREDICATE_ACTION_V5 PA_INV_3_4SV;

		TRNSCT_SM_INVITE_SV(int requestType, MESSAGE* matrixMess, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE INVITE CLIENT
//**********************************************************************************
class TRNSCT_SM_INVITE_CL : public TRNSCT_SM {

	private:
		//TODO move this to upper class
		MESSAGE* A_Matrix;

	public:

		PREDICATE_ACTION_V5 PA_INV_0_1CL;
		PREDICATE_ACTION_V5 PA_INV_1_1CL; //resend invite
		PREDICATE_ACTION_V5 PA_INV_1_1bCL; //100 try incoming
		PREDICATE_ACTION_V5 PA_INV_1_3CL; //100 try not arriving, 180 ring or Diag Estab instead
		PREDICATE_ACTION_V5 PA_INV_1_99CL; //resend invite max reached
		PREDICATE_ACTION_V5 PA_INV_1_4CL; //200OK
		PREDICATE_ACTION_V5 PA_INV_4_5CL; //Non transactional ACK


		int resend_invite;

		TRNSCT_SM_INVITE_CL(int requestType, MESSAGE* matrixMess, MESSAGE* A_Matrix, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE ACK SERVER
//**********************************************************************************
class TRNSCT_SM_ACK_SV : public TRNSCT_SM {

	public:

		PREDICATE_ACTION_V5 PA_ACK_0_1SV;


		TRNSCT_SM_ACK_SV(int requestType, MESSAGE* matrixMess, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE ACK CLIENT
//**********************************************************************************
class TRNSCT_SM_ACK_CL : public TRNSCT_SM {

	private:
		//TODO move this to upper class
		MESSAGE* A_Matrix;

	public:

		PREDICATE_ACTION_V5 PA_ACK_0_1CL;


		TRNSCT_SM_ACK_CL(int requestType, MESSAGE* matrixMess, MESSAGE* A_Matrix, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE BYE SERVER
//**********************************************************************************
class TRNSCT_SM_BYE_SV : public TRNSCT_SM {

	public:

		PREDICATE_ACTION_V5 PA_BYE_0_1SV;
		PREDICATE_ACTION_V5 PA_BYE_1_2SV;


		TRNSCT_SM_BYE_SV(int requestType, MESSAGE* matrixMess, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE BYE CLIENT
//**********************************************************************************
class TRNSCT_SM_BYE_CL : public TRNSCT_SM {

	private:
		//TODO move this to upper class
		MESSAGE* A_Matrix;

	public:

		int resend_bye;

		PREDICATE_ACTION_V5 PA_BYE_0_1CL;
		PREDICATE_ACTION_V5 PA_BYE_1_1CL; //resend bye
		PREDICATE_ACTION_V5 PA_BYE_1_2CL; //200OK

		TRNSCT_SM_BYE_CL(int requestType, MESSAGE* matrixMess, MESSAGE* A_Matrix, ENGINE* sl_cc, SL_CO* sl_co);

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

	CALL_SM(void);

};

//**********************************************************************************
//**********************************************************************************
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

////**********************************************************************************
////**********************************************************************************
//// State machine client (b side)
////**********************************************************************************
////**********************************************************************************
//class SL_SM_CL : public SL_SM {
//
//	public:
//		int placeholder;
//
//		int resend_invite;
//
//		PREDICATE_ACTION P0_1CL;
//		PREDICATE_ACTION P1_2CL;
//		PREDICATE_ACTION P1_3CL;
//		PREDICATE_ACTION P1_4CL;
//		PREDICATE_ACTION P3_3CL;
//		PREDICATE_ACTION P3_4CL;
//		PREDICATE_ACTION P2_3CL;
//		PREDICATE_ACTION P2_4CL;
//		PREDICATE_ACTION P4_5CL;
//		PREDICATE_ACTION P5_7CL;
//		PREDICATE_ACTION P7_8CL;
//
//
//
//		SL_SM_CL(ENGINE*, SL_CO*);
//
//};
////**********************************************************************************
////**********************************************************************************
//// State machine server (a side)
////**********************************************************************************
////**********************************************************************************
//class SL_SM_SV : public SL_SM {
//
//	public:
//		int placeholder;
//
//		PREDICATE_ACTION P0_1SV;
//		PREDICATE_ACTION P1_2SV;
//		PREDICATE_ACTION P2_2SV;
//		PREDICATE_ACTION P2_3SV;
//		PREDICATE_ACTION P1_3SV;
//		PREDICATE_ACTION P3_4SV;
//		PREDICATE_ACTION P4_6SV;
//
//
//		SL_SM_SV(ENGINE*, SL_CO*);
//
//};
