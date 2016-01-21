#include "KbdRdr.h"
#include "..\Common\KernelUserCommon.h"
#include "..\KnCommLib\KnCommLib.h"
#include "..\KnKbdLib\KnKbdLib.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ULONG	g_myKnCommId = 0;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C
NTSTATUS
DriverEntry(
	IN	PDRIVER_OBJECT	pDriverObject,
	IN	PUNICODE_STRING	pRegistryPath
	)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	do
	{
		//
		// KnComm Lib 초기화
		//

		if (InitializeKnCommLib(KBDRDR_DRIVER_NAME) == FALSE)
		{
			break;
		}


		//
		// KNCOMM 클라이언트 생성
		//

		KNCOMM_CB_INFO knCommCbinfo;
		knCommCbinfo.pfnOnUserConnect = OnUserConnect;
		knCommCbinfo.pfnOnUserDisconnect = OnUserDisconnect;
		knCommCbinfo.pfnOnUserIoctl = NULL;
		knCommCbinfo.pCallbackContext = NULL;

		if (CreateKnCommClient(&knCommCbinfo, &g_myKnCommId) == FALSE)
		{
			break;
		}


		//
		// 키보드 Lib 초기화
		//

		if (InitializeKnKbdLib() == FALSE)
		{
			break;
		}


		//
		// 성공
		//

		pDriverObject->DriverUnload = UnloadRoutine;

		status = STATUS_SUCCESS;

	} while (FALSE);

	if (!NT_SUCCESS(status))
	{
		FinalizeKnKbdLib();
		FinalizeKnCommLib();
	}

	return status;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
UnloadRoutine(
	IN	PDRIVER_OBJECT	pDriverObject
	)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	
	//
	// 키보드 읽기를 중지 후 Finalize한다.
	//

	KnKbdStopRead();
	FinalizeKnKbdLib();


	//
	// KnComm 클라이언트를 제거 후 Finalize한다.
	//

	DeleteKnCommClient(g_myKnCommId);
	FinalizeKnCommLib();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
NTAPI
OnKeyboardReadCompletion(
	IN	PKNKBD_READDATA			pKnKbdInputData,
	IN	PVOID					pContext
	)
{
	UNREFERENCED_PARAMETER(pContext);


	//
	// 키 이벤트가 발생했다.
	// 유저 모드 모듈로 전송해주자.
	//

	DbgPrint("[%s:%d] SC : %02X Flags : 0x%X SessionID : %d\n", _FN_, _LN_, pKnKbdInputData->makeCode, pKnKbdInputData->flags, pKnKbdInputData->sessionId);

	KEYEVENT_DATA keyEventData;
	keyEventData.makeCode = pKnKbdInputData->makeCode;
	keyEventData.flags = pKnKbdInputData->flags;
	
	SendToUserViaKnComm(g_myKnCommId, &keyEventData, sizeof(keyEventData), FALSE, 0, NULL, 0);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
NTAPI
OnUserConnect(
	IN	HANDLE			processId,
	IN	PVOID			pContext
	)
{
	UNREFERENCED_PARAMETER(processId);
	UNREFERENCED_PARAMETER(pContext);


	//
	// 유저 모드 모듈이 접속했다.
	// 키보드 읽기를 시작한다.
	//

	KnKbdStartRead(OnKeyboardReadCompletion, NULL);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
NTAPI
OnUserDisconnect(
	IN	HANDLE			processId,
	IN	PVOID			pContext
	)
{
	UNREFERENCED_PARAMETER(processId);
	UNREFERENCED_PARAMETER(pContext);


	//
	// 유저 모드 모듈이 접속을 끊었다.
	// 키보드 읽기를 중지한다.
	//

	KnKbdStopRead();
}

