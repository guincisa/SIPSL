//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer 
// Copyright (C) 2007 Guglielmo Incisa di Camerana
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

/***********************************************************
  SIPENGINE.h
  parses the SIP protocol
  messages must inherit from MESSAGE
  SIPENGINE class must inherit from ENGINEADAPTER
************************************************************/

//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//TODO singleton
class SIPENGINE : public ENGINE {
    public:

        // link on this the SL_CC engine
        void associateSL_CC(ENGINE *);
        void parse(MESSAGE);

        void associateSIPSTACK(SUDP *);

        // start AC listener
        void start(void);

        ENGINE * getSL_CC(void);

    private:
        ENGINE *_SL_CC;
        SUDP * sipStack;
};
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
