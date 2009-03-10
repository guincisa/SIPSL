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

#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
//**********************************************************************************
// * SL_CC.h
// * Call Control
//**********************************************************************************
//**********************************************************************************
// SL_CC
//**********************************************************************************
class SL_CC : public ENGINE {

    public:
        void parse(MESSAGE*);
        //void sendReply(MESSAGE);

    private:

    	//COMAP *callMap; // array
        //CO * co; // ??? non corretto
        //SUDP * sipStack;
};

