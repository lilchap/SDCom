#pragma once
#include <cstdlib>
#include <thread>

#include "SerialClass.h"

/*
SDCom - Serial Device Communication

registerfunc(uintptr_t func) - Adds a function to the calltable
registerfunc(uintptr_t* funclist, size_t size) - Adds a lot of functions to the calltable

*/

#define zeromem(x) memset(x, 0, sizeof(x))
#define zeroflag(x) x = 0;
#define endmsg() \
	zeroflag(msgflag); \
	zeroflag(msgBytesIdx); \
	runMsg(sdcom, msgBuffer); \
	zeromem(msgBuffer);

typedef void(*callfunc_t)(void*, char*);

class SDCom {
public:

	void registerfunc(callfunc_t func);
	void registerfunc(callfunc_t* funclist, size_t size);
	void serialStart(const char* portname);

	// Expose it anyway just in case
	Serial* serialConnection;
	DWORD baudRate;

	SDCom(DWORD baudRate, size_t _message_size, size_t _calltable_size);
	~SDCom();

	callfunc_t* calltable;
	size_t calltable_size;
	size_t calltable_idx;
	size_t message_size;
};

void SDComMainThread(SDCom* sdcom);