//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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

//V6
#ifndef ALO_H
#define ALO_H
class ALO {
	public:

		//CALL_OSET needed for back reference to umbrella object
		ALO(ENGINE*, CALL_OSET*);
		int dummy;

		/* parse will get the message and depending on the request will invoke the
		 * related call back
		 * for example is INVITE, it will invoke onINVITE()
		 * all call backs can be overridden if the application needs to implement something
		 */
        void call(MESSAGE*);

        virtual void onInvite(MESSAGE*);
        virtual void onAck(MESSAGE*);
        virtual void onBye(MESSAGE*);
        virtual void on200Ok(MESSAGE*);
        virtual void onAckNoTrnsct(MESSAGE*);

        void noCallBack(MESSAGE*);

        void linkSUDP(SUDP*);
        SUDP* getSUDP(void);
	protected:
        //TODO must be private
        ENGINE* sl_cc;
        CALL_OSET* call_oset;
        SUDP* sudp;


};
#endif
