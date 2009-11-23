//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Core And Service Layer
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
	return;
}
void ACTION::addSingleAction(SingleAction _sa){
	act_item.push(_sa);
}
stack<SingleAction> ACTION::getActionList(void){
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
SingleAction::SingleAction(MESSAGE* _message):
	internalOp(0, "", ""){
	type = 1;
	message = _message;
}
SingleAction::SingleAction(INTERNALOP _internalOp):
	internalOp(_internalOp){
	type = 2;
}
// *****************************************************************************************
// *****************************************************************************************
MESSAGE* SingleAction::getMessage(void){
	return message;
}
INTERNALOP SingleAction::getInternalOp(void){
	return internalOp;
}
int SingleAction::getType(void){
	return type;
}

