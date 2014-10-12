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

#define COMAP_H

//Call oset is running and can be used
#define NOT_DOA 0
//Call oset has been delete from memory
#define DOA_DELETED -1
// doa requested only message with gen != NWTPOINT can be processed by call_oset
#define DOA_REQUESTED 1
// doa confirmed no message can get in


//map to associate callId_Y to callId_X
typedef unordered_map<string, string> CALL_ID_Y2X;


//Call Object Map
class COMAP {

	private:

		int loktry[COMAPS];

		//Call id x
		unordered_map<string, CALL_OSET*> comap_mm[COMAPS];


		//call id y to call id x
		unordered_map<string, string> call_id_y2x[COMAPS];
                //Questo serve quando creo una nuova call
                //tramite un call_x emi serve per trovare 
                //la entry nella tabella call_id_y2x
                unordered_map<string, string> call_id_x2y[COMAPS];

		//Map of call_osets and doa
		//doa can't be stored into call_oset
		unordered_map<CALL_OSET*, int> call_oset_doa_state[COMAPS];

		//Time to live
		unordered_map<CALL_OSET*, lli> call_oset_ttl[COMAPS];
		unordered_map<CALL_OSET*, lli> call_oset_ttl_delete[COMAPS];

		void setDoa(CALL_OSET*, int,int);
		int getDoa(CALL_OSET*,int);

		void resetDoaRequestTimer(CALL_OSET* call_oset,int modulus);

	public:
		CALL_OSET* setCALL_OSET(string callId_X,int, ENGINE*, TRNSPRT*, MESSAGE*, string, int);

		CALL_OSET* getCALL_OSET_XMain(string callId_X,int);
		CALL_OSET* getCALL_OSET_YDerived(string callId_Y,int);
//		void setY2XCallId(string callId_Y, string callId_X,int);

	//Friend to call_oset
		int use_CALL_OSET_SL_CO_call(CALL_OSET*, MESSAGE*,int);
		void use_CALL_OSET_setCallId_Y(CALL_OSET*, string _cally,int);
		void setDoaRequested(CALL_OSET*,int);

		void purgeDOA(int comapset);

		int getDoaState(CALL_OSET*,int);


		COMAP(void);
		COMAP(const COMAP& x);
        ~COMAP(void);

		//pthread_mutex_t unique_ex[COMAPS];
		pthread_mutex_t unique_exx[COMAPS];

};
