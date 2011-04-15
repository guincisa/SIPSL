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

/***********************************************************
  SIPENGINE.h
  parses the SIP protocol
  messages must inherit from MESSAGE
  SIPENGINE class must inherit from ENGINE
************************************************************/
#define SIPENGINE_H
class TRNSPRT;
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//TODO singleton
class SIPENGINE : public ENGINE {

    public:

        void parse(void *, int);

        SIPENGINE(int);

        // link on this the SL_CC engine
        void setSL_CC(ENGINE *);

        ENGINE * getSL_CC(void);

        void linkTransport(TRNSPRT*);


    private:
        //Second stage link to call control
        ENGINE *sl_cc;

        TRNSPRT* transport;
};
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
