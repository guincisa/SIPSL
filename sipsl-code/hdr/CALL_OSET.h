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

#ifndef CALL_OSET_H
#define CALL_OSET_H

class SL_CO_P;
class SL_MO;
class SL_CO;

class ALO;
class SL_SM;
class TRNSCT_SM;
class ENGINE;
class TRNSPRT;
class MESSAGE;
class ACTION;

#define TYPE_SL_CO 1
#define TYPE_SL_MO 2

//Umbrella class which hosts states machines and call object
//This must be controlled by a mutex
class CALL_OSET {

	friend class SL_CO_P;
	friend class SL_MO;
	friend class SL_CO;

	friend class ALO;
	friend class VALO;
	friend class TRNSCT_SM;
	friend class COMAP;

	private:

		TRNSPRT* transport;
		SL_CO_P* sl_co;
		ALO* alo;
		ENGINE* engine;
		string callId_X;
		string callId_Y;
		void setCallId_Y(string _cally);
		void setCallId_X(string callId_X);
		void setSL_CO(SL_CO_P*);
		SL_CO_P* getSL_CO(void);
		ALO* getALO(void);
		void setALO(ALO*);

		int modulus;


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
	public:
		TRNSCT_SM* getTrnsctSm(string method, int sode, string branch);
		void dumpTrnsctSm(void);
		TRNSCT_SM* lastTRNSCT_SM_ACK_CL;

	private:


		//Trick to access the ack client state machine without branch
		//it is needed when the 200OK comes again from b and should trigger the ack b again

		// Sequence map
		// used to store used sequences and return new unused sequences
		map<string, int> sequenceMap;
		// current sequence, shared among all request
		int currentSequence;
		int getNextSequence(string method);
		int getCurrentSequence(string method);
		void insertSequence(string method, int cntr);

		map<MESSAGE*,int> lockedMessages;


		MESSAGE* getNextLockedMessage(void);

	public:
		CALL_OSET(ENGINE*, TRNSPRT*, string call_x, int modulus, int typeofco);
		~CALL_OSET(void);

		string getCallId_Y(void);
        string getCallId_X(void);

		int getOverallState_CL(void);
		int getOverallState_SV(void);

		//Wrapper for SL_CO_P::call
		void call(MESSAGE*);

		void insertLockedMessage(MESSAGE*);
		void removeLockedMessage(MESSAGE*);
		bool isLockedMessage(MESSAGE*);

		ENGINE* getENGINE(void);

#ifdef SV_CL_MUTEX
		pthread_mutex_t mutex_cl;
		pthread_mutex_t mutex_sv;
#else
		pthread_mutex_t mutex;
#endif
		TRNSPRT* getTRNSPRT(void);


};
//**********************************************************************************
//**********************************************************************************
// Call object
//**********************************************************************************
//**********************************************************************************
class SL_CO_P {

	private:
	virtual int actionCall_SV(ACTION*, int& mod) = 0;
	virtual int actionCall_CL(ACTION*, int& mod) = 0;

	int typeofco;

	public:
		int getTypeOfCo(void);

		int placeholder;

		int OverallState_SV;
		int OverallState_CL;

		SL_CO_P(CALL_OSET*,int);

		CALL_OSET* call_oset;


		//operation
		// 0 nothing
		// 1 do
		virtual int call(MESSAGE*,int& modulus) = 0;

};
//**********************************************************************************
//**********************************************************************************
// Message object
//**********************************************************************************
//**********************************************************************************
class SL_CO : public SL_CO_P{

	private:

	int actionCall_SV(ACTION*, int& mod);
	int actionCall_CL(ACTION*, int& mod);


	public:
		SL_CO(CALL_OSET*);

		//operation
		// 0 nothing
		// 1 do
		int call(MESSAGE*,int& modulus);

};
class SL_MO : public SL_CO_P{

	private:

	int actionCall_SV(ACTION*, int& mod);
	int actionCall_CL(ACTION*, int& mod);


	public:
		SL_MO(CALL_OSET*);
		//operation
		// 0 nothing
		// 1 do
		int call(MESSAGE*,int& modulus);

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
#endif
