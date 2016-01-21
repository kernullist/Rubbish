#pragma once


#define KBDRDR_DRIVER_NAME		L"KbdRdr"


#pragma pack (push, 1)

typedef struct _KEYEVENT_DATA
{
	USHORT		makeCode;
	USHORT		flags;

} KEYEVENT_DATA, *PKEYEVENT_DATA;

#pragma pack (pop)