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

// SIPUTIL is a static class to be used for creating messages from other messages

#ifndef SIPUTIL_H
#define SIPUTIL_H

class SIPUTIL {

	private:
		static int i;

	public:
		// Generate a reply to A from a Reply
		void genASideReplyFromBReply(MESSAGE* gtor, MESSAGE* _gtor, MESSAGE* gted, SUDP* sudp);
		void genASideReplyFromRequest(MESSAGE* gtor, MESSAGE* getd);

		// Generate B Request from A Request
		void genBInvitefromAInvite(MESSAGE* gtor, MESSAGE* getd, SUDP* sudp, string callidy);

		void genTryFromInvite(MESSAGE* invite, MESSAGE* etry);
		void genQuickReplyFromInvite(MESSAGE* _invite, MESSAGE* _qrep, string _header);


};
#endif
