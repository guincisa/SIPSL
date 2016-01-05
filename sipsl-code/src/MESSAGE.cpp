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

#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif

#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif

#ifndef SUDP_H
#include "SUDP.h"
#endif

// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// FMESSAGE
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
MESSAGE::MESSAGE(const char* _incMessBuff,
				int _genEntity,
				SysTime _inc_ts,
				int _sock,
				struct sockaddr_in _echoClntAddr){

	DEBINFMESSAGE_MIN("MESSAGE::MESSAGE(MESSAGE* _sourceMessage,int _genEntity,SysTime _creaTime)",this)

	sourceMessage 			= MainMessage;
	sourceHeadCallId 		= "";
	sourceModulus			= -1;

    invalid					= 0;

	lock					= false;

	key						= "";

	message_char			= 0x0;

	NEWPTR2(original_message, char[strlen(_incMessBuff)+1],"original_message "<<strlen(_incMessBuff)+1)
	strcpy(original_message, _incMessBuff);

	hasvialines				= false;
	hasSdp					= false;
	messageStatus			= 0;

	fillCounter				= 0;
	compileCounter			= 0;


	//0 new
	//1 filled
	//2 changed
	//
	// -> 0
	// message created
	//
	// 0 -> 1
	// getXXX invoked
	// fillIn is called
	//
	// 1 -> 2
	// setX invoked
	// the message is not synced
	//
	// 2 -> 2
	// setX
	//
	// 2 -> 0
	// compile message
	//
	// 0 -> 2 (0->1->2)
	// setX


    sock					= _sock;
    echoClntAddr			= _echoClntAddr;
    inc_ts					= _inc_ts;
	requestDirection		= 0;
	genEntity				= _genEntity;
	destEntity				= 0;
	natAddress				= "";
	natPort					= "";


	fireTime				= 0;
	orderOfOperation		= "";
	type_trnsct				= TYPE_TRNSCT;
	typeOfInternal			= TYPE_MESS;
	typeOfOperation			= TYPE_OP_NOOP;

    modulus					=  -1;


	branch					= "";
	parsedBranch			= false;
	viaUriHost				= "";
	viaUriParsed   			= false;
	viaUriPort				= 0;


	callId					= "";
	parsedCallId			= false;

	fromUser				= "" ;
	fromTag					= "";
	parsedFromTag			= false;
	toTag					= "";
	parsedToTag				= false;


	headTo					= "";
	headToName				= "";
	headToUri				= "";
	headToParms				= "";
	parsedTo				= false;
	parsedToName			= false;
	parsedToUri				= false;
	parsedToParms			= false;

	parsedContact			= false;
	contactUri				= "";
	contactName				= "";
	contactAddress			= "";
	contactPort				= "";


	route_address			= "";
	route_port				= "";
	has_route				= false;

	reqRep					= 0;
	headSipRequest			= "";
	headSipReply			= "";
	replyCode				= 0;
	requestCode				= 0;


	cSeqMethod				= "";
	cSeq					= 0;
	parsedCseq				= false;
	requestUriProtocol      = make_pair("","");

}

MESSAGE::MESSAGE(MESSAGE* _sourceMessage,
				int _genEntity,
				SysTime _creaTime){

	DEBINFMESSAGE_MIN("MESSAGE::MESSAGE(MESSAGE* _sourceMessage,int _genEntity,SysTime _creaTime)",this)

	sourceMessage 			= _sourceMessage;
	sourceHeadCallId 		= _sourceMessage->getHeadCallId();
	sourceModulus			= _sourceMessage->getModulus();

    invalid					= 0;

	lock					= false;

	key						= "";

	message_char			= 0x0;

	NEWPTR2(original_message, char[_sourceMessage->getDimString()+1],"original_message "<<_sourceMessage->getDimString()+1)
	strcpy(original_message, _sourceMessage->getOriginalString());


	hasvialines				= false;
	hasSdp					= false;
	messageStatus			= 0;

	fillCounter				= 0;
	compileCounter			= 0;

	sock 					= _sourceMessage->getSock();
	echoClntAddr 			= _sourceMessage->getEchoClntAddr();
    inc_ts					= _creaTime;
    requestDirection 		= _sourceMessage->getRequestDirection();
    genEntity				= _genEntity;
	destEntity				= 0;
	natAddress				= "";
	natPort					= "";


	fireTime				= 0;
	orderOfOperation		= "";
	type_trnsct				= TYPE_TRNSCT;
	typeOfInternal			= TYPE_MESS;
	typeOfOperation			= TYPE_OP_NOOP;

    modulus					=  -1;


	branch					= "";
	parsedBranch			= false;
	viaUriHost				= "";
	viaUriParsed   			= false;
	viaUriPort				= 0;


	callId					= "";
	parsedCallId			= false;

	fromUser				= "" ;
	fromTag					= "";
	parsedFromTag			= false;
	toTag					= "";
	parsedToTag				= false;


	headTo					= "";
	headToName				= "";
	headToUri				= "";
	headToParms				= "";
	parsedTo				= false;
	parsedToName			= false;
	parsedToUri				= false;
	parsedToParms			= false;

	parsedContact			= false;
	contactUri				= "";
	contactName				= "";
	contactAddress			= "";
	contactPort				= "";



	route_address			= "";
	route_port				= "";
	has_route				= false;

	reqRep					= 0;
	headSipRequest			= "";
	headSipReply			= "";
	replyCode				= 0;
	requestCode				= 0;


	cSeqMethod				= "";
	cSeq					= 0;
	parsedCseq				= false;
	requestUriProtocol      = make_pair("","");

}
//TODO put in constructor
void MESSAGE::_clearStatus(void){

	branch					= "";
	parsedBranch			= false;
	viaUriHost				= "";
	viaUriParsed   			= false;
	viaUriPort				= 0;


	callId					= "";
	parsedCallId			= false;


	fromUser				= "" ;
	fromTag					= "";
	parsedFromTag			= false;
	toTag					= "";
	parsedToTag				= false;


	headTo					= "";
	headToName				= "";
	headToUri				= "";
	headToParms				= "";
	parsedTo				= false;
	parsedToName			= false;
	parsedToUri				= false;
	parsedToParms			= false;


	reqRep					= 0;
	headSipRequest			= "";
	headSipReply			= "";
	replyCode				= 0;
	requestCode				= 0;


	cSeqMethod				= "";
	cSeq					= 0;
	parsedCseq				= false;
	requestUriProtocol      = make_pair("","");


	sdp_line.clear();
	via_line.clear();
	message_line.clear();

}


MESSAGE::~MESSAGE(){
	DEBINFMESSAGE_MIN("MESSAGE::~MESSAGE()",this)

	DELPTRARR(original_message,"original_message")

	if(messageStatus != 0){
		DELPTRARR(message_char,"message_char")

		for(unsigned int i = 0; i < message_line.size(); i ++){
			if(message_line[i].second){
				DELPTRARR(message_line[i].first,"message_line")
			}
		}
		for(unsigned int i = 0; i < sdp_line.size(); i ++){
			if(sdp_line[i].second){
				DELPTRARR(sdp_line[i].first,"sdp_line")
			}
		}
		for(unsigned int i = 0; i < via_line.size(); i ++){
			if(via_line[i].second){
				DELPTRARR(via_line[i].first,"via_line")
			}
		}
	}

	DEBWARNING("MESSAGE::~MESSAGE() incomplete code?",this)

}
MESSAGE* MESSAGE::getSourceMessage(void){
	DEBINFMESSAGE("MESSAGE* MESSAGE::getSourceMessage(void)",this<<"][")
	if (invalid == 1)
		DEBASSERT("MESSAGE::setKey invalid")

	DEBINFMESSAGE("MESSAGE* MESSAGE::getSourceMessage(void) sourceMessage",sourceMessage)
	return sourceMessage;
}
void MESSAGE::setSourceMessage(MESSAGE* _source){
	DEBINFMESSAGE("void MESSAGE::setSourceMessage(MESSAGE* _source)",this<<"]["<<_source)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setKey invalid")

    sourceHeadCallId = _source->getHeadCallId();
    sourceModulus = _source->getModulus();
    sourceMessage = _source;
	DEBINFMESSAGE("void MESSAGE::setSourceMessage(MESSAGE* _source)",this<<"]["<<sourceHeadCallId<<"]["<<sourceModulus<<"]["<<sourceMessage)

}
string MESSAGE::getSourceMessageCallId(void){
	DEBINFMESSAGE("string MESSAGE::getSourceMessageCallId(void)",this<<"][")
	if (invalid == 1)
		DEBASSERT("MESSAGE::setKey invalid")

	DEBINFMESSAGE("string MESSAGE::getSourceMessageCallId(void)",this<<"]["<<sourceHeadCallId)
	return sourceHeadCallId;
}
void MESSAGE::setSourceHeadCallId(string _scid){
	DEBINFMESSAGE("void MESSAGE::setSourceHeadCallId(string _scid)",this<<"]["<<_scid)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setKey invalid")

	sourceHeadCallId = _scid;
}
void MESSAGE::setSourceModulus(int _mod){
	DEBINFMESSAGE("void MESSAGE::setSourceModulus(int _mod)",this<<"]["<<_mod)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setKey invalid")

	sourceModulus = _mod;
}
int MESSAGE::getSourceModulus(void){
	DEBINFMESSAGE("int MESSAGE::getSourceModulus(void)",this<<"][")
	if (invalid == 1)
		DEBASSERT("MESSAGE::setKey invalid")

	DEBINFMESSAGE("int MESSAGE::getSourceModulus(void)",this<<"]["<<sourceModulus)
	return sourceModulus;
}
void MESSAGE::setValid(int _valid){
	DEBINFMESSAGE("void MESSAGE::setValid(int _valid)",this<<"]["<<_valid)
	invalid = _valid;
}
bool MESSAGE::getLock(void){
	DEBINFMESSAGE("bool MESSAGE::getLock(void)",this<<"][")
	if (invalid == 1)
		DEBASSERT("MESSAGE::getLock invalid")

	DEBINFMESSAGE("bool MESSAGE::getLock(void)",this<<"]["<<lock)
	return lock;
}
void MESSAGE::unSetLock(CALL_OSET* _call_oset){
	DEBINFMESSAGE("void MESSAGE::unSetLock(CALL_OSET* _call_oset)",this<<"]["<<_call_oset)
	if (invalid == 1){
		DEBASSERT("MESSAGE::unSetLock invalid")}

	_call_oset->removeLockedMessage(this);
	lock=false;
}
void MESSAGE::setLock(CALL_OSET* _call_oset){
	DEBINFMESSAGE("void MESSAGE::setLock(CALL_OSET* _call_oset)",this<<"]["<<_call_oset)
	if (invalid == 1){
		DEBASSERT("MESSAGE::setLock invalid")}

	_call_oset->insertLockedMessage(this);
	lock=true;
}
string MESSAGE::getKey(void){
	DEBINFMESSAGE("string MESSAGE::getKey(void)",this<<"][")
	if (invalid == 1)
		DEBASSERT("MESSAGE::getKey invalid")

	DEBINFMESSAGE("string MESSAGE::getKey(void)",this<<"]["<<key)
	return key;
}
void MESSAGE::setKey(string _key){
	DEBINFMESSAGE("void MESSAGE::setKey(string _key)",this<<"]["<<_key)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setKey invalid")

	key = _key;
}
int MESSAGE::fillIn(void){
	DEBINFMESSAGE("int MESSAGE::fillIn(void)",this<<"][")
	if (invalid == 1)
		DEBASSERT("MESSAGE::fillIn invalid")

	TIMEDEF
	SETNOW

	if(messageStatus == 1){
		DEBINFMESSAGE("int MESSAGE::fillIn(void)",this<<"][already filled")
		return message_line.size();
		PRINTDIFF("MESSAGE::fillIn already filled")
	}

	if(messageStatus == 2){
		DEBINFMESSAGE("int MESSAGE::fillIn(void)",this<<"][message changed")
		compileMessage();
	}

	fillCounter++;
	if (fillCounter > 2){
		DEBINFMESSAGE("int MESSAGE::fillIn(void) fillCounter",this<<"] fillCounter["<<fillCounter<<"] [compileCounter ["<<compileCounter)
		DEBWARNING("int MESSAGE::fillIn(void) > 2",this)
		DEBASSERT("")
	}

	//messageStatus == 0
	//serve???
	DEBINFMESSAGE("****************** original message",original_message)
	if (message_char != 0x0){
		DELPTRARR(message_char,"message_char")
	}
	NEWPTR2(message_char, char[strlen(original_message)+1],"message_char "<<strlen(original_message)+1)
	strcpy(message_char, original_message);
	DEBINFMESSAGE("****************** message_char",message_char)

	_clearStatus();


	bool fillSDP = false;
	char *saveptr1;
	char* tok = strtok_r(message_char, "\n",&saveptr1);
	while (tok != NULL){
		DEBINFMESSAGE("MESSAGE::tok", tok)
		DEBOUT("tok!", tok)
		//SDP of payload first line empty
		if (strlen(tok) == 1){
			fillSDP = true;
			hasSdp = true;
			DEBOUT("has sdp", tok)
		}
		//cut ^M
		int ll = strlen(tok);
		char* trok = tok + ll - 1 ;
		strcpy(trok,"");
		if (!fillSDP){
			if (strncmp(tok,"Via:", 4) == 0){
				DEBINFMESSAGE("MESSAGE::fillIn via", tok)
				hasvialines = true;
				via_line.push_back( make_pair (tok,false) );
			}
			else{
				DEBINFMESSAGE("MESSAGE::fillIn line", tok)
				message_line.push_back( make_pair (tok,false) );
			}
		}
		else{
			DEBINFMESSAGE("MESSAGE::fillIn sdp", tok)
			sdp_line.push_back( make_pair(tok,false));
		}
		tok = strtok_r(NULL, "\n",&saveptr1);
	}
	messageStatus = 1;

	DEBINFMESSAGE("int MESSAGE::fillIn(void)", this<<"]["<<message_line.size())
	//_dumpMessageBuffer();
	PRINTDIFF("MESSAGE::fillIn filled")
	return message_line.size();

}
bool MESSAGE::isFilled(void){
	DEBINFMESSAGE("bool MESSAGE::isFilled(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::isFilled invalid")

	DEBINFMESSAGE("bool MESSAGE::isFilled(void)", messageStatus)
	return (messageStatus == 1);
}
int MESSAGE::getDimString(void){
	DEBINFMESSAGE("int MESSAGE::getDimString(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getDimString invalid")

	DEBINFMESSAGE("int MESSAGE::getDimString(void)", this<<"]["<<strlen(original_message))
	return strlen(original_message);
}
char* MESSAGE::getOriginalString(void){
	DEBINFMESSAGE("char* MESSAGE::getOriginalString(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getOriginalString invalid")

	DEBINFMESSAGE("char* MESSAGE::getOriginalString(void)", this<<"][\n"<<original_message)
	return original_message;
}
bool MESSAGE::hasSDP(void){
	DEBINFMESSAGE("bool MESSAGE::hasSDP(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::hasSDP invalid")

	if(messageStatus !=1){
		fillIn();
	}
	DEBINFMESSAGE("bool MESSAGE::hasSDP(void)", this<<"]["<<hasSdp)
	return hasSdp;
}
string MESSAGE::getFirstLine(void){
	DEBINFMESSAGE("string MESSAGE::getFirstLine(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getFirstLine invalid")
	TIMEDEF
	SETNOW
	if(messageStatus !=1){
		fillIn();
	}
	DEBINFMESSAGE("string MESSAGE::getFirstLine(void)", this<<"]["<<message_line[0].first)
	PRINTDIFF("MESSAGE::getFirstLine")
	return message_line[0].first;
}
char* MESSAGE::getMessageBuffer(void){
	DEBINFMESSAGE("char* MESSAGE::getMessageBuffer(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getMessageBuffer invalid")

//	if(messageStatus !=1){
//		fillIn();
//	}
	DEBINFMESSAGE("char* MESSAGE::getMessageBuffer(void)",this<<"]["<<original_message)
	return original_message;
}
void MESSAGE::compileMessage(void){
	DEBINFMESSAGE("void MESSAGE::compileMessage(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::compileMessage invalid")

	//build the original_message

	if(messageStatus == 0){
		DEBINFMESSAGE("void MESSAGE::compileMessage(void) not filled, do nothing", this)
		DEBWARNING("void MESSAGE::compileMessage(void) not filled, do nothing", this);
		return;
	}
	if(messageStatus == 1 ){
		//not changed
		DEBINFMESSAGE("void MESSAGE::compileMessage(void) not changed, do nothing", this)
		DEBWARNING("void MESSAGE::compileMessage(void) not changed, do nothing", this);
		return;
	}
	//messageStatus==2
	compileCounter++;
	if(compileCounter > 2){
		DEBINFMESSAGE("int MESSAGE::compileMessage(void) compileCounter",this<<"] fillCounter["<<fillCounter<<"] [compileCounter ["<<compileCounter)
		DEBWARNING("int MESSAGE::compileMessage(void) > 2",this)
		DEBASSERT("compileCounter")
	}


	int sizeOfSdp = 0;

	if (hasSdp){
		//calculate size
		for(unsigned int i = 1; i < sdp_line.size(); i ++){
			sizeOfSdp += strlen(sdp_line[i].first) + 2;
		}
	}

	char mess[ECHOMAX];
	char* messX = mess;

	for(unsigned int i = 0; i < message_line.size(); i ++){
		if (strncmp(message_line[i].first,"Content-Length: ", 16) != 0 &&
				strncmp(message_line[i].first,"Content-Type: ", 14) != 0
				&& strncmp(message_line[i].first,"xxDxx",5) != 0
				&& strlen(message_line[i].first)!=0 ){
			sprintf(messX,"%s\r\n",message_line[i].first);
		}
		if (message_line[i].second){
			DELPTRARR(message_line[i].first,"message_line "<<i)
		}
		//start where the last sprintf has put the \0
		messX = messX+strlen(messX);
	}
	for(unsigned int i = 0; i < via_line.size(); i ++){
		sprintf(messX,"%s\r\n",via_line[i].first);
		if (via_line[i].second){
			DELPTRARR(via_line[i].first,"via_line")
		}
		messX = messX+strlen(messX);
	}
	if(!hasSdp){
		strcpy(messX,"Content-Length: 0\r\n");
	}
	messX = messX+strlen(messX);



	if (hasSdp){
		strcpy(messX,"Content-Type: application/sdp\r\n");
		messX = messX+strlen(messX);
		sprintf(messX,"Content-Length: %d\r\n",sizeOfSdp);
		messX = messX+strlen(messX);
		for(unsigned int i = 0; i < sdp_line.size(); i ++){
			sprintf(messX,"%s\r\n",sdp_line[i].first);
			messX = messX+strlen(messX);
			if (sdp_line[i].second){
				DELPTRARR(sdp_line[i].first,"sdp_line")
			}

		}
	}
	else{
		strcpy(messX,"\r\n");
	}

	messageStatus = 0;
	_clearStatus();

	DEBINFMESSAGE("void MESSAGE::compileMessage(void) origmess.str()", mess)

	DELPTRARR(original_message,"original_message")
	NEWPTR2(original_message, char[strlen(mess)+1],"original_message "<<strlen(mess)+1)
	strcpy(original_message,mess);
	DEBINFMESSAGE("void MESSAGE::compileMessage(void) original_message", original_message)

	DELPTRARR(message_char,"message_char")
	message_char = 0x0;
}
void MESSAGE::dumpMessageBuffer(void){
	DEBINFMESSAGE("void MESSAGE::dumpMessageBuffer(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::dumpMessageBuffer invalid")
#ifndef LOGMIN
		DEBASSERT("MESSAGE::dumpMessageBuffer active")
#endif

	for(unsigned int i = 0; i < message_line.size(); i ++){
		DEBINFMESSAGE("MESSAGE::dumpMessageBuffer m", message_line[i].second << "][" << message_line[i].first)
	}
	for(unsigned int i = 0; i < via_line.size(); i ++){
		DEBINFMESSAGE("MESSAGE::dumpMessageBuffer v", via_line[i].second << "][" << via_line[i].first)
	}
	for(unsigned int i = 0; i < sdp_line.size(); i ++){
		DEBINFMESSAGE("MESSAGE::dumpMessageBuffer s", sdp_line[i].second << "][" << sdp_line[i].first)
	}
}
void MESSAGE::_dumpMessageBuffer(void){
	DEBINFMESSAGE("void MESSAGE::_dumpMessageBuffer(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::_dumpMessageBuffer invalid")

#ifndef LOGMIN
		DEBASSERT("MESSAGE::dumpMessageBuffer active")
#endif

	for(unsigned int i = 0; i < message_line.size(); i ++){
		DEBOUT("MESSAGE::_dumpMessageBuffer m", message_line[i].second << "][" << message_line[i].first)
	}
	for(unsigned int i = 0; i < via_line.size(); i ++){
		DEBOUT("MESSAGE::_dumpMessageBuffer v", via_line[i].second << "][" << via_line[i].first)
	}
	for(unsigned int i = 0; i < sdp_line.size(); i ++){
		DEBOUT("MESSAGE::_dumpMessageBuffer s", sdp_line[i].second << "][" << sdp_line[i].first)
	}

}
int MESSAGE::getSock(void){
	DEBINFMESSAGE("int MESSAGE::getSock(void)",this)
	if (invalid == 1)
		DEBASSERT("FMESSAGE::getSock invalid")

	DEBINFMESSAGE("int MESSAGE::getSock(void)",this<<"]["<<sock)
	return sock;
}
struct sockaddr_in MESSAGE::getEchoClntAddr(void){
	DEBINFMESSAGE("struct sockaddr_in MESSAGE::getEchoClntAddr(void)",this)
	if (invalid == 1)
		DEBASSERT("FMESSAGE::getEchoClntAddr invalid")

