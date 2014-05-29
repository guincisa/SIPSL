//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
// Copyright (C) 2014 Guglielmo Incisa di Camerana
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
#ifndef SEAMFAIL_H
#define SEAMFAIL_H

//Seamless failover system
//this sends the instantiation messages to the mate
typedef struct _hearBeatTuple {
    timespec sleep_time;
    SUDP* hbSUDP;
    string peerAddress;
    int peerPort;
} HearBeatTuple;

class SEAMFAILENG : public ENGINE {

    private:
        int dummy;
        ThreadWrapper* heartBeat;
        timespec sleep_time;
        SUDP* hbSUDP;
        string peerAddress;
        int peerPort;


    public:

        SEAMFAILENG(int,int,std::string);
        void setSUDP(SUDP*, string, int);

        void parse(void*,int);

};
#endif
