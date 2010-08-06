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

//Call oset is running and can be used
#define NOT_DOA 0
//Call oset has been delete from memory
#define DOA_DELETED -1
// doa requested only message with gen != NWTPOINT can be processed by call_oset
#define DOA_REQUESTED 1
// doa confirmed no message can get in
#define DOA_CONFIRMED 2


//map to associate callId_Y to callId_X
typedef map<string, string> CALL_ID_Y2X;


//Call Object Map
class COMAP {

	private:
		//Call id x
		pthread_mutex_t comap_mutex;
		map<string, CALL_OSET*> comap_mm;

		//call id y to call id y
		pthread_mutex_t call_y2x_mutex;
		map<string, string> call_id_y2x;

		//Map of call_osets and doa
		//doa can't be stored into call_oset
		pthread_mutex_t doa_mutex;
		map<CALL_OSET*, int> call_oset_doa_state;
		void setDoa(CALL_OSET*, int);
		int getDoa(CALL_OSET*);

		//Number of messages currently running inside call_oset
		pthread_mutex_t co_msgcnt_mutex;
		map<CALL_OSET*, int> call_oset_msg_cnt;
		int getCALL_OSET_MsgCnt(CALL_OSET*);
		void incCALL_OSET_MsgCnt(CALL_OSET*);
		void decCALL_OSET_MsgCnt(CALL_OSET*);

	public:
		void setCALL_OSET(string callId_X, CALL_OSET*);
		CALL_OSET* getCALL_OSET_XMain(string callId_X);
		CALL_OSET* getCALL_OSET_YDerived(string callId_Y);
		void setY2XCallId(string callId_Y, string callId_X);

	//Friend to call_oset
		int use_CALL_OSET_SL_CO_call(CALL_OSET*, MESSAGE*);
		void use_CALL_OSET_setCallId_Y(CALL_OSET*, string _cally);
		void setDoaRequested(CALL_OSET*);

		void purgeDOA(void);

		COMAP(void);
		~COMAP(void);

};
