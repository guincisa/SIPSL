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

class SL_CO;
class ALO;
class SL_SM;
class TRNSCT_SM;
class ENGINE;
class TRNSPRT;

//Umbrella class which hosts states machines and call object
class CALL_OSET {

	friend class COMAP;
	friend class SL_CO;
	friend class ALO;
	friend class VALO;
	friend class TRNSCT_SM;
	friend class TRNSPRT;

	private:

		TRNSPRT* transport;
		SL_CO* sl_co;
		ALO* alo;
		ENGINE* engine;
		string callId_X;
		string callId_Y;
		void setCallId_Y(string _cally);
		string getCallId_Y(void);
		void setCallId_X(string callId_X);
		string getCallId_X(void);
		void setSL_CO(SL_CO*);
		SL_CO* getSL_CO(void);
		ENGINE* getENGINE(void);
		ALO* getALO(void);
		void setALO(ALO*);
		TRNSPRT* getTRNSPRT(void);

		// TRANSACTION MANAGEMENT
		// KEY is like this
		// <REQUESTTYPE>#<SIDE>#<BRANCH>
		// example 1#A#1
		map<string, TRNSCT_SM*> trnsctSmMap;
		// Get new transaction state machine
		TRNSCT_SM* newTrnsct(string method, string side);
		// add transaction state machine
		void addTrnsctSm(string method, int sode, string branch, TRNSCT_SM* trnsctSm);
		// get transaction state machine
		TRNSCT_SM* getTrnsctSm(string method, int sode, string branch);

		// Sequence map
		// used to store used sequences and return new unused sequences
		map<string, int> sequenceMap;
		// current sequence, shared among all request
		int currentSequence;
		int getNextSequence(string method);
		int getCurrentSequence(string method);
		void insertSequence(string method, int cntr);

		map<MESSAGE*,int> lockedMessages;

	public:
		CALL_OSET(ENGINE*, string call_x);
		~CALL_OSET(void);
		void insertLockedMessage(MESSAGE*);
		MESSAGE* getNextLockedMessage(void);
		void removeLockedMessage(MESSAGE*);



};
//**********************************************************************************
//**********************************************************************************
// Call object
//**********************************************************************************
//**********************************************************************************
class SL_CO {

	private:
	pthread_mutex_t mutex;

	void actionCall_SV(ACTION*);
	void actionCall_CL(ACTION*);


	public:
		int placeholder;

		int OverallState_SV;
		int OverallState_CL;

		SL_CO(CALL_OSET*);

		CALL_OSET* call_oset;

		void call(MESSAGE*);

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
