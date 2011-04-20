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
#define SL_CC_H

class ENGINE;
class COMAP;
//**********************************************************************************
// * SL_CC
// * Call Control second stage Engine
//**********************************************************************************
class SL_CC : public ENGINE {

    public:
        void parse(void*, int);
        SL_CC(int,int);
        COMAP* getCOMAP(void);
        void linkTransport(TRNSPRT*);
        void linkSipEngine(ENGINE*);


    private:

        int internalparse(void*);


        TRNSPRT* transport;

        ENGINE *sipengine;


    	COMAP* comap;

};

