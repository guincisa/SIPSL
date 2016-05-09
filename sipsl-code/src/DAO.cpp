//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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
//**********************************************************************************
// DAO laeyr
//**********************************************************************************

#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string>
#include <string.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>

#include "UTIL.h"

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
DAO::DAO(int _i, int _m, string _s,SL_CC* _sl_cc, SIPENGINE* _sipengine, TRNSPRT* _trnsprt):ENGINE(_i,_m,_s){

    pthread_mutex_init(&mutex, NULL);

    sl_cc = _sl_cc;
    sipengine = _sipengine;
    trnsprt = _trnsprt;


}
string DAO::getData(int _table, string _data){

    map <string,string>::iterator it;
	GETLOCK(&mutex,"DAO mutex",19)
	it = datatable[_table].find(_data);
	RELLOCK(&mutex,"DAO mutex")
	if (it == datatable[_table].end()){
		return "";
	}

	return it->second;

}
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

    vector< pair<int,pair<int, pair<string,string> > > >::iterator m_l;
    m_l = com.commandTable.begin();
    while (m_l  != com.commandTable.end()){
    	int table = m_l->first;
    	if(table >= TABLES){
    		DEBASSERT("table index incorrect")
    	}
        pair<int, pair<string,string> >  m_l_2;
        m_l_2 = m_l->second;
        // m_l_2.first is the oeration
        // i = i insert
        // d = 2 delete
        // q = 3 query/dump
        if (m_l_2.first == 1){ //INSERT
            GETLOCK(&mutex,"DAO mutex",20)
            DEBOUT("void DAO::parse",m_l_2.first << "]["<<((pair <string,string>)m_l_2.second).first<<"]["<<((pair <string,string>)m_l_2.second).second)
            datatable[table].insert(make_pair( ((pair <string,string>)m_l_2.second).first,((pair <string,string>)m_l_2.second).second));
        	RELLOCK(&mutex,"DAO mutex")

            //PD-SIPSL
            //0%i%sipsl:5061%service@sipsl:5061
            //send

        }
        if (m_l_2.first == 3){ //DUMP
            GETLOCK(&mutex,"DAO mutex",20)
            DEBOUT("void DAO::parse dumping registration table size", datatable[table].size());
            map <string,string>::iterator reg_l;
            reg_l = datatable[table].begin();
            while (reg_l  != datatable[table].end()){
            	DEBOUT("void DAO::parse record ", reg_l->first << "][" << reg_l->second);
                reg_l++;
            }
            DEBOUT("void DAO::parse dumping registration end","");

			RELLOCK(&mutex,"DAO mutex")
        }
        if (m_l_2.first == 20){
            //PD-SIPSL
            //t%SL_CC%10
            //send
        	//change tuning params
        	//riduce o aumenta le code e non i loro trheads
        	DEBOUT("void DAO::parse",m_l_2.first << "]["<<((pair <string,string>)m_l_2.second).first<<"]["<<((pair <string,string>)m_l_2.second).second)
        	if (((string)((pair <string,string>)m_l_2.second).first).compare("SL_CC") == 0){
            	int k = atoi(((string)((pair <string,string>)m_l_2.second).second).c_str());
            	sl_cc->changeEngineMaps(k);
        	}
        	if (((string)((pair <string,string>)m_l_2.second).first).compare("SIPENGINE") == 0){
            	int k = atoi(((string)((pair <string,string>)m_l_2.second).second).c_str());
            	sipengine->changeEngineMaps(k);
        	}
        	if (((string)((pair <string,string>)m_l_2.second).first).compare("TRNSPRT") == 0){
            	int k = atoi(((string)((pair <string,string>)m_l_2.second).second).c_str());
            	trnsprt->changeEngineMaps(k);
        	}

        }

        m_l ++;
    }
    PURGEMESSAGE(_mess)


}
void DAO::putData(int _table,pair<string,string> _tuple){
	//Register data
	GETLOCK(&mutex,"DAO mutex",20)
	DEBOUT("void DAO::putData",_table <<"]["<<_tuple.first << "]["<<_tuple.second)
	datatable[_table].erase(_tuple.first);
	datatable[_table].insert(_tuple);
	RELLOCK(&mutex,"DAO mutex")
}

