//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
// Copyright (C) 2010 Guglielmo Incisa di Camerana
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

#define TRNSCT_SM_H

class SL_CO;
class ALO;
class SL_SM;
class TRNSCT_SM;
class ENGINE;

//**********************************************************************************
//**********************************************************************************
// V5 Transaction State machines
//**********************************************************************************
//**********************************************************************************
class PREDICATE_ACTION;
class SM {

	private:
		pthread_mutex_t mutex;
		multimap< const int, PREDICATE_ACTION*> move_sm;

		ENGINE* sl_cc;
	    SL_CO* sl_co;

	public:
	    int State; // initial 0, final -1
		void insert_move(int, PREDICATE_ACTION*);
		ACTION* event(MESSAGE*);

		ENGINE* getSL_CC(void);
		SL_CO* getSL_CO(void);
		SM(ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
class TRNSCT_SM  :  public SM{

	private:

		int requestType;
		//Matrix is the request which triggers the creation of the State Machine
		MESSAGE* Matrix;
		//A Matrix is the a-side request which triggers the above request
		//it differs from the above message only in case of client sm
		MESSAGE* A_Matrix;

		string id;

	public:

		string getId(void);
		void setId(string);

		MESSAGE* getMatrixMessage(void);

		MESSAGE* getA_Matrix(void);

		TRNSCT_SM(int requestType, MESSAGE* matrixMess, MESSAGE* A_Matrix, ENGINE* sl_cc, SL_CO* sl_co);

		~TRNSCT_SM();


};
//**********************************************************************************
class PREDICATE_ACTION {

	private:
	SM* machine;

	public:

	bool (*predicate)(SM*, MESSAGE*);
	ACTION* (*action)(SM*, MESSAGE*);

	PREDICATE_ACTION(SM*);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE INVITE SERVER
//**********************************************************************************
class TRNSCT_SM_INVITE_SV : public TRNSCT_SM {

	public:

		PREDICATE_ACTION PA_INV_0_1SV;
		PREDICATE_ACTION PA_INV_1_1SV;
		PREDICATE_ACTION PA_INV_1_2SV;
		PREDICATE_ACTION PA_INV_1_3SV;
		PREDICATE_ACTION PA_INV_3_4SV;
		PREDICATE_ACTION PA_INV_2_2SV;
		PREDICATE_ACTION PA_INV_3_3aSV;
		PREDICATE_ACTION PA_INV_3_3bSV;
		PREDICATE_ACTION PA_INV_3_3cSV;
		PREDICATE_ACTION PA_INV_3_3dSV;

		//This is the 200ok a to be resent if invite a arrives again
		//TODO
		MESSAGE* STOREMESS_1_3;
		//This is the try  a to be resent if invite a arrives again
		//TODO
		MESSAGE* STOREMESS_1_2;

		int resend_200ok;

		TRNSCT_SM_INVITE_SV(int requestType, MESSAGE* matrixMess, ENGINE* sl_cc, SL_CO* sl_co);
};
//**********************************************************************************
// TRANSACTION STATE MACHINE INVITE CLIENT
//**********************************************************************************
class TRNSCT_SM_INVITE_CL : public TRNSCT_SM {


	public:

		PREDICATE_ACTION PA_INV_0_1CL;
		PREDICATE_ACTION PA_INV_1_1CL; //resend invite
		PREDICATE_ACTION PA_INV_1_2CL; //100 try incoming
		PREDICATE_ACTION PA_INV_1_3CL; //100 try not arriving, 180 ring or Diag Estab instead
		PREDICATE_ACTION PA_INV_1_99CL; //resend invite max reached
		PREDICATE_ACTION PA_INV_1_4CL; //200OK
		PREDICATE_ACTION PA_INV_4_4aCL;
		PREDICATE_ACTION PA_INV_4_4bCL; //200OK B incoming again, need to resend ACK



		int resend_invite;

		TRNSCT_SM_INVITE_CL(int requestType, MESSAGE* matrixMess, MESSAGE* A_Matrix, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE ACK SERVER
//**********************************************************************************
class TRNSCT_SM_ACK_SV : public TRNSCT_SM {

	public:


		PREDICATE_ACTION PA_ACK_0_1SV;


		TRNSCT_SM_ACK_SV(int requestType, MESSAGE* matrixMess, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE ACK CLIENT
//**********************************************************************************
class TRNSCT_SM_ACK_CL : public TRNSCT_SM {

	private:

	public:

		PREDICATE_ACTION PA_ACK_0_1CL;
		PREDICATE_ACTION PA_ACK_1_1CL;

		TRNSCT_SM_ACK_CL(int requestType, MESSAGE* matrixMess, MESSAGE* A_Matrix, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE BYE SERVER
//**********************************************************************************
class TRNSCT_SM_BYE_SV : public TRNSCT_SM {

	public:

		PREDICATE_ACTION PA_BYE_0_1SV;
		PREDICATE_ACTION PA_BYE_1_2SV;


		TRNSCT_SM_BYE_SV(int requestType, MESSAGE* matrixMess, ENGINE* sl_cc, SL_CO* sl_co);

};
//**********************************************************************************
// TRANSACTION STATE MACHINE BYE CLIENT
//**********************************************************************************
class TRNSCT_SM_BYE_CL : public TRNSCT_SM {


	public:

		int resend_bye;

		PREDICATE_ACTION PA_BYE_0_1CL;
		PREDICATE_ACTION PA_BYE_1_1CL; //resend bye
		PREDICATE_ACTION PA_BYE_1_2CL; //200OK
		PREDICATE_ACTION PA_BYE_1_99CL;

		TRNSCT_SM_BYE_CL(int requestType, MESSAGE* matrixMess, MESSAGE* A_Matrix, ENGINE* sl_cc, SL_CO* sl_co);

};

//**********************************************************************************
//**********************************************************************************
// Call/Dialog state machine
//**********************************************************************************
//**********************************************************************************
class CALL_SM  : SM{

	private:

	vector<TRNSCT_SM*> trnsct_sm_array;

	public:

	CALL_SM(void);

};