	return echoClntAddr;
}
int MESSAGE::getRequestDirection(void){
	DEBINFMESSAGE("int MESSAGE::getRequestDirection(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getRequestDirection invalid")

	DEBINFMESSAGE("int MESSAGE::getRequestDirection(void)",this<<"]["<<requestDirection)
	return requestDirection;
}
void MESSAGE::setRequestDirection(int _dir){
	DEBINFMESSAGE("void MESSAGE::setRequestDirection(int _dir)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setRequestDirection invalid")

	DEBINFMESSAGE("void MESSAGE::setRequestDirection(int _dir)",this<<"]["<<_dir)
	requestDirection = _dir;
}
int MESSAGE::getGenEntity(void){
	DEBINFMESSAGE("int MESSAGE::getGenEntity(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getGenEntity invalid")

	DEBINFMESSAGE("int MESSAGE::getGenEntity(void)",this<<"]["<<genEntity)
	return genEntity;
}
void MESSAGE::setGenEntity(int _gen){
	DEBINFMESSAGE("void MESSAGE::setGenEntity(int _gen)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setGenEntity invalid")

	DEBINFMESSAGE("void MESSAGE::setGenEntity(int _gen)",this<<"]["<<_gen)
	genEntity = _gen;
}
void MESSAGE::setDestEntity(int _destEntity){
	DEBINFMESSAGE("void MESSAGE::setDestEntity(int _destEntity)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setDestEntity invalid")

	DEBINFMESSAGE("void MESSAGE::setDestEntity(int _destEntity)",this<<"]["<<_destEntity)
	destEntity = _destEntity;
}
int MESSAGE::getDestEntity(void){
	DEBINFMESSAGE("int MESSAGE::getDestEntity(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getDestEntity invalid")

	DEBINFMESSAGE("int MESSAGE::getDestEntity(void)", this<<"]["<<destEntity)
	return destEntity;
}
bool MESSAGE::natTraversal(void){
	DEBINFMESSAGE("lli MESSAGE::natTraversal(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::natTraversal invalid")

	return (natAddress.length()>0);
}
void MESSAGE::setNatTraversal(string _address, string _port){
	DEBINFMESSAGE("lli MESSAGE::setNatTraversal(void)", this<<"]["<<_address<<"]["<<_port)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setNatTraversal invalid")
	natAddress = _address;
	natPort = _port;
}
pair < string, string > MESSAGE::getNatAddress(void){
	DEBINFMESSAGE("lli MESSAGE::getNatAddress(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getNatAddress invalid")

		return make_pair(natAddress,natPort);
}

