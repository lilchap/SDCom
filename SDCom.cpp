#include "SDCom.h"
#include <iostream>

void SDCom::registerfunc(callfunc_t func) {
	if (calltable_idx == calltable_size) {
		return;
	}

	calltable[calltable_idx] = func;
	calltable_idx++;

	return;
}

void SDCom::registerfunc(callfunc_t* funclist, size_t size) {
	if (calltable_idx + size > calltable_size) {
		return;
	}

	for (size_t i = 0; i < size; i++) {
		calltable[calltable_idx] = funclist[i];
		calltable_idx++;
	}
}

void SDCom::serialStart(const char* portname) {
	serialConnection = new Serial(portname, baudRate);

	std::thread t(SDComMainThread, this);
	t.detach();
}

SDCom::SDCom(DWORD _baudRate, size_t _message_size, size_t _calltable_size) {
	baudRate = _baudRate;
	calltable_size = _calltable_size;
	message_size = _message_size;
	calltable_idx = 0;
	calltable = new callfunc_t[calltable_size];
	ZeroMemory(calltable, calltable_size * sizeof(size_t));
}

SDCom::~SDCom() {
	delete serialConnection;
}

void runMsg(SDCom* sdcom, char* msgBuffer) {
	DWORD targetfuncid = *(DWORD*)(msgBuffer);
	if (targetfuncid >= sdcom->calltable_size) {
		return;
	}

	callfunc_t callfunc = sdcom->calltable[targetfuncid];
	if (callfunc) {
		callfunc(sdcom, msgBuffer);
	}
}

void SDComMainThread(SDCom* sdcom) {
	// Allocate serialData buffer
	char* serialData = new char[sdcom->message_size];
	// Preserve current msg
	char* msgBuffer = new char[sdcom->message_size];
	int msgBytesIdx = 0;
	BYTE msgflag = 0;

	// Clear just in case
	zeromem(serialData);
	zeromem(msgBuffer);

	while (sdcom->serialConnection->IsConnected()) {
		int bytesRead = sdcom->serialConnection->ReadData(serialData, sdcom->message_size);
		// Read nothing; return
		if (!bytesRead)
			continue;

		// Recieving new message
		if (!msgflag) {
			msgflag = 1;
			memcpy(msgBuffer, serialData, bytesRead);
			msgBytesIdx += bytesRead;

			// Already recieved full message
			if (bytesRead == sdcom->message_size)
				endmsg();
		}
		// Continue reading message
		else {
			memcpy(msgBuffer + msgBytesIdx, serialData, bytesRead);
			msgBytesIdx += bytesRead;

			// Message complete
			if (msgBytesIdx == sdcom->message_size)
				endmsg();
		}

		// Reset serialData
		zeromem(serialData);

		// Stability for now
		Sleep(10);
	}

	delete msgBuffer;
	delete serialData;
}