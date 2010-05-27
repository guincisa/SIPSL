//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Core And Service Layer
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
//
//**********************************************************************************
// SL_CC Core and Service Layer Call Control
// Implements the basic call state machine
//**********************************************************************************
//#include <pthread.h>
//#include <unistd.h>
//#include <iostream>
//#include <stdio.h>
//#include <string>
//#include <sys/socket.h> /* for socket() and bind() */
//#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
//#include <stdlib.h>     /* for atoi() and exit() */

#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <map>

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stack>


#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef SIPENGINE_H
#include "SIPENGINE.h"
#endif
#ifndef SL_CC_H
#include "SL_CC.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef COMAP_H
#include "COMAP.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif
#ifndef VALO_H
#include "VALO.h"
#endif


//**********************************************************************************
//**********************************************************************************
SL_CC::SL_CC(void) {

	comap = new COMAP();
}
COMAP* SL_CC::getCOMAP(void){
	return comap;
}

//**********************************************************************************
//**********************************************************************************
void SL_CC::parse(MESSAGE* _mess) {

	DEBOUT("SL_CC::parse", _mess->getIncBuffer())

	pthread_mutex_unlock(&(sb.condvarmutex));


	if (_mess->getGenEntity() == SODE_NTWPOINT){

		DEBOUT("SL_CC::parse incoming message from network ", _mess->getGenEntity())

		CALL_OSET* call_oset = 0x0;

		string callidx = _mess->getDialogExtendedCID();

		_mess->getDialogExtendedCID();

		DEBOUT("SL_CC::parse CALLOSET extended ID",callidx)

		call_oset = comap->getCALL_OSET_XMain(callidx);

		//First try to get the Call object using x side parameters
		if (call_oset != 0x0) {
			DEBOUT("SL_CC::parse", "A SIDE call_oset exists")
			//Refine the source and destination
			_mess->setEndPoints(SODE_APOINT, SODE_SMSVPOINT);

			call_oset->getSL_CO()->call(_mess);
		}
		// Then try to get call object using y side params
		else {
			call_oset = comap->getCALL_OSET_YDerived(callidx);
			if (call_oset != 0x0){
				DEBOUT("SL_CC::parse", "B SIDE call_oset exists")
				//Refine the source and destination
				_mess->setEndPoints(SODE_BPOINT, SODE_SMCLPOINT);

				call_oset->getSL_CO()->call(_mess);
			}
		}
		// Does not exists on any side
		if (call_oset == 0x0) {
			//new call Server (originating) side
			DEBOUT("SL_CC::parse new call CALL_OSET creation X side, message", _mess)

			//If new then it is always SODE_APOINT
			_mess->setEndPoints(SODE_APOINT, SODE_TRNSCT_SV);

			//////////////////////////////
			//Start - Initialization block
			call_oset = new CALL_OSET(this);
			SL_CO* sl_co = new SL_CO(call_oset);

			VALO* alo = new VALO(this, call_oset);
			alo->linkSUDP(getSUDP());
			call_oset->setALO(alo);
			call_oset->setSL_CO(sl_co);

			call_oset->setSL_X(callidx);


			comap->setCALL_OSET(callidx, call_oset);
			//End
			//////////////////////////////

			DEBOUT("SL_CC::parse CALL_OSET created by x side", callidx << "] [" <<call_oset)

			sl_co->call(_mess);
		}
	}
	// Message comes from ALO
	//v5 else if (_mess->getGenEntity() == SODE_ALOPOINT || _mess->getGenEntity() == SODE_SMCLPOINT){
	else if (_mess->getGenEntity() == SODE_ALOPOINT || _mess->getGenEntity() == SODE_TRNSCT_CL){

		DEBOUT("SL_CC::parse entity from SODE_ALOPOINT (3) or SODE_TRNSCT_CL (4)", _mess->getGenEntity() )

		//get generating idx to get the call object
		string callidx = _mess->getSourceMessage()->getDialogExtendedCID();

		DEBOUT("SL_CC::parse Message from ALO/TRNSCT_CL generating call object", callidx)

		CALL_OSET* call_oset = 0x0;

		call_oset = comap->getCALL_OSET_XMain(callidx);

		if (call_oset == 0x0) {
			DEBOUT("SL_CC::parse","Orphan Invite")
			//TODO kill the invite
		}
		else {
				call_oset->getSL_CO()->call(_mess);
		}
	}
    return;
}