lli MESSAGE::getFireTime(void){
	DEBINFMESSAGE("lli MESSAGE::getFireTime(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getFireTime invalid")

	DEBINFMESSAGE("lli MESSAGE::getFireTime(void)", this<<"]["<<fireTime)
	return fireTime;
}
void MESSAGE::setFireTime(lli _firetime){
	DEBINFMESSAGE("void MESSAGE::setFireTime(lli _firetime)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setFireTime invalid")

	DEBINFMESSAGE("void MESSAGE::setFireTime(lli _firetime)",this<<"]["<<_firetime)
	fireTime = _firetime;
}
string MESSAGE::getOrderOfOperation(void){
	DEBINFMESSAGE("string MESSAGE::getOrderOfOperation(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getOrderOfOperation invalid")

	DEBINFMESSAGE("string MESSAGE::getOrderOfOperation(void)", this<<"]["<<orderOfOperation)
	return orderOfOperation;
}
void MESSAGE::setOrderOfOperation(string _s){
	DEBINFMESSAGE("void MESSAGE::setOrderOfOperation(string _s)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setOrderOfOperation invalid")

	DEBINFMESSAGE("void MESSAGE::setOrderOfOperation(string _s)", this<<"]["<<_s)
	orderOfOperation = _s;
}
int MESSAGE::getType_trnsct(void){
	DEBINFMESSAGE("int MESSAGE::getType_trnsct(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getType_trnsct invalid")

	DEBINFMESSAGE("int MESSAGE::getType_trnsct(void)",this<<"]["<<type_trnsct)
	return type_trnsct;
}
void MESSAGE::setType_trnsct(int _t){
	DEBINFMESSAGE("void MESSAGE::setType_trnsct(int _t)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setType_trnsct invalid")

	DEBINFMESSAGE("void MESSAGE::setType_trnsct(int _t)", this<<"]["<<_t)
	type_trnsct = _t;
}
int MESSAGE::getTypeOfInternal(void){
	DEBINFMESSAGE("int MESSAGE::getTypeOfInternal(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getTypeOfInternal invalid")

	DEBINFMESSAGE("int MESSAGE::getTypeOfInternal(void)", this<<"]["<<typeOfInternal)
	return typeOfInternal;

}
void MESSAGE::setTypeOfInternal(int _toi){
	DEBINFMESSAGE("void MESSAGE::setTypeOfInternal(int _toi)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setTypeOfInternal invalid")

	DEBINFMESSAGE("void MESSAGE::setTypeOfInternal(int _toi)", this<<"]["<<_toi)
	typeOfInternal = _toi;
}
int MESSAGE::getTypeOfOperation(void){
	DEBINFMESSAGE("int MESSAGE::getTypeOfOperation(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getTypeOfOperation invalid")

	DEBINFMESSAGE("int MESSAGE::getTypeOfOperation(void)",this<<"]["<<typeOfOperation)
	return typeOfOperation;
}
void MESSAGE::setTypeOfOperation(int _t){
	DEBINFMESSAGE("void MESSAGE::setTypeOfOperation(int _t)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setTypeOfOperation invalid")

	DEBINFMESSAGE("void MESSAGE::setTypeOfOperation(int _t)",this<<"]["<<_t)
	typeOfOperation = _t;
}
int MESSAGE::getModulus(void){
	TIMEDEF
	SETNOW
#ifndef USEMAPMODUL
	DEBINFMESSAGE("int MESSAGE::getModulus(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getModulus invalid")

	if (modulus != -1){
		DEBINFMESSAGE("int MESSAGE::getModulus(void)",this<<"]["<<modulus)
		return modulus;
	}
	//is calculated in two ways:
	// if first 5 chars of call id is "CoMap" then
	// the modulus is the number after it (two digits)
	// otherwise is calculated

	string s = getHeadCallId();
	if (s.substr(0,5).compare("CoMap") == 0){
		modulus = atoi(s.substr(5,COMAPS_DIG).c_str());
	}else {
		char x[64];
		int k = 64<s.length() ? 64 : s.length();
		sprintf(x,"%s", s.substr(0,k).c_str());
		long long int tot=0;
		for (int i = 0; i < k ; i++){
			tot =  (long long int) atol(s.substr(i,3).c_str()) + tot;
		}
		DEBINFMESSAGE("int MESSAGE::getModulus(void) tot",this<<"]["<<tot)
		modulus = tot%COMAPS;
	}
	DEBINFMESSAGE("int MESSAGE::getModulus(void) modulus",this<<"]["<<modulus)
	return modulus;
#else
#ifndef MAPMODULHYBRID

	DEBINFMESSAGE("int MESSAGE::getModulus(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getModulus invalid")

	if (modulus != -1){
		DEBINFMESSAGE("int MESSAGE::getModulus(void)",this<<"]["<<modulus)
		return modulus;
	}

	string s = getHeadCallId();
	DEBOUT("int MESSAGE::getModulus(void) getHeadCallId", s)
	if (s.substr(0,5).compare("CoMap") == 0){
		modulus = atoi(s.substr(5,COMAPS_DIG).c_str());
		DEBOUT("int MESSAGE::getModulus(void) comap", modulus)
		return modulus;
	}

	map<const string, int>::iterator itm;
    GETLOCK(&modulusMapMtx,"modulusMapMtx",23);
	itm = modulusMap.find(s);
	if(itm != modulusMap.end()){
		RELLOCK(&modulusMapMtx,"modulusMapMtx")
		DEBOUT("int MESSAGE::getModulus(void) itm->second", itm->second)
		return itm->second;
	}
	modulus = modulusIter;
	modulusMap.insert(make_pair(s,modulus));
	modulusIter++;
	modulusIter = modulusIter % COMAPS;
	RELLOCK(&modulusMapMtx,"modulusMapMtx")
	DEBOUT("int MESSAGE::getModulus(void) modulus", modulus)
	return modulus;
#else

	string s1,s2;
	if (SUDP::getRealm() == CALL_REALM){
		s1 = getHeadCallId();
		s2 = s1;
	}else if (SUDP::getRealm() == MESSAGE_REALM){
		s1 = getFromUser();
		//This is because the reply has the comap id in the call id
		s2 = getHeadCallId();
	}

	DEBINFMESSAGE("int MESSAGE::getModulus(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getModulus invalid")

	if (modulus != -1){
		DEBINFMESSAGE("int MESSAGE::getModulus(void)",this<<"]["<<modulus)
		PRINTDIFF("MESSAGE::getModulus already parsed")
		return modulus;
	}
	DEBOUT("int MESSAGE::getModulus(void) getHeadCallId", s2)
	if (s2.substr(0,5).compare("CoMap") == 0){
		modulus = atoi(s2.substr(5,COMAPS_DIG).c_str());
		DEBOUT("int MESSAGE::getModulus(void) comap", modulus)
		PRINTDIFF("MESSAGE::getModulus CoMap")
		return modulus;
	}

	int premod = getPreModulus(s1.c_str());

	map<const string, int>::iterator itm;
	GETLOCK(&(modulusMapMtx[premod]),"modulusMapMtx "<<premod,23);
	itm = modulusMap[premod].find(s1);
	if(itm != modulusMap[premod].end()){
		RELLOCK(&(modulusMapMtx[premod]),"modulusMapMtx "<<premod)
		DEBOUT("int MESSAGE::getModulus(void) itm->second", itm->second)
		PRINTDIFF("MESSAGE::getModulus calc")
		return itm->second;
	}
	modulus = modulusIter[premod];
	modulusMap[premod].insert(make_pair(s1,modulus));
	modulusIter[premod]++;
	modulusIter[premod] = modulusIter[premod] % COMAPS;
	RELLOCK(&(modulusMapMtx[premod]),"modulusMapMtx "<<premod)
	DEBOUT("int MESSAGE::getModulus(void) modulus", modulus)
	PRINTDIFF("MESSAGE::getModulus calc")
	return modulus;


#endif
#endif
}
void MESSAGE::purgeSDP(void){
	DEBINFMESSAGE("void MESSAGE::purgeSDP(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::purgeSDP invalid")

	if(messageStatus ==0){
		fillIn();
	}
	hasSdp = false;
	for(unsigned int i = 0; i < sdp_line.size(); i ++){
		if(sdp_line[i].second){
			DELPTRARR(sdp_line[i].first,"sdp_line"<<i)
		}
	}
	messageStatus = 2;
}
vector< pair<char*, bool> > MESSAGE::getSDP(void){
	DEBINFMESSAGE("vector< pair<char*, bool> > MESSAGE::getSDP(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::purgeSDP invalid")

	if(messageStatus !=1){
		fillIn();
	}

	return sdp_line;
}
void MESSAGE::setSDP(vector< pair<char*, bool> > _vector){
	DEBINFMESSAGE("void MESSAGE::setSDP(vector< pair<char*, bool> > _vector)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setSDP invalid")

	if(messageStatus == 0){
		fillIn();
	}

	hasSdp = true;

	for(unsigned int i = 0; i < sdp_line.size(); i ++){
		if(sdp_line[i].second){
			DELPTRARR(sdp_line[i].first,"sdp_line"<<i)
		}
	}

	sdp_line.clear();
	//Need to duplicate each sdp line
	for(unsigned int i = 0; i < _vector.size(); i ++){
		DEBY
		char* sdp_l;
		NEWPTR2(sdp_l, char[strlen(_vector[i].first) + 1],"sdp_line")
		strcpy(sdp_l,_vector[i].first);
		sdp_line.push_back(make_pair(sdp_l,true));
	}
	messageStatus = 2;

}
void MESSAGE::setGenericHeader(string _header, string _content){
	DEBINFMESSAGE("void MESSAGE::setGenericHeader(string _header, string _content)",this<<"]["<<_header<<"]["<<_content)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setGenericHeader invalid")


	if(messageStatus == 0){
		fillIn();
	}

	messageStatus = 2;

	if (_header.compare("Via:") == 0){
		DEBASSERT("MESSAGE::setGenericHeader invalid for via")
	}
	if (_header.compare("REPLY") == 0){
		//SIP/2.0 xxx
		char* newfirstline;
		NEWPTR2(newfirstline, char[_content.length() + 9],"message_line")
		sprintf(newfirstline,"SIP/2.0 %s",_content.c_str());
		message_line[0].first = newfirstline;
		message_line[0].second = true;
		return;
	}
	if (_header.compare("REQUEST") == 0){
		char* newfirstline;
		NEWPTR2(newfirstline, char[_content.length() + 1],"message_line")
		strcpy(newfirstline,_content.c_str());
		message_line[0].first = newfirstline;
		message_line[0].second = true;
		return;
	}


	size_t i;
	bool found = false;
	for(i = 1; i < message_line.size(); i ++){
		if(strncmp(message_line[i].first,_header.c_str(),_header.size()) == 0){

			if(message_line[i].second){
				//already updated, must delete the existing string
				DELPTRARR(message_line[i].first,"message_line "<<i)
			}
			char* newheader;
			NEWPTR2(newheader, char[_header.length() + 1 + _content.length() + 1],"message_line")
			message_line[i].first = newheader;
			sprintf(message_line[i].first,"%s %s",_header.c_str(), _content.c_str());
			message_line[i].second = true;
			found = true;
			break;
		}
	}
	if (!found) {
		DEBINFMESSAGE("MESSAGE::setGenericHeader not found",_header)
	}
	return;
}
string MESSAGE::getGenericHeader(string _header){
	DEBINFMESSAGE("string MESSAGE::getGenericHeader(string _header)",this<<"]["<<_header)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getGenericHeader invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if (_header.compare("Via:") == 0){
		int vl = via_line.size();
		if( vl !=0 ){
			DEBINFMESSAGE("string MESSAGE::getGenericHeader(string _header)",this<<"]["<<_header<<"]["<<via_line[vl-1].first + 5)
			return via_line[vl-1].first + 5;
		}
	}

	size_t i;
	bool found = false;
	for(i = 1; i < message_line.size(); i ++){
		if(strncmp(message_line[i].first,_header.c_str(), _header.size())==0){
			DEBINFMESSAGE("string MESSAGE::getGenericHeader(string _header)",this<<"]["<<_header<<"]["<<message_line[i].first + _header.size() + 1)
			return (message_line[i].first + _header.size() + 1);
		}
	}
	if (!found) {
		DEBSIP("MESSAGE::getGenericHeader not found",_header)
	}
	DEBINFMESSAGE("string MESSAGE::getGenericHeader(string _header)",this<<"]["<<_header<<"][notfound")
	return "";
}
bool MESSAGE::queryGenericHeader(string _header){
	if (invalid == 1)
		DEBASSERT("MESSAGE::queryGenericHeader invalid")

	DEBASSERT("")
}
void MESSAGE::addGenericHeader(string _header, string _content){
	DEBINFMESSAGE("void MESSAGE::addGenericHeader(string _header, string _content)",this<<"]["<<_header<<"]["<<_content)
	if (invalid == 1)
		DEBASSERT("MESSAGE::addGenericHeader invalid")

	if(messageStatus == 0){
		fillIn();
	}

	if (_header.compare("Via:") == 0){
		DEBASSERT("MESSAGE::addGenericHeader invalid for via")
		return;
	}
	//must compile each time to avoid duplicates
	string s = getGenericHeader(_header);
	if (s.length() != 0){
		DEBINFMESSAGE("void MESSAGE::addGenericHeader(string _header, string _content) replacing",this<<"]["<<_header<<"]["<<_content)
		setGenericHeader(_header, _content);
	}
	else{
		char* newheader;
		NEWPTR2(newheader, char[_header.length() + 1 + _content.length() + 1],"message_line")
		sprintf(newheader,"%s %s",_header.c_str(), _content.c_str());
		message_line.push_back(make_pair(newheader,true));
		DEBINFMESSAGE("void MESSAGE::addGenericHeader(string _header, string _content) adding",this<<"]["<<_header<<"]["<<_content)
	}
	messageStatus = 2;
	return;
}
void MESSAGE::dropHeader(string _header){
	DEBINFMESSAGE("void MESSAGE::dropHeader(string _header)",this<<"]["<<_header)
	if (invalid == 1)
		DEBASSERT("MESSAGE::dropHeader invalid")

	if(messageStatus == 0){
		fillIn();
	}

	if (_header.compare("Via:") == 0){
		DEBASSERT("MESSAGE::dropHeader invalid for via")
		return;
	}

	size_t i;
	bool found = false;
	vector< pair<char*, bool> >::iterator _find;
	_find = message_line.begin();
	for(i = 0; i < message_line.size(); i ++){
		if(strncmp(message_line[i].first,_header.c_str(), _header.length()) == 0 ){
			if (message_line[i].second){
				DELPTRARR(message_line[i].first,"message_line "<<i)
			}
			message_line.erase(_find);
			found = true;
			break;
		}
		_find ++;
	}
	if (!found) {
		DEBINFMESSAGE("MESSAGE::removeHeader not found",_header)
		return;
	}
	DEBINFMESSAGE("void MESSAGE::dropHeader(string _header) found",this<<"]["<<_header)
	messageStatus = 2;
	return;

}
string MESSAGE::getProperty(string _header,string _property){
	DEBINFMESSAGE("string MESSAGE::getProperty(string _header,string _property)",this<<"]["<<_header<<"]["<<_property)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getProperty invalid")

	if(messageStatus !=1){
		fillIn();
	}
	string tmpr;

	if (_header.compare("Via:") == 0){
		if (hasvialines){
			tmpr = _getProperty(via_line[0].first, _property);
			if (tmpr.compare("xxdxxdxx") == 0){
				return "";
			}
			return tmpr;
		}
		else{
			return "";
		}
	}
	else{
		for (size_t i = 0; i < message_line.size(); i++){
			if(strncmp(message_line[i].first,_header.c_str(), _header.length()) == 0){
				tmpr = _getProperty(message_line[i].first, _property);
				DEBOUT("tag",tmpr)
				return tmpr;
			}
			if (tmpr.compare("xxdxxdxx") == 0){
				DEBY
				return "";
			}
		}
		DEBY
		return "";
	}
}
string MESSAGE::_getProperty(string _fullstring,string __property){
	DEBINFMESSAGE("string MESSAGE::_getProperty(string _fullstring,string __property)",this<<"]["<<_fullstring<<"]["<<__property)

	char xxx[_fullstring.length() +1];
	strcpy(xxx,_fullstring.c_str());
	string _property = ";" + __property;

	char* aaa = strstr(xxx + _property.length(), _property.c_str());

	if ( aaa != NULL ){
		if (strncmp(aaa + _property.length(),";",1) == 0){
			//caso a senza valore ma in mezzo
			DEBINFMESSAGE("string MESSAGE::_getProperty(string _fullstring,string __property) return",this<<"]["<<_fullstring<<"]["<<__property<<"][")
			return "";
		}
		else if (strncmp(aaa + _property.length(),"\0",1) == 0){
			//caso b senza valore alla fine
			DEBINFMESSAGE("string MESSAGE::_getProperty(string _fullstring,string __property) return",this<<"]["<<_fullstring<<"]["<<__property<<"][")
			return "";
		}
		else if (strncmp(aaa + _property.length(),"=",1) == 0){
			//caso con valore
			DEBOUT("",aaa)
			char* bbb;
			if (aaa!=NULL){
				bbb = strchr(aaa+_property.length(),';');
				DEBOUT("",bbb)

			}
			if (bbb != NULL){
				strcpy(bbb,"");
				DEBOUT("",bbb)
			}
			DEBINFMESSAGE("string MESSAGE::_getProperty(string _fullstring,string __property) return",this<<"]["<<_fullstring<<"]["<<__property<<"]["<<aaa+_property.length()+1)
			DEBOUT("",aaa)
			return (aaa+_property.length() + 1);
		}
	}
	else {
		DEBY
		DEBINFMESSAGE("string MESSAGE::_getProperty(string _fullstring,string __property) return",this<<"]["<<_fullstring<<"]["<<__property<<"][xxdxxdxx")
		return "xxdxxdxx";
	}

}
void MESSAGE::setProperty(string _head,string __property,string _value){
	DEBINFMESSAGE("void MESSAGE::setProperty(string _head,string __property,string _value)",this<<"]["<<_head<<"]["<<__property<<"]["<<_value)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setProperty invalid")

	if(messageStatus == 0){
		fillIn();
	}
	messageStatus = 2;

	string _fullstring = "";
	int idx = -1;
	bool isVia = false;

	if (_head.compare("Via:") == 0){
		if (hasvialines){
			_fullstring = via_line[0].first;
			isVia = true;
		}else{
			DEBINFMESSAGE("void MESSAGE::setProperty(string _head,string __property,string _value) nothing done",this)
			return;
		}
	}else{
		for (int i = 0; i < message_line.size(); i++){
			if(strncmp(message_line[i].first,_head.c_str(), _head.length()) == 0){
				_fullstring = message_line[i].first;
				idx = i;
			}
		}
	}
	if (idx == -1 &&!isVia){
		DEBINFMESSAGE("void MESSAGE::setProperty(string _head,string __property,string _value) nothing done",this)
		return;
	}

	char xxx[_fullstring.length() +1];
	char yyy[_fullstring.length() +1];
	strcpy(xxx,_fullstring.c_str());
	strcpy(yyy,_fullstring.c_str());

	string _1_property = __property + "=";
	string _2_property = __property + "=;";
	string _3_property = __property + ";";

	//if present delete it and put in yyy
	char* aaa = strstr(xxx, __property.c_str());
	if (aaa !=NULL){
		//present
		char* bbb = strstr(xxx, _1_property.c_str());
		if (bbb != NULL){
			char* ccc = strstr(xxx, _2_property.c_str());
			if (ccc!=NULL){
				//property=;
				DEBASSERT("")
			}else{
				//property=xxx...
				char* ddd = strstr(bbb,";");
				if( ddd != NULL){
					//property=xxxx;....
					//remove it
					*aaa = '\0';
					sprintf(yyy,"%s%s",xxx,ddd+1);
					DEBOUT("yyy",yyy)
				}else{
					//property=xxx last position
					char* _aaa = aaa-1;
					*_aaa = '\0';
					sprintf(yyy,"%s",xxx);
					DEBOUT("yyy",yyy)
				}

			}
		}else{
			//no value
			//property;
			char* eee = strstr(xxx, _3_property.c_str());
			if ( eee != NULL){
				char* fff = strstr(eee,";");
				*aaa = '\0';
				sprintf(yyy,"%s%s",xxx,fff+1);
				DEBOUT("yyy",yyy)
			}else{
				*aaa = '\0';
				sprintf(yyy,"%s",xxx);
				DEBOUT("yyy",yyy)
			}
		}

	}
	//use yyy which has not property set adn add it
	if (isVia){
		char* nx;
		NEWPTR2(nx, char[strlen(yyy) + __property.length() + _value.length() + 3],"via_line")
		sprintf(nx,"%s;%s=%s",yyy,__property.c_str(),_value.c_str());
		if (via_line[0].second){
			DELPTRARR(via_line[0].first,"via_line")
			via_line[0].first = nx;
		}else{
			via_line[0].second = true;
			via_line[0].first = nx;
		}
	}else{
		char* nx;
		NEWPTR2(nx, char[strlen(yyy) + __property.length() + _value.length() + 3],"message_line")
		sprintf(nx,"%s;%s=%s",yyy,__property.c_str(),_value.c_str());
		if (message_line[idx].second){
			DELPTRARR(message_line[idx].first,"message_line")
			message_line[idx].first = nx;
		}else{
			message_line[idx].second = true;
			message_line[idx].first = nx;
		}
	}
	return;

//	char xxx[_fullstring.length() +1];
//	strcpy(xxx,_fullstring.c_str());
//	string _property = ";" + __property;
//
//	char* aaa = strstr(xxx + _property.length(), _property.c_str());
//
//	if ( aaa != NULL ){
//		if (strncmp(aaa + _property.length(),";",1) == 0){
//			//caso a senza valore ma in mezzo
//			DEBY
//			char* yyy = aaa+1;
//			strcpy(yyy,"");
//			DEBINFMESSAGE("void MESSAGE::setProperty(string _head,string __property,string _value)",this<<"] first part xxx ["<<xxx);
//			yyy = aaa+_property.length();
//			DEBINFMESSAGE("void MESSAGE::setProperty(string _head,string __property,string _value)",this<<"] first part yyy ["<<yyy);
//			string newinse = xxx;
//			newinse += _property;
//			newinse += "=";
//			newinse += _value;
//			newinse += yyy;
//			DEBINFMESSAGE("void MESSAGE::setProperty(string _head,string __property,string _value)",this<<"] new line ["<<newinse);
//			if(isVia){
//				char* nx;
//				NEWPTR2(nx, char[newinse.length()+1],"via_line")
//				strcpy(nx,newinse.c_str());
//				via_line[0].first = nx;
//				via_line[0].second = true;
//			}
//			else{
//				char* nx;
//				NEWPTR2(nx, char[newinse.length()+1],"message_line")
//				strcpy(nx,newinse.c_str());
//				message_line[idx].first = nx;
//				message_line[idx].second = true;
//			}
//			return;
//		}
//		else if (strncmp(aaa + _property.length(),"\0",1) == 0){
//			//caso b senza valore alla fine
//			DEBY
//			char* yyy = aaa+1;
//			strcpy(yyy,"");
//			string newinse = xxx;
//			newinse += _property;
//			newinse += "=";
//			newinse += _value;
//			DEBINFMESSAGE("void MESSAGE::setProperty(string _head,string __property,string _value)",this<<"] new line ["<<newinse);
//			if(isVia){
//				char* nx;
//				NEWPTR2(nx, char[newinse.length()+1],"via_line")
//				strcpy(nx,newinse.c_str());
//				via_line[0].first = nx;
//				via_line[0].second = true;
//			}
//			else{
//				char* nx;
//				NEWPTR2(nx, char[newinse.length()+1],"message_line")
//				strcpy(nx,newinse.c_str());
//				message_line[idx].first = nx;
//				message_line[idx].second = true;
//			}
//			return;
//		}
//		else if (strncmp(aaa + _property.length(),"=",1) == 0){
//			//caso con valore
//			DEBY
//			char* bbb=NULL;
//			if (aaa!=NULL){
//				bbb = strchr(aaa+_property.length(),';');
//			}
//			if (bbb != NULL){
//				sprintf(bbb,"");
//				DEBY
//			}
//
//			char* yyy = aaa+1;
//			sprintf(yyy,"");
//			if (bbb!=NULL)
//				yyy = bbb+1;
//			string newinse = xxx;
//			//doppio ;
//			//newinse += _property;
//			newinse += __property;
//			newinse += "=";
//			newinse += _value;
//			if(bbb!=NULL){
//				newinse += ";";
//				newinse += yyy;
//			}
//			if(isVia){
//				char* nx;
//				NEWPTR2(nx, char[newinse.length()+1],"via_line")
//				strcpy(nx,newinse.c_str());
//				via_line[0].first = nx;
//				via_line[0].second = true;
//			}
//			else{
//				char* nx;
//				NEWPTR2(nx, char[newinse.length()+1],"message_line")
//				strcpy(nx,newinse.c_str());
//				message_line[idx].first = nx;
//				message_line[idx].second = true;
//			}
//			DEBINFMESSAGE("void MESSAGE::setProperty(string _head,string __property,string _value)",this<<"] new line ["<<newinse);
//			return;
//		}
//	}
//	else {
//		DEBY
//		//remove last char
//		string newinse = xxx;
//		//string newinse = _newinse.substr(0,_newinse.length());
//		newinse += _property;
//		newinse += "=";
//		newinse += _value;
//		if(isVia){
//			char* nx;
//			NEWPTR2(nx, char[newinse.length()+1],"via_line")
//			strcpy(nx,newinse.c_str());
//			via_line[0].first = nx;
//			via_line[0].second = true;
//		}
//		else{
//			char* nx;
//			NEWPTR2(nx, char[newinse.length()+1],"message_line")
//			strcpy(nx,newinse.c_str());
//			message_line[idx].first = nx;
//			message_line[idx].second = true;
//		}
//		DEBINFMESSAGE("void MESSAGE::setProperty(string _head,string __property,string _value)",this<<"] new line ["<<newinse);
//		return;
//	}


}
string MESSAGE::getViaLine(void){
	DEBINFMESSAGE("string MESSAGE::getViaLine(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getViaLine invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if (hasvialines){
		DEBINFMESSAGE("string MESSAGE::getViaLine(void)",this<<"]["<< via_line.front().first)
		return via_line.front().first;
	}
	else{
		DEBINFMESSAGE("string MESSAGE::getViaLine(void)",this<<"][")
		return "";
	}
}
string MESSAGE::getViaBranch(void){
	DEBINFMESSAGE("string MESSAGE::getViaBranch(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getViaProperty invalid")
	TIMEDEF
	SETNOW
	if(messageStatus !=1){
		fillIn();
	}

	if (!parsedBranch){
		branch = getProperty("Via:", "branch");
		parsedBranch = true;
	}
	DEBINFMESSAGE("string MESSAGE::getViaBranch(void)",this<<"]["<<branch)
	PRINTDIFF("MESSAGE::getViaBranch")
	return branch;
}
string MESSAGE::getViaUriHost(void){
	DEBINFMESSAGE("string MESSAGE::getViaUriHost(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getViaUriHost invalid")

	DEBWARNING("MESSAGE::getViaUriHost works with fixed values","")
	// Via: SIP/2.0/UDP sipsl.gugli.com:5060;branch=z9hG4bK1c40b01306836794445384;rport
	if(messageStatus !=1){
		fillIn();
	}
	if (viaUriParsed){
		DEBINFMESSAGE("string MESSAGE::getViaUriHost(void)",this<<"]["<<viaUriHost)
	}
	else {
		if(hasvialines){
			char workline[strlen(via_line[0].first) +1 - 17];
			strcpy(workline, via_line[0].first+17);

			char* bbb = strchr(workline,':');
			*bbb = '\0';
			char* ccc = bbb +1;
			char* ddd = strchr(ccc,';');
			*ddd = '\0';

			viaUriHost = workline;
			viaUriParsed = true;
			viaUriPort = atoi(ccc);
			viaUriProtocol = ccc;
			DEBINFMESSAGE("string MESSAGE::getViaUriHost(void)",this<<"]["<<viaUriHost)
		}
	}
	return viaUriHost;

}
int MESSAGE::getViaUriPort(void){
	DEBINFMESSAGE("int MESSAGE::getViaUriPort(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getViaUriPort invalid")

	DEBWARNING("MESSAGE::getViaUriHost works with fixed values","")

	if(messageStatus !=1){
		fillIn();
	}
	if (viaUriParsed){
	}
	else{
		getViaUriHost();
	}
	DEBINFMESSAGE("int MESSAGE::getViaUriPort(void)",this<<"]["<<viaUriPort)
	return viaUriPort;

}
string MESSAGE::getViaUriProtocol(void){
	DEBINFMESSAGE("int MESSAGE::getViaUriProtocol(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getViaUriPort invalid")

	DEBWARNING("MESSAGE::getViaUriHost works with fixed values","")

	if(messageStatus !=1){
		fillIn();
	}
	if (viaUriParsed){
	}
	else{
		getViaUriHost();
	}
	DEBINFMESSAGE("int MESSAGE::getViaUriPort(void)",this<<"]["<<viaUriPort)
	return viaUriProtocol;


}

