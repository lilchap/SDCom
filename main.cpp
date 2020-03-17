// main.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "SDCom.h"

#pragma pack(1)
struct testfunctiondata {
	char _[4];
	DWORD int1;
	DWORD int2;
};

void testfunction(void* sdcom, char* msgBuffer) {
	testfunctiondata* data = (testfunctiondata*)msgBuffer;
	DWORD result = data->int1 * data->int2;

	printf("Sending result of %d multiplied by %d to arduino...\n", data->int1, data->int2);

	char* buffer = new char[4];
	memcpy(buffer, &result, 4);

	((SDCom*)(sdcom))->serialConnection->WriteData(buffer, 4);
	delete[] buffer;
}

void theotherfunction(void* sdcom, char* msgBuffer) {
	printf("Message from arduino: %d\n", *(DWORD*)(msgBuffer + 4));
}

int main()
{
	printf("SerialApp - All rights reserved :)\n");
	std::cout << "--------------------" << std::endl;

	SDCom sdcom(CBR_256000, 32, 2);
	sdcom.registerfunc(testfunction);
	sdcom.registerfunc(theotherfunction);

	sdcom.serialStart("\\\\.\\COM3");

	std::getchar();

    return 0;
}

