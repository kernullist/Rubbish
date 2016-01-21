#pragma once

extern "C"
{
#include <ntddk.h>
#include <wdm.h>
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _FN_
#define _FN_	__FUNCTION__
#endif

#ifndef _LN_
#define _LN_	__LINE__
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
UnloadRoutine(
	IN	PDRIVER_OBJECT	pDriverObject
	);

void
NTAPI
OnUserConnect(
	IN	HANDLE			processId,
	IN	PVOID			pContext
	);

void
NTAPI
OnUserDisconnect(
	IN	HANDLE			processId,
	IN	PVOID			pContext
	);