bool MESSAGE::hasVia(void){
	DEBINFMESSAGE("bool MESSAGE::hasVia(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::hasVia invalid")

	if(messageStatus !=1){
		fillIn();
	}
	DEBINFMESSAGE("bool MESSAGE::hasVia(void)",this<<"]["<<hasvialines)

	return hasvialines;
}
void MESSAGE::popVia(void){
	DEBINFMESSAGE("void MESSAGE::popVia(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::popVia invalid")

	if(messageStatus ==0){
		fillIn();
	}

	messageStatus = 2;

	if(hasvialines){
		if(via_line[0].second){
			DELPTRARR(via_line[0].first,"via_line "<<0)
		}
		vector< pair<char*, bool> >::iterator _rem;
		_rem = via_line.begin();
		via_line.erase(_rem);
	}

	DEBWARNING("MESSAGE::popVia only removing [0]","")
}
void MESSAGE::pushNewVia(string _via){
	DEBINFMESSAGE("void MESSAGE::pushNewVia(string _via)", this<<"]["<<_via)
	if (invalid == 1)
		DEBASSERT("MESSAGE::pushNewVia invalid")

	if(messageStatus ==0){
		fillIn();
	}

	messageStatus = 2;
	char* newheader;
	NEWPTR2(newheader, char[_via.length()+6],"via_line")
	sprintf(newheader, "Via: %s", _via.c_str());
	vector< pair<char*, bool> >::iterator it;
	it = via_line.begin();
	via_line.insert(it, make_pair (newheader,true));


}
string MESSAGE::getHeadCallId(void){
	DEBINFMESSAGE("string MESSAGE::getHeadCallId(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadCallId invalid")

	TIMEDEF
	SETNOW

	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedCallId){
		callId = getGenericHeader("Call-ID:");
		parsedCallId = true;
	}
	DEBINFMESSAGE("string MESSAGE::getHeadCallId(void)",this<<"]["<<callId)
	PRINTDIFF("MESSAGE::getHeadCallId")
	return callId;

}
string MESSAGE::getDialogExtendedCID(void){
	DEBINFMESSAGE("string MESSAGE::getDialogExtendedCID(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getDialogExtendedCID invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedCallId){
		getHeadCallId();
	}
	if(!parsedFromTag){
		getFromTag();
	}
	DEBINFMESSAGE("string MESSAGE::getDialogExtendedCID(void)",this<<"]["<<callId<<fromTag)
	return callId + fromTag;
}
string MESSAGE::getFromTag(void){
	DEBINFMESSAGE("string MESSAGE::getFromTag(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getFromTag invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedFromTag){
		fromTag = getProperty("From:", "tag");
		parsedFromTag = true;
	}
	DEBINFMESSAGE("string MESSAGE::getFromTag(void)", this<<"]["<<fromTag)
	return fromTag;
}
string MESSAGE::getFromUser(void){
	DEBINFMESSAGE("string MESSAGE::getFromUser(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getFromUser invalid")

	if(messageStatus != 1){
		fillIn();
	}
	if (fromUser.length()!=0){
		return fromUser;
	}

	//From: "user" ...
	//From: user ..
	string fromLine = getGenericHeader("From:");
	DEBOUT("string fromLine",fromLine)
	char workline[fromLine.length() +1];
	strcpy(workline, fromLine.c_str());
	char* aaa = strstr(workline,"\"");
	if (aaa == NULL){
		// no ""
		aaa = strstr(workline," ");
		if (aaa == NULL){
			//malformed
			DEBOUT("string fromLine malformed",fromLine)
			fromUser =  "";
		}
		else{
			*aaa = '\0';
			DEBOUT("string fromLine",workline)
			fromUser = workline;
		}
	}else{
		DEBOUT("string fromLine",aaa+1)
		char* bbb = strstr(aaa+1,"\"");
		if (bbb == NULL){
			//malformed
			DEBOUT("string fromLine malformed",fromLine)
			fromUser = "";
		}
		else{
			*bbb = '\0';
			DEBOUT("string fromLine",aaa+1)
			fromUser = aaa+1;
		}
	}
	return fromUser;
}

