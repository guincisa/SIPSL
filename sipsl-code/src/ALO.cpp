#include <stdio.h>


#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef ALO_H
#include "ALO.h"
#endif

ALO::ALO(ENGINE* _sl_cc):ENGINE(){

	sl_cc = _sl_cc;

}
void ALO::parse(MESSAGE* _message) {

	DEBOUT("ALO",_message->getHeadCallId().getContent())

	// do business logic...
	// create b2b invite related message & so on...
	//TODO clean this
	char bu[512];
	SysTime inTime;
	GETTIME(inTime);
	MESSAGE* message;
	message = new MESSAGE(_message->getIncBuffer().c_str(), SODE_SMSVPOINT, inTime, _message->getSock(), _message->getSocket());
	sprintf(bu, "%x#%lld",message,inTime.tv.tv_sec*1000000+inTime.tv.tv_usec);
	string key(bu);
	message->setKey(key);
	pthread_mutex_lock(&messTableMtx);
	globalMessTable.insert(pair<string, MESSAGE*>(key, message));
	pthread_mutex_unlock(&messTableMtx);

	DEBOUT("ALO","1")
	// TODO
	int tl = message->getTotLines();
	DEBOUT("ALO::parse tot lines",tl)
	sl_cc->p_w(message);
	DEBOUT("ALO","2")
}
