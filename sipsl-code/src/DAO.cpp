//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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
//**********************************************************************************
// DAO laeyr
//**********************************************************************************

#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <map>

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stack>

#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef SPIN_H
#include "SPIN.h"
#endif
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef SIPENGINE_H
#include "SIPENGINE.h"
#endif
#ifndef SL_CC_H
#include "SL_CC.h"
#endif
#ifndef ACTION_H
#include "ACTION.h"
#endif
#ifndef DOA_H
#include "DOA.h"
#endif
#ifndef SUDP_H
#include "SUDP.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif
#ifndef COMAP_H
#include "COMAP.h"
#endif
#ifndef ALO_H
#include "ALO.h"
#endif
#ifndef VALO_H
#include "VALO.h"
#endif
#ifndef SIP_PROPERTIES_H
#include "SIP_PROPERTIES.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
#endif
#ifndef SIPUTIL_H
#include "SIPUTIL.h"
#endif
#ifndef TRNSPRT_H
#include "TRNSPRT.h"
#endif
#ifndef DAO_H
#include "DAO.h"
#endif


//**********************************************************************************
//**********************************************************************************
DAO::DAO(int _i, int _m, string _s):ENGINE(_i,_m,_s){

    pthread_mutex_init(&mutex, NULL);

}
string DAO::getData(string _data){


    map <string,string>::iterator it;
	GETLOCK(&mutex,"DAO mutex",19)
	it = routingTable.find(_data);
	RELLOCK(&mutex,"DAO mutex")

	return it->second;


}
string getData(string);

//**********************************************************************************
//**********************************************************************************
void DAO::parse(void* __mess, int _mmod) {
	DEBINF("void DAO::parse(void* __mess, int _mmod)",__mess<<"]["<<_mmod)

    RELLOCK(&(sb[_mmod]->condvarmutex),"sb"<<_mmod<<".condvarmutex");

    PROFILE("DAO::parse() start")
    TIMEDEF
    SETNOW

	MESSAGE* _mess = (MESSAGE*)__mess;
    COMMAND com;
    _mess->buildCommand(com.commandTable);

    vector< pair<int, pair<string,string> > >::iterator m_l;
    m_l = com.commandTable.begin();
    while (m_l  != com.commandTable.end()){
        if (m_l->first == 1){
            GETLOCK(&mutex,"DAO mutex",20)
            DEBOUT("void DAO::parse",m_l->first << "]["<<((pair <string,string>)m_l->second).first<<"]["<<((pair <string,string>)m_l->second).second)
            routingTable.insert(make_pair( ((pair <string,string>)m_l->second).first,((pair <string,string>)m_l->second).second));
        	RELLOCK(&mutex,"DAO mutex")
            //tutto su umft1:
            //PD-SIPSL
            //i%service@10.21.99.79:5062%service@10.21.99.79:5062
            //send
            //sipp on umft2:
            //PD-SIPSL
            //i%service@10.21.99.79:5062%service@10.21.99.81:5062
            //send

        }
        m_l ++;
    }


}