string MESSAGE::getToTag(void){
	DEBINFMESSAGE("string MESSAGE::getToTag(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getToTag invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedToTag){
		DEBY
		toTag = getProperty("To:", "tag");
		parsedToTag = true;
	}
	DEBINFMESSAGE("string MESSAGE::getToTag(void)",this<<"]["<<toTag)
	return toTag;
}
string MESSAGE::getHeadTo(void){
	DEBINFMESSAGE("string MESSAGE::getHeadTo(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadTo invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedTo){
		headTo = getGenericHeader("To:");
		parsedTo = true;
	}
	DEBINFMESSAGE("string MESSAGE::getHeadTo(void)", this<<"]["<<headTo)
	return headTo;
}
string MESSAGE::getHeadToName(void){
	DEBINFMESSAGE("string MESSAGE::getHeadToName(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadToName invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedToName){
		DEBASSERT("")
	}
	DEBINFMESSAGE("string MESSAGE::getHeadToName(void)",this<<"]["<<headToName)
	return headToName;
}
string MESSAGE::getHeadToUri(void){
	DEBINFMESSAGE("string MESSAGE::getHeadToUri(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadToUri invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedToUri){
		DEBASSERT("")
	}
	DEBINFMESSAGE("string MESSAGE::getHeadToUri(void)",this<<"]["<<headToUri)
	return headToUri;
}
string MESSAGE::getHeadToParams(void){
	DEBINFMESSAGE("string MESSAGE::getHeadToParams(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadToParms invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedToParms){
		DEBASSERT("")
	}
	DEBINFMESSAGE("string MESSAGE::getHeadToParams(void)",this<<"]["<<headToParms)

	return headToParms;
}
string MESSAGE::getContactUri(void){
	DEBINFMESSAGE("string MESSAGE::getContact(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getContact invalid")

	if (parsedContact){
		return contactUri;
	}

	//Contact: <sip:majo@192.168.0.100:5062;transport=...>
	//Contact: <sip:192.168.0.100:5062>

	string cont = getGenericHeader("Contact:");
	//remove brackets <>
    char contch[cont.length() +1];
    strcpy(contch, cont.c_str());
    char* pun1 = strchr(contch,'<');
    if ( pun1 != NULL){
        char* pun2 = strchr(contch,'>');
        *pun2 = '\0';
		contactUri = pun1+5;
		DEBOUT("MESSAGE::getContactUri",contactUri)
    }
    else {
    	contactUri = contch;
		DEBOUT("MESSAGE::getContactUri",contactUri)
    }

    //majo@192.168.0.100:5062
    //192.168.0.100:5062
    char contch2[contactUri.length() +1];
    strcpy(contch2, contactUri.c_str());
    char* pun3 = strchr(contch2,'@');
    char* pun4;
	if (pun3 == NULL){
		pun3 = contch2;
		contactName = "";
		pun4 = pun3;
	}else{
		*pun3 = '\0';
		contactName = contch2;
		pun4 = pun3 +1;
	}
	DEBOUT("MESSAGE::getContactUri contactName",contactName)
	DEBOUT("pun4", pun4)

	//192.168.0.100:5062
    char* pun5 = strchr(pun4,':');
	if (pun5 == NULL){
		contactPort = "5060";
	}else{
		*pun5 = '\0';
		char* pun6 = strchr(pun5+1,';');
		if (pun6 != NULL){
			//;transport=...
			*pun6 = '\0';
		}
		contactPort = pun5 +1;
	}
	contactAddress = pun4;
	DEBOUT("MESSAGE::getContactUri contactAddress",contactAddress)
	DEBOUT("MESSAGE::getContactUri contactPort",contactPort)

	parsedContact = true;
	return contactUri;

}
string MESSAGE::getContactName(void){
	DEBINFMESSAGE("string MESSAGE::getContactName(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getContactName invalid")

	if (!parsedContact){
		getContactUri();
	}
	return contactName;
}
string MESSAGE::getContactAddress(void){
	DEBINFMESSAGE("string MESSAGE::getContactAddress(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getContactAddress invalid")

	if (!parsedContact){
		getContactUri();
	}
	return contactAddress;
}

string MESSAGE::getContactPort(void){
	DEBINFMESSAGE("string MESSAGE::getContactPort(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getContactPort invalid")

	if (!parsedContact){
		getContactUri();
	}
	return contactPort;
}
void MESSAGE::setRoute(string _r,string _p){
	DEBINFMESSAGE("int MESSAGE::setRoute(string _r,string _p)",this<<"]["<<_r<<"]["<<"]["<<_p)

	route_address = _r;
	route_port = _p;
	has_route = true;
	return;

}
bool MESSAGE::hasRoute(void){
	DEBINFMESSAGE("bool MESSAGE::hasRoute(void)",this)
	return has_route;
}
pair<string, string> MESSAGE::getRoute(void){
	DEBINFMESSAGE("pair<char*, bool> MESSAGE::getRoute(void)",this)

	return( make_pair(route_address,route_port));

}
void MESSAGE::unsetRoute(void){
	DEBINFMESSAGE("void MESSAGE::unsetRoute(void)",this)
	has_route = false;
}
int MESSAGE::getReqRepType(void){
	DEBINFMESSAGE("int MESSAGE::getReqRepType(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getReqRepType invalid")

	TIMEDEF
	SETNOW

	if(messageStatus !=1){
		fillIn();
	}

	if (reqRep != 0) {
		DEBINFMESSAGE("int MESSAGE::getReqRepType(void)",this<<"]["<<reqRep)
		PRINTDIFF("MESSAGE::getReqRepType - already parsed")
		return reqRep;
	}
	if(strncmp(message_line[0].first,"SIP",3) == 0){
		reqRep = REPSUPP;
		//Parse "SIP/2.0 200 OK"
		char num[4];
		strncpy(num, message_line[0].first+8, 3);
		num[3] = '\0';
		replyCode = atoi(num);
		char snum[strlen(message_line[0].first) +1];
		strcpy(snum, message_line[0].first+8);
		headSipReply = snum;
	}
	else if(strncmp(message_line[0].first,"INVITE",6) == 0){
		reqRep = REQSUPP;
		requestCode = INVITE_REQUEST;
		headSipRequest = "INVITE";
	}
	else if(strncmp(message_line[0].first,"ACK",3) == 0){
		reqRep = REQSUPP;
		requestCode = ACK_REQUEST;
		headSipRequest = "ACK";
	}
	else if(strncmp(message_line[0].first,"BYE",3) == 0){
		reqRep = REQSUPP;
		requestCode = BYE_REQUEST;
		headSipRequest = "BYE";
	}
	else if(strncmp(message_line[0].first,"CANCEL",6) == 0){
		reqRep = REQSUPP;
		requestCode = CANCEL_REQUEST;
		headSipRequest = "CANCEL";
	}
	else if(strncmp(message_line[0].first,"REGISTER",8) == 0){
		reqRep = REQSUPP;
		requestCode = REGISTER_REQUEST;
		headSipRequest = "REGISTER";
	}
	else if(strncmp(message_line[0].first,"MESSAGE",7) == 0){
		reqRep = REQSUPP;
		requestCode = MESSAGE_REQUEST;
		headSipRequest = "MESSAGE";
	}
	else if(strncmp(message_line[0].first,"PD-SIPSL",8) == 0){
		reqRep = RECOMMPD;
		headSipRequest = "PD-SIPSL";
	}
	else if(strncmp(message_line[0].first,"OM-SIPSL",8) == 0){
		reqRep = RECOMMOM;
		headSipRequest = "OM-SIPSL";
	}
	else{
		reqRep = REQUNSUPP;
		headSipRequest = "???";
	}
	DEBINFMESSAGE("int MESSAGE::getReqRepType(void)",this<<"]["<<reqRep)
	PRINTDIFF("MESSAGE::getReqRepType - parsed")
	return reqRep;

}
void MESSAGE::setHeadSipRequest(string _content){
	DEBINFMESSAGE("void MESSAGE::setHeadSipRequest(string _content)", this <<"]["<<_content)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setHeadSipRequest invalid")

	if(messageStatus == 0){
		fillIn();
	}

	messageStatus = 2;

	setGenericHeader("REQUEST", _content);
	DEBWARNING("MESSAGE::setHeadSipRequest change also other codes","")

}
void MESSAGE::setHeadSipReply(string _content){
	DEBINFMESSAGE("void MESSAGE::setHeadSipReply(string _content)", this <<"]["<<_content)
	if (invalid == 1)
		DEBASSERT("MESSAGE::setHeadSipReply invalid")

	if(messageStatus == 0){
		fillIn();
	}

	messageStatus = 2;


	setGenericHeader("REPLY", _content);

	DEBWARNING("MESSAGE::setHeadSipReply change also other codes","")

}
int  MESSAGE::getHeadSipRequestCode(void){
	DEBINFMESSAGE("int  MESSAGE::getHeadSipRequestCode(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadSipRequestCode invalid")

	TIMEDEF
	SETNOW

	if(messageStatus !=1){
		fillIn();
	}

	if (reqRep == 0){
		getReqRepType();
	}
	DEBINFMESSAGE("int  MESSAGE::getHeadSipRequestCode(void)",this<<"]["<<requestCode)
	PRINTDIFF("MESSAGE::getHeadSipRequestCode")
	return requestCode;
}
string  MESSAGE::getHeadSipRequest(void){
	DEBINFMESSAGE("string  MESSAGE::getHeadSipRequest(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadSipRequest invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if (reqRep == 0){
		getReqRepType();
	}
	DEBINFMESSAGE("string  MESSAGE::getHeadSipRequest(void)",this<<"]["<<headSipRequest)
	return headSipRequest;
}
int  MESSAGE::getHeadSipReplyCode(void){
	DEBINFMESSAGE("int  MESSAGE::getHeadSipReplyCode(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadSipReplyCode invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if (reqRep == 0){
		getReqRepType();
	}
	DEBINFMESSAGE("int  MESSAGE::getHeadSipReplyCode(void)",this<<"]["<<replyCode)
	return replyCode;

}
string  MESSAGE::getHeadSipReply(void){
	DEBINFMESSAGE("string  MESSAGE::getHeadSipReply(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadSipReply invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if (reqRep == 0){
		getReqRepType();
	}
	DEBINFMESSAGE("string  MESSAGE::getHeadSipReply(void)",this<<"]["<<headSipReply)
	return headSipReply;

}
void MESSAGE::_forceHeadSipReply(int _code){
	DEBINFMESSAGE("string  MESSAGE::_forceHeadSipReply(int)",this<<"]["<<_code)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadSipReply invalid")
	reqRep = REPSUPP;
	replyCode = _code;
}

bool MESSAGE::isUriSecure(string header){
	DEBINFMESSAGE("bool MESSAGE::isUriSecure(string header)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::isUriSecure invalid")
	if(messageStatus !=1){
		fillIn();
	}
	DEBASSERT("")
}
//pair<string,int> MESSAGE::getUri(string header){
//	DEBINFMESSAGE("string MESSAGE::getUriHost(string header)",this<<"]["<<header)
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getUriHost invalid")
//
//	if(messageStatus !=1){
//		fillIn();
//	}
//
//	string return1;
//	int return2 = 0;
//
//	if(header.compare("REQUEST") == 0){
//		//INVITE sip:service@10.21.99.79:5062 SIP/2.0
//		//string Via="INVITE sip:service@10.21.99.79:5062 SIP/2.0";
//		char* xxx = new char[strlen(message_line[0].first) + 1];
//		strcpy(xxx,message_line[0].first);
//		char* aaa = strstr(xxx," ");
//		char* bbb = strstr(aaa+1," ");
//		strcpy(bbb,"");
//		char* ccc = aaa+1;
//		char* ddd = strstr(ccc,"@");
//		char* eee = ddd+1;
//		char* fff = strstr(eee,":");
//		if (fff != NULL){
//			strcpy(fff,"");
//			return2 = atoi(fff+1);
//		}
//		return1 = eee;
//		DEBINFMESSAGE("pair<string,int> MESSAGE::getUri(string header)",this<<"]["<<header<<"]["<<return1<<"]["<<return2)
//		return(make_pair(return1,return2));
//	}
//	return (make_pair("",0));
//
//}
pair<string,string> MESSAGE::getRequestUriProtocol(void){
	DEBINFMESSAGE("string MESSAGE::getRequestUriProtocol(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getRequestUriProtocol invalid")

	if(messageStatus !=1){
		fillIn();
	}

	if (requestUriProtocol.first.length()==0){
		requestUriProtocol = getUriProtocol("REQUEST");
		if (requestUriProtocol.second.length() == 0){
			requestUriProtocol.second = "5060";
		}
	}
	return requestUriProtocol;
}

