//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer 
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
#define CALLMNT

//**********************************************************************************
//**********************************************************************************
// State machine
//**********************************************************************************
//**********************************************************************************
class SL_SM {
    protected:
        
        int State;
    public:
        SL_SM();
        int getState (void);

        virtual MESSAGE * event(MESSAGE&);
};
class SL_SM_CL : public SL_SM {
    public:
    SL_SM_CL();
	// array of pointers to messages
    MESSAGE ** event(MESSAGE&);
    int dummy;
};
class SL_SM_SV : public SL_SM {
    public:
    SL_SM_SV();
	// array of pointers to messages
    MESSAGE ** event(MESSAGE&);
    int dummy;
};
//**********************************************************************************
//**********************************************************************************
// ALO Application Layer
//**********************************************************************************
//**********************************************************************************
class ALO : public ENGINE {
    public:
        int placeholder;
};
//**********************************************************************************
//**********************************************************************************
// Call Object
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
class SL_CO {
    protected:
        //Activity entry
        SL_SM_SV *sm_sv;
        ALO * alo;

        string CallIdX;

        pthread_mutex_t mutex;

        ENGINE * sl_cc; //back calls

		// incoming invite used to derive
		// all the rest
		// hard copy
		MESSAGE mastermessage;

    public:
        SL_CO(ENGINE*, MESSAGE);
        SL_CO();
        ~SL_CO();
        void call(MESSAGE);
};
//**********************************************************************************
//**********************************************************************************
typedef map<string, SL_CO*> COMAPTYPE;
typedef COMAPTYPE::value_type CMTvalue;
//**********************************************************************************
//**********************************************************************************
// CALL ID YY i, CALL ID XX
typedef map<string, string> SVMAPTYPE;
typedef SVMAPTYPE::value_type SMTvalue;
//**********************************************************************************
//**********************************************************************************
class COMAP {
    private:
        // we choose the mutex access to COMAP
        // but with more than one lock
        // the choice of the lock is determined by (callidxx MOD MAXCOMAPLOCKS)
        pthread_mutex_t mutex[MAXCOMAPLOCKS];
        COMAPTYPE OnCall;
    public:
        COMAP();
        //~COMAP() 
        SL_CO * find(string);
        string store(SL_CO *, string);
        void remove(string);
};

	
