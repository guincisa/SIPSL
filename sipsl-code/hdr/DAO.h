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

/***********************************************************
  DAO.h
  provisioning object
************************************************************/
#ifndef DAO_H
#define DAO_H
class TRNSPRT;
class SIPENGINE;
class SL_CC;


//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//TODO singleton
class DAO : public ENGINE {

    public:

        void parse(void *, int);

        DAO(int,int,string,SL_CC*, SIPENGINE*, TRNSPRT*);

        string getData(int,string);
        void putData(int,pair<string,string>);

    private:
        pthread_mutex_t mutex;

        //table id, key, data

        //
        map<string,string> datatable[TABLES];

        SL_CC* sl_cc;
        SIPENGINE* sipengine;
        TRNSPRT* trnsprt;

};

class COMMAND {
	public:
		//table id, operation, key, data
		pair<int,pair<int, pair<string,string> > > topTuple(void);
		bool popCommand(void);
		vector< pair<int, pair<int, pair<string,string> > > > commandTable;
};
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************

#endif
