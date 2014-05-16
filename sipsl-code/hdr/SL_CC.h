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
#ifndef SL_CC_H
#define SL_CC_H

class ENGINE;
class COMAP;
class DAO;
class TRNSPRT;
//**********************************************************************************
// * SL_CC
// * Call Control second stage Engine
//**********************************************************************************
class SL_CC : public ENGINE {

    public:
        void parse(void*, int);
        SL_CC(int,int,string);
        COMAP* getCOMAP(void);
        void linkTransport(TRNSPRT*);
        void linkSipEngine(ENGINE*);

        void setDAO(DAO *);
        DAO* getDAO(void);


    private:

        int internalparse(void*);


        TRNSPRT* transport;

        ENGINE *sipengine;

    	COMAP* comap;

        DAO *dao;

};
#endif
