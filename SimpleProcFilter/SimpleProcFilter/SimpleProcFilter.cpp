#include "SimpleProcFilter.h"
#include "..\Common\KernelUserCommon.h"
#include "..\KnCommLib\KnCommLib.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ULONG g_myKnCommId = 0;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C
NTSTATUS
DriverEntry(
	IN	PDRIVER_OBJECT		pDriverObject,
	IN	PUNICODE_STRING		pRegistryPath
	)
{
	DbgPrint("[%s:%d]\n", _FN_, _LN_);

	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	do
	{
		//
		// KnComm Lib 초기화
		//

		if (InitializeKnCommLib(FILTER_NAME) == FALSE)
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
		// 성공
		//

		pDriverObject->DriverUnload = UnloadRoutine;

		status = STATUS_SUCCESS;

	} while (FALSE);

	if (!NT_SUCCESS(status))
	{
		FinalizeKnCommLib();
	}

	return status;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
UnloadRoutine(
	IN	PDRIVER_OBJECT		pDriverObject
	)
{
	DbgPrint("[%s:%d]\n", _FN_, _LN_);

	UNREFERENCED_PARAMETER(pDriverObject);

	//
	// KnComm 클라이언트를 제거 후 Finalize한다.
	//

	DeleteKnCommClient(g_myKnCommId);
	FinalizeKnCommLib();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NTSTATUS
PassRoutine(
	IN	PDEVICE_OBJECT		pDeviceObject,
	IN	PIRP				pIrp
	)
{
	UNREFERENCED_PARAMETER(pDeviceObject);

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
NTAPI
OnUserConnect(
	IN	HANDLE				processId,
	IN	PVOID				pContext
	)
{
	UNREFERENCED_PARAMETER(pContext);

	DbgPrint("[%s:%d] User Process ID : %d\n", _FN_, _LN_, processId);

	NTSTATUS status = PsSetCreateProcessNotifyRoutineEx(KnProcessNotifyRoutineEx, FALSE);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("[%s:%d] Error on PsSetCreateProcessNotifyRoutineEx(FALSE). status : 0x%X\n", _FN_, _LN_, status);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
NTAPI
OnUserDisconnect(
	IN	HANDLE				processId,
	IN	PVOID				pContext
	)
{
	UNREFERENCED_PARAMETER(pContext);

	DbgPrint("[%s:%d] User Process ID : %d\n", _FN_, _LN_, processId);

	NTSTATUS status = PsSetCreateProcessNotifyRoutineEx(KnProcessNotifyRoutineEx, TRUE);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("[%s:%d] Error on PsSetCreateProcessNotifyRoutineEx(TRUE). status : 0x%X\n", _FN_, _LN_, status);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
KnProcessNotifyRoutineEx(
	PEPROCESS				pProcess,
	HANDLE					processId,
	PPS_CREATE_NOTIFY_INFO	pCreateInfo
	)
{
	UNREFERENCED_PARAMETER(pProcess);

	PKN_PROCESS_INFO pProcessInfo = NULL;
	PVOID pReply = NULL;

	do
	{
		if (pCreateInfo == NULL)
		{
			break;
		}

		DbgPrint("[%s:%d] New Process ID : %d\n", _FN_, _LN_, processId);

		if (pCreateInfo->ImageFileName == NULL || pCreateInfo->ImageFileName->Length >= KN_MAX_PATH * sizeof(WCHAR))
		{
			break;
		}

		pProcessInfo = (PKN_PROCESS_INFO)ExAllocatePoolWithTag(PagedPool, sizeof(KN_PROCESS_INFO), TAG_NAME);
		if (pProcessInfo == NULL)
		{
			break;
		}

		RtlZeroMemory(pProcessInfo, sizeof(KN_PROCESS_INFO));

		pProcessInfo->parentProcessId = (ULONG)pCreateInfo->ParentProcessId;
		pProcessInfo->processId = (ULONG)processId;
		RtlCopyMemory(pProcessInfo->processPath, pCreateInfo->ImageFileName->Buffer, pCreateInfo->ImageFileName->Length);


		//
		// 프로세스 실행 여부를 응용프로그램에 쿼리한다. (타임아웃 10초)
		//

		SIZE_T replyDataSize = 0;

		if (SendToUserViaKnComm(g_myKnCommId, pProcessInfo, sizeof(KN_PROCESS_INFO), TRUE, 10000, &pReply, &replyDataSize) != TRUE)
		{
			break;
		}

		if (replyDataSize != sizeof(KN_PROCESS_DECISION))
		{
			break;
		}

		PKN_PROCESS_DECISION pUserDecision = (PKN_PROCESS_DECISION)pReply;

		DbgPrint("[%s:%d] Process Id : %d ==> isAllowed : %d\n", _FN_, _LN_, processId, pUserDecision->isAllowed);

		if (pUserDecision->isAllowed == FALSE)
		{
			//
			// 허용되지 않은 프로세스 실행이다.
			//

			DbgPrint("[%s:%d] Not Allowed Process!!!\n", _FN_, _LN_);

			pCreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
		}

	} while (FALSE);

	if (pReply != NULL)
	{
		ReleaseKnCommDataBuffer(pReply);
		pReply = NULL;
	}

	if (pProcessInfo != NULL)
	{
		ExFreePoolWithTag(pProcessInfo, TAG_NAME);
		pProcessInfo = NULL;
	}
}