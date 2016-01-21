#pragma once


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// author	: kernullist.gloryo
// date		: 2008.07.24


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C"
{
#include <ntddk.h>
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef void (NTAPI* FN_ONUSERCONNECT)(
	IN	HANDLE			processId,
	IN	PVOID			pContext
	);

typedef void (NTAPI* FN_ONUSERDISCONNECT)(
	IN	HANDLE			processId,
	IN	PVOID			pContext
	);

typedef NTSTATUS (NTAPI* FN_ONUSERIOCTL)(
	IN	ULONG			controlCode,
	IN	PVOID			pInputBuffer,
	IN	ULONG			inputBufferSize,
	OUT	PVOID			pOutputBuffer,
	IN	ULONG			outputBufferSize,
	IN	PVOID			pContext
	);

typedef struct _KNCOMM_CB_INFO
{
	FN_ONUSERCONNECT	pfnOnUserConnect;
	FN_ONUSERDISCONNECT	pfnOnUserDisconnect;
	FN_ONUSERIOCTL		pfnOnUserIoctl;
	PVOID				pCallbackContext;

} KNCOMM_CB_INFO, *PKNCOMM_CB_INFO;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOLEAN
NTAPI
InitializeKnCommLib(
	IN	PCWSTR			pUniqueName
	);

void
NTAPI
FinalizeKnCommLib(
	void
	);

BOOLEAN
NTAPI
CreateKnCommClient(
	IN	PKNCOMM_CB_INFO	pKnCommInfo,
	OUT	PULONG			pKernelModeId
	);

BOOLEAN
NTAPI
DeleteKnCommClient(
	IN	ULONG			kernelModeId
	);

BOOLEAN
NTAPI
SendToUserViaKnComm(
	IN	ULONG			kernelModeId,
	IN	PVOID			pSendDataBuffer,
	IN	SIZE_T			sendDataBufferSize,
	IN	BOOLEAN			waitReply,
	IN	LONG			waitTimeOutMS,
	OUT	PVOID*			ppReplyDataBuffer,		// must Release by ReleaseKnCommDataBuffer
	OUT	PSIZE_T			pReplyDataBufferSize
	);

void
ReleaseKnCommDataBuffer(
	IN	PVOID			pBuffer
	);