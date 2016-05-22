//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Core And Service Layer
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
#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <unordered_map>
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
#ifndef ACTION_H
#include "ACTION.h"
#endif

// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// Action
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
ACTION::ACTION(void){
	DEBUGACTION_3("ACTION::ACTION(void)","")
	return;
}
void ACTION::addSingleAction(SingleAction _sa){
	DEBUGACTION_3("ACTION::addSingleAction",_sa.getMessage())
	act_item.push(_sa);
}
stack<SingleAction> ACTION::getActionList(void){
	DEBUGACTION_3("ACTION::getActionList(void)","")
	return act_item;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// SingleAction
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
//SingleAction::SingleAction(MESSAGE* _message){
//	DEBASSERT("use message, 1")
//	message = _message;
//}
SingleAction::SingleAction(MESSAGE* _message){
	DEBUGACTION_3("SingleAction::SingleAction(MESSAGE* _message)",_message)
	message = _message;
}
// *****************************************************************************************
// *****************************************************************************************
MESSAGE* SingleAction::getMessage(void){
	DEBUGACTION_3("MESSAGE* SingleAction::getMessage","")
	return message;
}
