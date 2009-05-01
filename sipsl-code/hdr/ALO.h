#define ALO_H

#ifndef ENGINE_H
#include "ENGINE.h"
#endif

class ALO : public ENGINE {
	public:
		int dummy;

        void parse(MESSAGE *);

};
