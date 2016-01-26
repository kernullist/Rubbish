#pragma once

extern "C"
{
#include <ntddk.h>
#include <wdm.h>
#include <ntstrsafe.h>
}

#ifndef _FN_
#define _FN_				__FUNCTION__
#endif

#ifndef _LN_
#define _LN_				__LINE__
#endif

#define TAG_NAME			'TFPS'


void
UnloadRoutine(
	IN	PDRIVER_OBJECT		pDriverObject
	);

void
NTAPI
OnUserConnect(
	IN	HANDLE				processId,
	IN	PVOID				pContext
	);

void
NTAPI
OnUserDisconnect(
	IN	HANDLE				processId,
	IN	PVOID				pContext
	);

void
KnProcessNotifyRoutineEx(
	PEPROCESS				pProcess,
	HANDLE					processId,
	PPS_CREATE_NOTIFY_INFO	pCreateInfo
	);
