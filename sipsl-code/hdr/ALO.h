#define ALO_H
#ifndef ENGINE_H
#include "ENGINE.h"
#endif


class ALO : public ENGINE {
	public:

		//CALL_OSET needed for back reference to umbrella object
		ALO(ENGINE*);
		int dummy;

        void parse(MESSAGE*);
	private:
        ENGINE* sl_cc;

};
