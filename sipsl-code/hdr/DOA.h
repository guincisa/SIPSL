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
#ifndef DOA_H
#define DOA_H
class DOA {

    public:

	void init(void);
	void doa(int);
	DOA(SL_CC* sl_cc, __time_t sec, long int nsec);


    private:

    //pthread_mutex_t mutex;

	timespec sleep_time;
	SL_CC* sl_cc;

    ThreadWrapper *listenerThread[DOATH];

};
#endif
