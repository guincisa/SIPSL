//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer 
// Copyright (C) 2008 Guglielmo Incisa di Camerana
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

//**********************************************************************************
// * SL_CC.h
// * Call Control
//**********************************************************************************
//**********************************************************************************
// SL_CC 
//**********************************************************************************
class SL_CC : public ENGINE {

    public:
        void parse(MESSAGE);
        void sendReply(MESSAGE);
        void associateSIPSTACK(SUDP *);

        // OCT 13 2008 not needed
        //SL_CC();

    private:
        // not directly used
        // OCT 13 2008 not needed
        //SIPENGINE *_SIPENG;

        COMAP *callMap; // array
        //CO * co; // ??? non corretto
        SUDP * sipStack;
};