pair<string,string> MESSAGE::getUriProtocol(string header){
	DEBINFMESSAGE("string MESSAGE::getUriProtocol(string header)",this<<"]["<<header)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getUriProtocol invalid")

	if(messageStatus !=1){
		fillIn();
	}

	string return1;
	if(header.compare("REQUEST") == 0){
		//INVITE sip:service@10.21.99.79:5062 SIP/2.0
		//string Via="INVITE sip:service@10.21.99.79:5062 SIP/2.0";
		char* xxx = new char[strlen(message_line[0].first) + 1];
		strcpy(xxx,message_line[0].first);
		char* aaa = strstr(xxx," ");
		char* bbb = strstr(aaa+1," ");
		strcpy(bbb,"");
		char* ccc = aaa+1;
		char* ddd = strstr(ccc,"@");
		char* eee = ddd+1;
		char* fff = strstr(eee,":");
		if (fff != NULL){
			strcpy(fff,"");
			return1 = eee;
			return(make_pair(return1,fff+1));
		}else{
			return1 = eee;
			return(make_pair(return1,""));
		}
		DEBINFMESSAGE("pair<string,int> MESSAGE::getUriProtocol(string header)",this<<"]["<<header<<"]["<<return1)
	}
	return (make_pair("",""));

}

string MESSAGE::getHeadCSeqMethod(void){
	DEBINFMESSAGE("string MESSAGE::getHeadCSeqMethod(void)",this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadCSeqMethod invalid")
	TIMEDEF
	SETNOW
	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedCseq){
		getHeadCSeq();
	}
	DEBINFMESSAGE("string MESSAGE::getHeadCSeqMethod(void)",this<<"]["<<cSeqMethod)
	PRINTDIFF("MESSAGE::getHeadCSeqMethod")
	return cSeqMethod;

}
int MESSAGE::getHeadCSeq(void){
	DEBINFMESSAGE("int MESSAGE::getHeadCSeq(void)", this)
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadCSeq invalid")
	TIMEDEF
	SETNOW
	if(messageStatus !=1){
		fillIn();
	}

	if(!parsedCseq){
		DEBY
		string sh = getGenericHeader("CSeq:");
		DEBY
		char tsmp[sh.length()+1];
		DEBY
		strcpy(tsmp, sh.c_str());
		DEBY
		char* token = strchr(tsmp, ' ');
		DEBY
		*token = '\0';
		DEBINFMESSAGE("int MESSAGE::getHeadCSeq(void)",tsmp)
		cSeq = atoi(tsmp);
		DEBY
		cSeqMethod = token+1;
		DEBY
		parsedCseq = true;
		DEBY
	}
	DEBINFMESSAGE("int MESSAGE::getHeadCSeq(void)", this<<"]["<<cSeq)
	PRINTDIFF("MESSAGE::getHeadCSeq")
	return cSeq;

}
int MESSAGE::fillCommand(void){
	DEBINFMESSAGE("int MESSAGE::fillCommand(void)",this<<"][")
	if (invalid == 1)
		DEBASSERT("MESSAGE::fillCommand invalid")


	NEWPTR2(message_char, char[strlen(original_message)+1],"message_char "<<strlen(original_message)+1)
	strcpy(message_char, original_message);

	bool fillSDP = false;
	char* saveptr1;
	char* tok = strtok_r(message_char, "\n",&saveptr1);
	while (tok != NULL){
		int ll = strlen(tok);
		char* trok = tok + ll - 1 ;
		strcpy(trok,"");
		DEBINFMESSAGE("MESSAGE::fillCommand line", tok)
			message_line.push_back( make_pair (tok,false) );
		tok = strtok_r(NULL, "\n",&saveptr1);
	}

	DEBINFMESSAGE("int MESSAGE::fillCommand(void)", this<<"]["<<message_line.size())
	return message_line.size();

}
bool MESSAGE::buildCommand(vector< pair<int,pair<int, pair<string,string> > > >& _command){
	//DEBINFMESSAGE("int MESSAGE::buildCommand(vector< pair<int, pair<string,string> > >&)",this<<"]["<<_command)

	/*
	 0%i%127.0.0.1:5061%service@127.0.0.1:5061
	 0 table id
	 i operation
	 127.0.0.1:5061 key
	 service@127.0.0.1:5061 data

	 dump all regitrations
	 0%q%all%
	 */

	vector< pair<char*, bool> >::iterator m_l;

	m_l = message_line.begin();
	m_l++;
	while ( m_l != message_line.end()){
		DEBOUT("MESSAGE::buildCommand",m_l->first)
		char temp[strlen(m_l->first) +1];
		strcpy(temp, m_l->first);

		char *saveptr1;
		char* tok = strtok_r(temp, "%",&saveptr1);
		int i = 0;
		string s[4];
		while (tok != NULL || i < 4){
			int ll = strlen(tok);
			char* trok = tok + ll;
			strcpy(trok,"");
			s[i] = tok;
			i++;
			tok = strtok_r(NULL, "%",&saveptr1);
		}
		//table
		int table = atoi(s[0].c_str());
		int k = 0;
		if(s[1].compare("i") == 0){
			k = 1;
		}
		else if (s[1].compare("d")  == 0){
			k = 2;
		}
		else if (s[1].compare("q") == 0){
			//query
			k = 3;
		}
		else if (s[1].compare("t")  == 0){
			k = 20;
		}

		_command.push_back(make_pair(table,make_pair(k, make_pair(s[2],s[3]))));
		DEBOUT("MESSAGE::buildCommand",k<<"]["<<s[1]<<"]["<<s[2])
		DEBOUT("MESSAGE::buildCommand",k<<"]["<<s[1]<<"]["<<s[2]<<"]["<<s[3])

		m_l++;
	}
	messageStatus = 1;



}



































//char* MESSAGE::getLine(int pos){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getLine invalid")
//
//    int i = fillIn();
//
//    if (pos >= i)
//        pos = i-1;
//
//    return(message_line[pos].first);
//}




//string MESSAGE::getProperty(string,string){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::dropHeader invalid")
//
//	if (_header.compare("Via:") == 0){
//		DEBASSERT("MESSAGE::getProperty invalid for via")
//		return;
//	}
//
//
//
//}


