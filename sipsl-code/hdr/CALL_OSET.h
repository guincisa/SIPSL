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
class MESSAGE;
class ACTION;

//Umbrella class which hosts states machines and call object
//This must be controlled by a mutex
class CALL_OSET {

	friend class SL_CO;
	friend class ALO;
	friend class VALO;
	friend class TRNSCT_SM;

	private:

		TRNSPRT* transport;
		SL_CO* sl_co;
		ALO* alo;
		ENGINE* engine;
		string callId_X;
		string callId_Y;
		void setCallId_Y(string _cally);
		void setCallId_X(string callId_X);
		void setSL_CO(SL_CO*);
		SL_CO* getSL_CO(void);
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

		void dumpTrnsctSm(void);

		//Trick to access the ack client state machine without branch
		//it is needed when the 200OK comes again from b and should trigger the ack b again
		TRNSCT_SM* lastTRNSCT_SM_ACK_CL;

		// Sequence map
		// used to store used sequences and return new unused sequences
		map<string, int> sequenceMap;
		// current sequence, shared among all request
		int currentSequence;
		int getNextSequence(string method);
		int getCurrentSequence(string method);
		void insertSequence(string method, int cntr);

		map<MESSAGE*,int> lockedMessages;

		pthread_mutex_t mutex;

		MESSAGE* getNextLockedMessage(void);

	public:
		CALL_OSET(ENGINE*, TRNSPRT*, string call_x);
		~CALL_OSET(void);

		string getCallId_Y(void);
                string getCallId_X(void);


		int getOverallState_CL(void);
		int getOverallState_SV(void);

		//Wrapper for SL_CO::call
		void call(MESSAGE*);

		void insertLockedMessage(MESSAGE*);
		void removeLockedMessage(MESSAGE*);
		bool isLockedMessage(MESSAGE*);

		ENGINE* getENGINE(void);


};
//**********************************************************************************
//**********************************************************************************
// Call object
//**********************************************************************************
//**********************************************************************************
class SL_CO {

	private:

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
