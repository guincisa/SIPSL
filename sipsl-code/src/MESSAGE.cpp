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

#include <string>
#include <stack>
#include <assert.h>
#include <map>
#include <vector>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#ifndef UTIL_H
#include "UTIL.h"
#endif

#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif

#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif

// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// BASEMESSAGE
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
BASEMESSAGE::BASEMESSAGE(string _incMessBuff, int _genEntity, SysTime _inc_ts, int _sock,
                    struct sockaddr_in _echoClntAddr){

	incMessBuff=_incMessBuff;
	genEntity = _genEntity;
	inc_ts = _inc_ts;
	sock = _sock;
	echoClntAddr = _echoClntAddr;

	arrayFilled = false;

	return;
}
BASEMESSAGE::BASEMESSAGE(string _incMessBuff, SysTime _inc_ts){
	//TODO only for test
	incMessBuff=_incMessBuff;

	inc_ts = _inc_ts;

	arrayFilled = false;

	return;
}

void BASEMESSAGE::fillLineArray(void){

    if (arrayFilled)
        return;


    Tuple s = brkin2(incMessBuff,"\n");
    string t = s.Rvalue;
    DEBOUT("s", s.Lvalue)
    DEBOUT("t", t)
    flex_line.push_back(s.Lvalue);
    while (t.compare("")!=0){
    	s = brkin2(t,"\n");
    	t = s.Rvalue;
        DEBOUT("s", s.Lvalue)
        DEBOUT("t", t)
        flex_line.push_back(s.Lvalue);
    }

    arrayFilled = true;

    return;

    /*
    int iii=1,jjj=0,kkk=0,ij=0;

    bool stilllines = true;

    while (stilllines) {
        //search: METHOD, CallId, To, From, Cseq, expires
        kkk = incMessBuff.find("\n", jjj );
        if ( kkk < 0) {
            stilllines = false;
        }
        else {
            linePosition.push_back(kkk + 1);
            jjj = kkk+1;
            iii ++;
        }
    }
    for (ij = 1; ij < iii;ij++) {
        flex_line.push_back(incMessBuff.substr(linePosition[ij-1],linePosition[ij] - linePosition[ij-1] -2));
        DEBOUT("Line","line " +  ij-1 + flex_line[ij-1]);
    }

    totLines = iii -1;

    arrayFilled = true;

    return;
    */
}
// *****************************************************************************************
// *****************************************************************************************
int BASEMESSAGE::getTotLines() {

    if (!arrayFilled)
    	fillLineArray();

    return(flex_line.size());
}
// *****************************************************************************************
// *****************************************************************************************
string &BASEMESSAGE::getLine(int pos){
    // no copia REFERRENCE

    if (!arrayFilled)
    	fillLineArray();

    int i = getTotLines();

    if (pos >= i)
        pos = i-1;

    return(flex_line[pos]);
}


/*
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// MESSAGE
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
MESSAGE::MESSAGE() {
}
MESSAGE::MESSAGE(string _s, int _sock, struct sockaddr_in _echoClntAddr ) {

    incomingMessage = _s;
    sock = _sock;
    echoClntAddr = _echoClntAddr;
}
void MESSAGE::createReplyTemplate(string pay) {
    //replyTemplate.I_I = I_I;
    //replyTemplate.P_P = pay;
    char m[256];
    //sprintf(m, "M:REPLY-%s\nI:%s\nR:%d\nP:%s", M_M.c_str(), I_I.c_str(), R_R, pay.c_str());
    sprintf(m, "gugli reply template");
    replyTemplate.message = m;
    return;
}
void MESSAGE::sendReply(string message) {
        char ecco[ECHOMAX];
        //replyTemplate.out_ts = gethrtime();
        GETTIME(replyTemplate.out_ts)
        sprintf(ecco, "%s\nTI:%lld\nTO:%lld", replyTemplate.message.c_str(), in_ts, replyTemplate.out_ts);
        sendto(sock, ecco, ECHOMAX, 0, (struct sockaddr *) &(echoClntAddr), sizeof(echoClntAddr));
        return;

}
// ack
void MESSAGE::sendAck(string message) {
        char ecco[ECHOMAX];
        //sprintf(ecco, "M:OK %s\nI:%s\nR:%d", message.c_str(), I_I.c_str(), R_R);
        sprintf(ecco, "gugli ack");
        sendto(sock, ecco, ECHOMAX, 0, (struct sockaddr *) &(echoClntAddr), sizeof(echoClntAddr));

}
void MESSAGE::sendNAck(int errnum, string message) {
        char ecco[ECHOMAX];
        //sprintf(ecco, "M:NOK! %s\nI:%s\nR:%d", message.c_str(), I_I.c_str(), R_R);
        sprintf(ecco, "gugli nack");
        sendto(sock, ecco, ECHOMAX, 0, (struct sockaddr *) &(echoClntAddr), sizeof(echoClntAddr));
}
//auto_ptr<REGISTER200OK> MESSAGE::create200OK(void) {
REGISTER200OK * MESSAGE::p_create200ok(void) {
    REGISTER200OK * p_notuse;
    p_notuse = new REGISTER200OK;
    return p_notuse;
}

*/