//void MESSAGE::setHeadSipRequest(string _content){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setHeadSipRequest invalid")
//
//	reqRep = 0;
//	flex_line[0] = _content;
//	headSipRequest.setContent(_content);
//}
//	void O_MESSAGE::setHeadSipReply(string _content){
//		if (invalid == 1)
//			DEBASSERT("MESSAGE::setHeadSipReply invalid")
//
//		reqRep = 0;
//		flex_line[0] = _content;
//		headSipReply.setContent(_content);
//	}
//	C_HeadSipReply &O_MESSAGE::getHeadSipReply(void){
//		if (invalid == 1)
//			DEBASSERT("MESSAGE::getHeadSipReply invalid")
//
//
//		if(reqRep == 0){
//			reqRep = getReqRepType();
//		}
//
//		if (reqRep == REPSUPP || reqRep == REPUNSUPP){
//			return headSipReply;
//		}
//		DEBASSERT("MESSAGE::getHeadSipReply illegal instruction");
//	}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// BASEMESSAGE
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
//BASEMESSAGE::BASEMESSAGE(string _incMessBuff, int _genEntity, SysTime _inc_ts, int _sock,
//                    sockaddr_inX _echoClntAddr):
//                    incMessBuff(_incMessBuff){
//
//	genEntity = _genEntity;
//	inc_ts = _inc_ts;
//	sock = _sock;
//	echoClntAddr = _echoClntAddr;
//
//	arrayFilled = false;
//
//	requestDirection = 0;
//
//	//others
//    id = 0; //Used in spin buffer
//    destEntity = 0;
//	key="";
//
//    totLines=0;
//    arrayFilled=false;
//
//    invalid = 0;
//
//	return;
//}
//BASEMESSAGE::BASEMESSAGE(string _incMessBuff, SysTime _inc_ts):
//						incMessBuff(_incMessBuff){
//	//TODO only for test
//	incMessBuff=_incMessBuff;
//
//	inc_ts = _inc_ts;
//
//	arrayFilled = false;
//
//	requestDirection = 0;
//
//    invalid = 0;
//
//	return;
//}
//BASEMESSAGE::BASEMESSAGE(BASEMESSAGE* _basemessage, int _genEntity, SysTime _creaTime){
//	/*
//	SysTime inTime;
//	GETTIME(inTime);
//	char bu[512];
//	sprintf(bu, "%x#%lld",this,inTime.tv.tv_sec*1000000+inTime.tv.tv_usec);
//	string key(bu);
//	*/
//    DEBSIP("BASEMESSAGE::BASEMESSAGE copy",_basemessage)
//    echoClntAddr = _basemessage->getAddress();
//    sock = _basemessage->getSock();
//
//    inc_ts = _creaTime;
//
//    //flex_line = _basemessage->flex_line;
//    incMessBuff = _basemessage->getIncBuffer();
//
//    arrayFilled = false;
//
//    genEntity = _genEntity;
//
//    requestDirection = _basemessage->getRequestDirection();
//
//    invalid = 0;
//
//    return;
//}
////BASEMESSAGE::BASEMESSAGE(const BASEMESSAGE& x){
////	DEBASSERT("BASEMESSAGE copy constructor");
////}
//
//void BASEMESSAGE::setValid(int _valid){
//	invalid = _valid;
//}
//vector<string> BASEMESSAGE::getLines(void){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getLines invalid")
//
//	return flex_line;
//}
//
//void BASEMESSAGE::fillLineArray(void){
//
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::fillLineArray invalid")
//
//    if (arrayFilled)
//        return;
//
//    Tuple s = brkin2(incMessBuff,"\n");
//    string t = s.Rvalue;
//    string strim = trimCR(s.Lvalue);
//    flex_line.push_back(strim);
//    while (t.compare("")!=0){
//
//    	s = brkin2(t,"\n");
//    	t = s.Rvalue;
//
//    	if (s.Lvalue.substr(0,1).compare(" ") == 0 || s.Lvalue.substr(0,1).compare("\t") == 0){
//    		string tt = flex_line.back();
//    		string ttt = tt + trimCR(s.Lvalue);
//    		flex_line.pop_back();
//            flex_line.push_back(ttt);
//    	} else{
//    		string strim = trimCR(s.Lvalue);
//    		TRYCATCH(flex_line.push_back(strim))
//    	}
//    }
//
//    arrayFilled = true;
//
//    return;
//}
//
//// *****************************************************************************************
//// *****************************************************************************************
//int BASEMESSAGE::getTotLines() {
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getTotLines invalid")
//
//    if (!arrayFilled){
//    	fillLineArray();
//    }
//
//    return(flex_line.size());
//}
//// *****************************************************************************************
//// *****************************************************************************************
//string &BASEMESSAGE::getLine(int pos){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getLine invalid")
//
//    // no copia REFERRENCE
//
//    if (!arrayFilled)
//    	fillLineArray();
//
//    int i = getTotLines();
//
//    if (pos >= i)
//        pos = i-1;
//
//    return(flex_line[pos]);
//}
//string &BASEMESSAGE::getIncBuffer(void){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getIncBuffer invalid")
//
//	return incMessBuff;
//}
//int BASEMESSAGE::getGenEntity(void){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getGenEntity invalid")
//
//	return genEntity;
//}
//void BASEMESSAGE::setGenEntity(int _genEntity){
//
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::setGenEntity invalid")
//
//	//MLF2
//	if (genEntity != 0){
//		DEBSIP("BASEMESSAGE::setGenEntity is not null cannot be changed anymore",genEntity << " new["<<_genEntity)
//		DEBASSERT("BASEMESSAGE::setGenEntity is not null cannot be changed anymore")
//	}
//
//	genEntity = _genEntity;
//}
//int BASEMESSAGE::getRequestDirection(void){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getRequestDirection invalid")
//
//	return requestDirection;
//}
//void BASEMESSAGE::setRequestDirection(int _genEntity){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::setRequestDirection invalid")
//
//	requestDirection = _genEntity;
//}
//int BASEMESSAGE::getDestEntity(void){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getDestEntity invalid")
//
//	return destEntity;
//}
//void BASEMESSAGE::setDestEntity(int _destEntity){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::setDestEntity invalid")
//
//	destEntity = _destEntity;
//}
//void BASEMESSAGE::setEndPoints(int _genEntity, int _destEntity){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::setEndPoints invalid")
//
//	destEntity = _destEntity;
//	genEntity = _genEntity;
//}
//
//string &BASEMESSAGE::getKey(void){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getKey invalid")
//
//	return key;
//}
//void BASEMESSAGE::setKey(string _key){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::setKey invalid")
//
//	key = _key;
//}
//SysTime BASEMESSAGE::getCreationTime(void){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getCreationTime invalid")
//
//	return inc_ts;
//}
//struct sockaddr_in BASEMESSAGE::getAddress(void){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getAddress invalid")
//
//	return echoClntAddr;
//}
//int BASEMESSAGE::getSock(void){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::getSock invalid")
//
//	return sock;
//}
//void BASEMESSAGE::removeHeader(int _pos){
//	if (invalid == 1)
//		DEBASSERT("BASEMESSAGE::removeHeader invalid")
//
//	flex_line[_pos] = "xxDxx";
//}
//// *****************************************************************************************
//// *****************************************************************************************
//// *****************************************************************************************
//// MESSAGE
//// *****************************************************************************************
//// *****************************************************************************************
//// *****************************************************************************************
//O_MESSAGE::O_MESSAGE(string _incMessBuff, int _genEntity, SysTime _inc_ts, int _sock,
//        struct sockaddr_in _echoClntAddr):
//	BASEMESSAGE(_incMessBuff, _genEntity, _inc_ts, _sock, _echoClntAddr),
//	headSipRequest(""),
//	headSipReply(""),
//	headMaxFwd(""),
//	headCallId(""),
//	headCSeq(""),
//        sourceHeadCallId(""){
//
//	reqRep = 0;
//
//	s_headVia_p = false;
//	headMaxFwd_p = false;
//	headContact_p = false;
//	headTo_p = false;
//	headFrom_p = false;
//	headCallId_p = false;
//	headCSeq_p = false;
//	headRoute_p = false;
//	headRoute_e = false;
//	sdpVector_p = false;
//	sdpSize = 0;
//
//	lock = false;
//
//	source=MainMessage;
//
//	isInternal = false;
//
//	modulus = -1;
//
//	typeOfInternal = TYPE_MESS; // Message or operation
//	typeOfOperation = TYPE_OP_NOOP; // Type of operation
//	orderOfOperation = ""; //Alarm id in case more alarms are triggered with the same message
//
//	type_trnsct = TYPE_TRNSCT;
//
//	//Pointers
//	headTo = NULL;
//	headFrom = NULL;
//	headContact = NULL;
//	headRoute = NULL;
//
//#ifdef MESSAGEUSAGE
//	inuse = (int) pthread_self();
//#endif
//
//}
////MESSAGE::MESSAGE(const MESSAGE& x){
////	DEBASSERT("MESSAGE copy constructor");
////}
//
//O_MESSAGE::~O_MESSAGE(){
//
//	while (!s_headVia.empty()){
//		DELPTR((C_HeadVia*)(s_headVia.top()), "Delete via")
//		s_headVia.pop();
//	}
//	if (headTo != NULL){
//		DELPTR(headTo, "C_HeadTo")
//	}
//	if (headFrom != NULL){
//		DELPTR(headFrom, "C_HeadFrom")
//	}
//	if (headContact != NULL){
//		DELPTR(headContact, "C_HeadContact")
//	}
//	if (headRoute != NULL){
//		DELPTR(headRoute, "C_HeadRoute")
//	}
//
//
//
//}
//O_MESSAGE::O_MESSAGE(MESSAGE* _message, int _genEntity, SysTime _creaTime):
//	BASEMESSAGE(_message, _genEntity, _creaTime),
//	headSipRequest(""),
//	headSipReply(""),
//	headMaxFwd(""),
//	//headTo(""),
//	headCallId(""),
//        sourceHeadCallId(_message->getHeadCallId().getContent()),
//	headCSeq(""){
//
//	//source = _message;
//        //Must be done esplicitely
//        source = MainMessage;
//        sourceModulus = _message->getModulus();
//	s_headVia_p = false;
//	headMaxFwd_p = false;
//	headContact_p = false;
//	headTo_p = false;
//	headFrom_p = false;
//	headCallId_p = false;
//	headCSeq_p = false;
//	headRoute_p = false;
//	sdpVector_p = false;
//	sdpSize = 0;
//	reqRep = 0;
//	//, headRoute_e;
//
//	lock = false;
//
//	type_trnsct = TYPE_TRNSCT;
//
//	modulus = -1;
//	typeOfInternal = TYPE_MESS; // Message or operation
//	typeOfOperation = TYPE_OP_NOOP; // Type of operation
//	orderOfOperation = ""; //Alarm id in case more alarms are triggered with the same message
//
//	//Pointers
//	headTo = NULL;
//	headFrom = NULL;
//	headContact = NULL;
//	headRoute = NULL;
//
//#ifdef MESSAGEUSAGE
//	inuse = (int) pthread_self();
//#endif
//
//
//	return;
//}
////ONLY FOR TEST
//#ifdef TESTING
//O_MESSAGE::O_MESSAGE(string _incMessBuff, SysTime _inc_ts):
//	BASEMESSAGE(_incMessBuff, inc_ts),
//	headSipRequest(""),
//	headSipReply(""),
//	headMaxFwd(""),
//	headContact(""),
//	headTo(""),
//	headFrom(""),
//	headCallId(""),
//	headCSeq(""),
//	headRoute(""){
//
//	assert(0);
//
//	reqRep = 0;
//}
//#endif
///*
// * get SDP position
// * XXX: blabla
// * <empty>
// * x=yxz
// *
// * position in the array of the <empty> line
// */
//vector<string>::iterator O_MESSAGE::getSDPposition(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getSDPposition invalid")
//
//
//	vector<string>::iterator theIterator;
//
//	for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
//    	if ((*theIterator).size() == 0) {
//    		return theIterator--;
//    	}
//    }
//    return theIterator;
//}
//void O_MESSAGE::dumpVector(void){
//    if (invalid == 1)
//        DEBASSERT("MESSAGE::dumpVector invalid")
//
//    vector<string>::iterator theIterator;
//    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
//    	DEBSIP("Message vector", (string)*theIterator)
//    }
//}
//int O_MESSAGE::getReqRepType(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getReqRepType invalid")
//
//
//	if (reqRep != 0) {
//		return reqRep;
//	}
//	if(flex_line[0].substr(0,3).compare("SIP")==0){
//		headSipReply.setContent(flex_line[0]);
//		reqRep = REPSUPP;
//	}
//	if(flex_line[0].substr(0,6).compare("INVITE")==0){
//		headSipRequest.setContent(flex_line[0]);
//		reqRep = REQSUPP;
//	}
//	else if(flex_line[0].substr(0,3).compare("ACK")==0){
//		headSipRequest.setContent(flex_line[0]);
//		reqRep = REQSUPP;
//	}
//	else if(flex_line[0].substr(0,3).compare("BYE")==0){
//		headSipRequest.setContent(flex_line[0]);
//		reqRep = REQSUPP;
//	}
//	else if(flex_line[0].substr(0,6).compare("CANCEL")==0){
//		headSipRequest.setContent(flex_line[0]);
//		reqRep = REQSUPP;
//	}
//	else if(flex_line[0].substr(0,8).compare("REGISTER")==0){
//		headSipRequest.setContent(flex_line[0]);
//		reqRep = REQUNSUPP;
//	}
//	return reqRep;
//}
//void O_MESSAGE::compileMessage(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::compileMessage invalid")
//
//
//	// first look for xxDxx and remove it
//	vector<string>::iterator theIterator;
//    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
//    	if ((*theIterator).substr(0,5).compare("xxDxx") == 0 ) {
//    		flex_line.erase(theIterator);
//    		theIterator--;
//    	}
//    }
//    incMessBuff = "";
//    if (getGenericHeader("Content-Type:").size() == 0)
//    	flex_line.insert(theIterator ,1 , "");
//
//    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
//
//    	incMessBuff = incMessBuff + (*theIterator) + "\r\n";
//
//    }
//
//}
//O_MESSAGE* O_MESSAGE::getSourceMessage(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getSourceMessage invalid")
//
//#ifdef DEBCODE
//	map<const MESSAGE*, MESSAGE*>::iterator p;
//	int i = ::getModulus((void*)source);
//	pthread_mutex_lock(&messTableMtx[i]);
//	p = globalMessTable[i].find(source);
//	if (p ==globalMessTable[i].end()) {
//		DEBASSERT("MESSAGE::getSourceMessage deleted source:[" <<source<<"] message ["<<this<<"]")
//	}
//	pthread_mutex_unlock(&messTableMtx[i]);
//#endif
//
//
//	return source;
//}
//void O_MESSAGE::setSourceMessage(O_MESSAGE* _source){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setSourceMessage invalid")
//
//#ifdef DEBCODE
//	map<const MESSAGE*, MESSAGE*>::iterator p;
//	int i = ::getModulus((void*)_source);
//	pthread_mutex_lock(&messTableMtx[i]);
//	p = globalMessTable[i].find(_source);
//	if (p ==globalMessTable[i].end()) {
//		DEBASSERT("MESSAGE::setSourceMessage deleted source:[" <<_source<<"] message ["<<this<<"]")
//	}
//	pthread_mutex_unlock(&messTableMtx[i]);
//#endif
//
//    sourceHeadCallId = _source->getHeadCallId();
//    sourceModulus = _source->getModulus();
//
//	source = _source;
//}
//
//
///*
// * Request
// */
//C_HeadSipRequest &O_MESSAGE::getHeadSipRequest(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getHeadSipRequest invalid")
//
//	if(reqRep == 0){
//		reqRep = getReqRepType();
//	}
//
//	if (reqRep == REQSUPP || reqRep == REQUNSUPP){
//		return headSipRequest;
//	}
//    DEBASSERT("O_MESSAGE::getHeadSipRequest illegal instruction");
//}
//void O_MESSAGE::setHeadSipRequest(string _content){
//	if (invalid == 1)
//		DEBASSERT("O_MESSAGE::setHeadSipRequest invalid")
//
//	reqRep = 0;
//	flex_line[0] = _content;
//	headSipRequest.setContent(_content);
//}
//void O_MESSAGE::setHeadSipReply(string _content){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setHeadSipReply invalid")
//
//	reqRep = 0;
//	flex_line[0] = _content;
//	headSipReply.setContent(_content);
//}
//C_HeadSipReply &O_MESSAGE::getHeadSipReply(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getHeadSipReply invalid")
//
//
//	if(reqRep == 0){
//		reqRep = getReqRepType();
//	}
//
//	if (reqRep == REPSUPP || reqRep == REPUNSUPP){
//		return headSipReply;
//	}
//	DEBASSERT("MESSAGE::getHeadSipReply illegal instruction");
//}
///*
// * Via
// */
//stack<C_HeadVia*> &O_MESSAGE::getSTKHeadVia(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getSTKHeadVia invalid")
//
//
//	if(s_headVia_p){
//		return s_headVia;
//	}
//
//	size_t i;
//
//	for( i =   flex_line.size() - 1 ; i > 0 ; i --){
//		if(flex_line[i].substr(0,4).compare("Via:") == 0){
//
//			C_HeadVia* s;
//			NEWPTR2(s, C_HeadVia(flex_line[i].substr(5)), "New via")
//			s_headVia.push(s);
//		}
//	}
//	s_headVia_p = true;
//	return s_headVia;
//}
//void O_MESSAGE::popSTKHeadVia(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::popSTKHeadVia invalid")
//
//
//	DEBASSERT("popSTKHeadVia")
////	if(s_headVia_p){
////		getSTKHeadVia();
////	}
////
////	unsigned int i;
////	unsigned int j = 1;
////	bool topout = false;
////	for(i = 1; i < flex_line.size(); i ++){
////		if(flex_line[i].substr(0,4).compare("Via:") == 0){
////			if (!topout){
////				flex_line[i] = "xxDxx";
////				topout = true;
////			} else {
////				C_HeadVia* s = new C_HeadVia(flex_line[i]);
////				s_headVia.push(s);
////			}
////		}
////	}
////	s_headVia_p = true;
//}
////
//void O_MESSAGE::purgeSTKHeadVia(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::purgeSTKHeadVia invalid")
//
//
//	size_t i;
//
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,4).compare("Via:") == 0){
//			removeHeader(i);
//		}
//	}
//
//	if(!s_headVia_p){
//		// not initialized
//		return;
//	}
//	//else clear the vector
//	else {
//		while(!s_headVia.empty()){
//			//delete s_headVia.top();
//			s_headVia.pop();
//			DELPTR((C_HeadVia*)s_headVia.top(), "(C_HeadVia*)s_headVia.top()");
//		}
////		stack<C_HeadVia*> tmp;
////		s_headVia = tmp;
//		s_headVia_p = false;
//	}
//}
//void O_MESSAGE::pushHeadVia(string _content){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::pushHeadVia invalid")
//
//
//	NEWPTR(C_HeadVia*, s, C_HeadVia(_content),"New via")
//	s_headVia.push(s);
//	// first search Via and insert before
//	// if no via, then search SDP
//	bool found = false;
//	vector<string>::iterator theIterator;
//    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
//    	if ((*theIterator).substr(0,4).compare("Via:") == 0 ) {
//			found = true;
//			break;
//    	}
//    }
//	if (found){
//		string tmp = "Via: " + _content;
//		flex_line.insert(theIterator ,1 , _content);
//	}
//	else {
//		theIterator = getSDPposition();
//		DEBSIP("string tmp", _content)
//		flex_line.insert(theIterator ,1 , "Via: " + _content);
//	}
//}
//void O_MESSAGE::purgeSDP(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::purgeSDP invalid")
//
//
//	DEBSIP("MESSAGE::purgeSDP","" )
//
//	//setGenericHeader("Content-Length:","0");
//	dropHeader("Content-Length:");
//	dropHeader("Content-Type:");
//
//	vector<string>::iterator theIterator;
//	vector<string>::iterator theIteratorEnd;
//
//	theIterator = getSDPposition();
//	theIteratorEnd = flex_line.end();
//	sdpSize = 0;
//	if ( sdpVector_p ) {
//		sdpVector.clear();
//	} else {
//		sdpVector_p = true;
//	}
//
//	if (theIterator == flex_line.end()){
//		//nothing to erase
//		return;
//	}
//
//	flex_line.erase(theIterator, theIteratorEnd);
//}
//vector<string> O_MESSAGE::getSDP(void) {
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getSDP invalid")
//
//
//
//	if ( sdpVector_p )  {
//		return sdpVector;
//	}
//	else {
//		vector<string>::iterator theIterator;
//
//		theIterator = getSDPposition();
//
//		if (theIterator != flex_line.end()){
//			theIterator ++;
//		}
//		sdpSize = 0;
//		while(theIterator != flex_line.end()){
//			sdpSize = sdpSize + (*theIterator).length() + 2;
//			sdpVector.push_back((*theIterator));
//			theIterator++;
//		}
//		sdpVector_p = true;
//		return sdpVector;
//	}
//}
//int O_MESSAGE::getSDPSize(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getSDPSize invalid")
//
//
//	if ( sdpVector_p )  {
//		return sdpSize;
//	}
//	else {
//		getSDP();
//		return sdpSize;
//	}
//
//}
//void O_MESSAGE::importSDP(vector<string> _sdp){
//    if (invalid == 1)
//        DEBASSERT("MESSAGE::importSDP invalid")
//
//
//    vector<string>::iterator theIterator;
//
//    sdpVector = _sdp;
//
//    theIterator = _sdp.begin();
//    sdpSize = 0;
//    sdpVector_p = true;
//    if (theIterator!= _sdp.end()){
//        flex_line.push_back("Content-Type: application/sdp");
//        flex_line.push_back("Content-Length: 0");
//        flex_line.push_back("");
//    }
//    while (theIterator != _sdp.end()){
//        sdpSize = sdpSize + (*theIterator).length() + 2;
//        flex_line.push_back((*theIterator));
//        theIterator++;
//    }
//    char aaa[10];
//    sprintf(aaa,"%d", sdpSize);
//    setGenericHeader("Content-Length:", aaa );
//
//}
//
///*
// * MaxFwd
// */
//S_HeadMaxFwd& O_MESSAGE::getHeadMaxFwd(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getHeadMaxFwd invalid")
//
//
//	if(headMaxFwd_p){
//		return headMaxFwd;
//	}
//
//	size_t i;
//
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,13).compare("Max-Forwards:")==0){
//			headMaxFwd.setContent(flex_line[i]);
//			break;
//		}
//	}
//	headMaxFwd_p = true;
//		return headMaxFwd;
//}
//C_HeadTo* O_MESSAGE::getHeadTo(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getHeadTo invalid")
//
//
//	if (headTo == 0x0){
//		NEWPTR2(headTo, C_HeadTo(""), "C_HeadTo")
//	}
//
//	if(headTo_p){
//		return headTo;
//	}
//
//	size_t i;
//
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,3).compare("To:")==0){
//			headTo->setContent(flex_line[i].substr(4));
//			break;
//		}
//	}
//	headTo_p = true;
//	return headTo;
//}
///*
// * From
// */
//C_HeadFrom* O_MESSAGE::getHeadFrom(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getHeadFrom invalid")
//
//
//	if (headFrom == 0x0){
//		NEWPTR2(headFrom, C_HeadFrom(""), "C_HeadFrom")
//	}
//
//
//	if(headFrom_p){
//		return headFrom;
//	}
//
//	size_t i;
//
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,5).compare("From:")==0){
//			headFrom->setContent(flex_line[i].substr(6));
//			break;
//		}
//	}
//	headFrom_p = true;
//	return headFrom;
//}
//void O_MESSAGE::replaceHeadFrom(string _content){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::replaceHeadFrom invalid")
//
//
//	headFrom_p = false;
//
//	if (headFrom == NULL){
//		NEWPTR2(headFrom, C_HeadFrom(""), "C_HeadFrom")
//	}
//
//	headFrom->setContent(_content);
//
//	// replace in flex_line
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,5).compare("From:")==0){
//			flex_line[i] = "From: " + _content;
//			found = true;
//			break;
//		}
//	}
//	if (!found) {
//		DEBSIP("MESSAGE::replaceHeadFrom from header is missing","")
//	}
//}
//void O_MESSAGE::replaceHeadTo(string _content){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::replaceHeadTo invalid")
//
//
//	headTo_p = false;
//
//	if (headTo == NULL){
//		NEWPTR2(headTo, C_HeadTo(""), "C_HeadTo")
//	}
//
//	headTo->setContent(_content);
//
//	// replace in flex_line
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,3).compare("To:")==0){
//			flex_line[i] = "To: " + _content;
//			found = true;
//			break;
//		}
//	}
//	if (!found) {
//		DEBSIP("MESSAGE::replaceHeadTo from header is missing","")
//	}
//}
//
///*
// * Call Id
// */
//C_HeadCallId &O_MESSAGE::getHeadCallId(void){
//    if (invalid == 1)
//        DEBASSERT("MESSAGE::getHeadCallId invalid")
//
//    if(headCallId_p){
//        DEBSIP("CALL ID already parsed", headCallId.getContent())
//        return headCallId;
//    }
//
//    size_t i;
//
//    for(i = 1; i < flex_line.size(); i ++){
//        if(flex_line[i].substr(0,8).compare("Call-ID:") == 0){
//            headCallId.setContent(flex_line[i].substr(9));
//            break;
//        }
//    }
//    headCallId_p = true;
//    return headCallId;
//}
///*
// * CSeq
// */
//C_HeadCSeq &O_MESSAGE::getHeadCSeq(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getHeadCSeq invalid")
//
//
//	if(headCSeq_p){
//		return headCSeq;
//	}
//
//	size_t i;
//
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,5).compare("CSeq:")==0){
//			headCSeq.setContent(flex_line[i].substr(6));
//			break;
//		}
//	}
//	headCSeq_p = true;
//	return headCSeq;
//}
//void O_MESSAGE::replaceHeadCSeq(string _content){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::replaceHeadCSeq invalid")
//
//
//	headCSeq_p = false;
//	headCSeq.setContent(_content);
//
//	// replace in flex_line
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,5).compare("CSeq:")==0){
//			flex_line[i] = "CSeq: " + _content;
//			found = true;
//			break;
//		}
//	}
//	if (!found) {
//		DEBASSERT("MESSAGE::replaceHeadCSeq from header is missing")
//	}
//}
//void O_MESSAGE::replaceHeadCSeq(int _cseq, string _method){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::replaceHeadCSeq invalid")
//
//
//	char tt[128];
//	sprintf(tt, "%d %s", _cseq, _method.c_str());
//	headCSeq_p = false;
//	headCSeq.setContent(tt);
//
//	// replace in flex_line
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,5).compare("CSeq:")==0){
//			flex_line[i] = "CSeq: " + (string)(tt);
//			found = true;
//			break;
//		}
//	}
//	if (!found) {
//		DEBASSERT("MESSAGE::replaceHeadCSeq from header is missing")
//	}
//}
//
///*
// * Route
// */
//C_HeadRoute* O_MESSAGE::getHeadRoute(void) throw (HeaderException){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getHeadRoute invalid")
//
//
//	if (headRoute == 0x0){
//		NEWPTR2(headRoute, C_HeadRoute(""),"C_HeadRoute")
//	}
//	if(headRoute_e){
//		DEBSIP("MESSAGE::getHeadRoute","No Route header")
//		DELPTR(headRoute,"C_HeadRoute")
//		headRoute = NULL;
//		throw HeaderException("No Route header");
//	}
//	if(headRoute_p){
//		return headRoute;
//	}
//
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,6).compare("Route:")==0){
//			found = true;
//			headRoute->setContent(flex_line[i]);
//			break;
//		}
//	}
//	headRoute_p = true;
//	if (found == true){
//		return headRoute;
//	}
//	else {
//		headRoute_e = true;
//		DELPTR(headRoute,"C_HeadRoute")
//		headRoute = NULL;
//		DEBSIP("MESSAGE::getHeadRoute","No Route header")
//		throw HeaderException("No Route header");
//	}
//}
//void O_MESSAGE::removeHeadRoute(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::removeHeadRoute invalid")
//
//	headRoute_p = false;
//	size_t i;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,6).compare("Route:")==0){
//			removeHeader(i);
//			headRoute = NULL;
//			DELPTR(headRoute,"C_HeadRoute")
//			break;
//		}
//	}
//	return;
//}
///*
// * Contact
// */
//C_HeadContact* O_MESSAGE::getHeadContact(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getHeadContact invalid")
//
//
//	if (headContact == 0x0){
//		NEWPTR2(headContact, C_HeadContact(""),"C_HeadContact")
//	}
//
//	if(headContact_p){
//		return headContact;
//	}
//
//	size_t i;
//
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,8).compare("Contact:")==0){
//			headContact->setContent(flex_line[i].substr(9));
//			break;
//		}
//	}
//	headContact_p = true;
//		return headContact;
//}
//
//void O_MESSAGE::replaceHeadContact(string _content){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::replaceHeadContact invalid")
//
//
//	if (headContact == 0x0){
//		NEWPTR2(headContact, C_HeadContact(""),"C_HeadContact")
//	}
//
//	headContact_p = false;
//	headContact->setContent(_content);
//
//	// replace in flex_line
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,8).compare("Contact:")==0){
//			flex_line[i] = "Contact: " + _content;
//			found = true;
//			break;
//		}
//	}
//	if (!found) {
//		DEBSIP("MESSAGE::replaceHeadContact from header is missing","")
//	}
//}
//void O_MESSAGE::removeMaxForwards(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::removeMaxForwards invalid")
//
//
//	headMaxFwd_p = false;
//	headMaxFwd.setContent("");
//
//	// replace in flex_line
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,13).compare("Max-Forwards:")==0){
//			removeHeader(i);
//			found = true;
//			break;
//		}
//	}
//	if (!found) {
//		DEBSIP("MESSAGE::removeMaxForwards, MaxForwards is missing","")
//	}
//
//}
//void O_MESSAGE::increaseMaxForwards(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::increaseMaxForwards invalid")
//	DEBASSERT("MESSAGE::increaseMaxForwards for what?")
//	return ;
//}
//
//void O_MESSAGE::dropHeader(string _header){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::dropHeader invalid")
//
//
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,_header.size()).compare(_header)==0){
//			removeHeader(i);
//			found = true;
//			break;
//		}
//	}
//	if (!found) {
//		DEBSIP("MESSAGE::removeHeader not found",_header)
//	}
//}
//void O_MESSAGE::setGenericHeader(string _header, string _content){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setGenericHeader invalid")
//
//
//	DEBSIP("MESSAGE::setGenericHeader", _header + " " + _content)
//	if (_header.compare("To:") == 0){
//		DEBASSERT("DON'T USE")
//	}
//	if (_header.compare("From:") == 0){
//		DEBASSERT("DON'T USE")
//	}
//	if (_header.compare("Contact:") == 0){
//		DEBASSERT("DON'T USE")
//	}
//	if (_header.compare("Route:") == 0){
//		DEBASSERT("DON'T USE")
//	}
//
//
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,_header.size()).compare(_header)==0){
//			flex_line[i] = _header + " " + _content;
//			found = true;
//			break;
//		}
//	}
//	if (!found) {
//		DEBSIP("MESSAGE::setGenericHeader not found",_header)
//	}
//}
//string O_MESSAGE::getGenericHeader(string _header){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getGenericHeader invalid")
//
//
//	DEBSIP("MESSAGE::getGenericHeader", _header)
//	size_t i;
//	bool found = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,_header.size()).compare(_header)==0){
//			Tuple s = brkin2(flex_line[i], " ");
//			found = true;
//			DEBSIP("found header",_header << "***"<< s.Rvalue)
//			return s.Rvalue;
//		}
//	}
//	if (!found) {
//		DEBSIP("MESSAGE::setGenericHeader not found",_header)
//	}
//	return "";
//
//}
//
//
//void O_MESSAGE::addGenericHeader(string _header, string _content){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::addGenericHeader invalid")
//
//
//	DEBSIP("MESSAGE::addGenericHeader",_header << " " << _content)
//
//	flex_line.push_back(_header + " " + _content);
//
//}
//
//void O_MESSAGE::setFireTime(lli _firetime){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setFireTime invalid")
//
//
//	fireTime = _firetime;
//}
////
////void MESSAGE::setFireTime_x(SysTime _systime){
////
////	fireTime = _systime;
////}
//lli O_MESSAGE::getFireTime(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getFireTime invalid")
//
//
//	return fireTime;
//}
//string O_MESSAGE::getTransactionExtendedCID(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getTransactionExtendedCID invalid")
//
//
//	return getHeadCallId().getNormCallId() + getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");
//
//}
//string O_MESSAGE::getDialogExtendedCID(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getDialogExtendedCID invalid")
//
//	//Call id and FromTag
//	DEBSIP("MESSAGE::getDialogExtendedCID(void) fromtag part", getHeadFrom()->getC_AttUriParms().getTuples().findRvalue("tag"))
//	return getHeadCallId().getNormCallId() + getHeadFrom()->getC_AttUriParms().getTuples().findRvalue("tag");
//}
//void O_MESSAGE::setLock(CALL_OSET* _call_oset){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setLock invalid")
//
//	_call_oset->insertLockedMessage(this);
//	lock = true;
//}
//bool O_MESSAGE::getLock(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getLock invalid")
//
//	return lock;
//}
//void O_MESSAGE::unSetLock(CALL_OSET* _call_oset){
//	if (invalid == 1){
//		DEBASSERT("MESSAGE::unSetLock invalid")}
//
//	DEBSIP("MESSAGE::unSetLock ", this)
//	_call_oset->removeLockedMessage(this);
//	lock=false;
//}
//
//string O_MESSAGE::getOrderOfOperation(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getOrderOfOperation invalid")
//
//	return orderOfOperation;
//}
//void O_MESSAGE::setOrderOfOperation(string _s){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setOrderOfOperation invalid")
//
//	orderOfOperation = _s;
//}
//int O_MESSAGE::getTypeOfInternal(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getTypeOfInternal invalid")
//
//	return typeOfInternal;
//}
//void O_MESSAGE::setTypeOfInternal(int _i){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setTypeOfInternal invalid")
//
//	if( typeOfInternal == TYPE_OP && _i != TYPE_OP){
//		DEBSIP("MESSAGE::setTypeOfInternal changing from TYPE_OP",this)
//		DEBASSERT("MESSAGE::setTypeOfInternal changing from TYPE_OP")
//	}
//	typeOfInternal = _i;
//}
//int O_MESSAGE::getType_trnsct(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getType_trnsct invalid")
//
//	return type_trnsct;
//}
//void O_MESSAGE::setType_trnsct(int _t){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setType_trnsct invalid")
//
//	type_trnsct = _t;
//}
//int O_MESSAGE::getTypeOfOperation(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getTypeOfOperation invalid")
//
//	return typeOfOperation;
//}
//void O_MESSAGE::setTypeOfOperation(int _t){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::setTypeOfOperation invalid")
//
//	typeOfOperation = _t;
//}
//
//int O_MESSAGE::getModulus(void){
//	if (invalid == 1)
//		DEBASSERT("MESSAGE::getModulus invalid")
//
//	if (modulus != -1){
//		return modulus;
//	}
//	//is calculated in two ways:
//	// if first 5 chars of call id is "CoMap" then
//	// the modulus is the number after it (two digits)
//	// otherwise is calculated
//
//	string s = getHeadCallId().getContent();
//	if (s.substr(0,5).compare("CoMap") == 0){
//		modulus = atoi(s.substr(5,COMAPS_DIG).c_str());
//	}else {
//		char x[64];
//		int k = 64<s.length() ? 64 : s.length();
//		sprintf(x,"%s", s.substr(0,k).c_str());
//		long long int tot=0;
//		for (int i = 0; i < k ; i++){
//			tot =  (long long int) atol(s.substr(i,3).c_str()) + tot;
//		}
//		DEBOUT("MODULUS tot", tot)
//		modulus = tot%COMAPS;
//	}
//	DEBDEV("Modulus found", modulus<<"]["<< s)
//	return modulus;
//
//
//
//}
//C_HeadCallId& O_MESSAGE::getSourceHeadCallId(void){
//    return sourceHeadCallId;
//}
//int O_MESSAGE::getSourceModulus(void){
//    return sourceModulus;
//}
//void O_MESSAGE::setSourceHeadCallId(string _content){
//    sourceHeadCallId.setContent(_content);
//}
//void O_MESSAGE::setSourceModulus(int _mod){
//    sourceModulus = _mod;
//}
