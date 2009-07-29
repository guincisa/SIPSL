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
	MESSAGE* inviteB = new MESSAGE(_message, SODE_SMSVPOINT);
	// TODO
	sl_cc->p_w(inviteB);

}
