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
#ifndef TRNSPRT_H
#define TRNSPRT_H

class SIPENGINE;


//Is a global object but not an engine, it runs
//inside SIPENGINE when up
//inside SL_CC when down

//Must be an engine for the downcall
//because of the network functions called
class TRNSPRT : public ENGINE {

    private:
        int dummy;
        SIPENGINE* sipengine;

    public:
        
        TRNSPRT(int,int,string);

        void upCall(MESSAGE*);

        void parse(void*,int); //downcall

        void setSipEngine(SIPENGINE* sipengine);

};
#endif
