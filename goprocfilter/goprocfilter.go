// goprocfilter project goprocfilter.go
package goprocfilter

import (
	"C"
	"fmt"
	"syscall"
	"unsafe"
)

const (
	FILTER_NAME = "SimpleProcFilter"
	KN_MAX_PATH = 512
)

// KN_PROCESS_INFO structure
type KnProcessInfo struct {
	parentProcessId uint32
	processId       uint32
	processPath     [KN_MAX_PATH]uint16
}

// KN_PROCESS_DECISION structure
type KnProcessDecision struct {
	processId uint32
	isAllowed uint32
}

type GoProcessCallback func(ParentPid, ProcessId uint32, ProcessPath string) (isAllowed bool)

type GoProcFilter struct {
	knComm          KNCOMM
	callbackRoutine GoProcessCallback
}

func MakeKnComm(callback GoProcessCallback) *GoProcFilter {
	return &GoProcFilter{
		knComm:          0,
		callbackRoutine: callback,
	}
}

func (goProcFlt *GoProcFilter) goProcFilterCallback(dataId uint32, processInfo *KnProcessInfo, dataSize uintptr, isReplyRequired uint32, context uintptr) uintptr {
	processPath := syscall.UTF16ToString(processInfo.processPath[:KN_MAX_PATH])

	allowed := true

	allowed = goProcFlt.callbackRoutine(processInfo.parentProcessId, processInfo.processId, processPath)

	decision := KnProcessDecision{
		processId: processInfo.processId,
	}

	if allowed == true {
		decision.isAllowed = 1
	} else {
		decision.isAllowed = 0
	}

	ret, _, _ := procReplyDataViaKnComm.Call(
		uintptr(goProcFlt.knComm),
		uintptr(dataId),
		uintptr(unsafe.Pointer(&decision)),
		uintptr(unsafe.Sizeof(decision)))
	if ret == 0 {
		fmt.Println("Error procReplyDataViaKnComm")
	}
	return 0
}

func (goProcFlt *GoProcFilter) ConnectToKnComm() bool {
	if goProcFlt.knComm != 0 {
		return true
	}

	cbInfo := KnCommCbInfo{
		procOnDataRecv: syscall.NewCallback(goProcFlt.goProcFilterCallback),
		context:        0,
	}

	goProcFlt.knComm = 0

	ret, _, _ := procConnectToKnComm.Call(
		uintptr(unsafe.Pointer(syscall.StringToUTF16Ptr(FILTER_NAME))),
		uintptr(unsafe.Pointer(&cbInfo)),
		uintptr(unsafe.Pointer(&goProcFlt.knComm)))
	if ret == 0 {
		fmt.Println("Error procConnectToKnComm")
		return false
	}

	return true
}

func (goProcFlt *GoProcFilter) DisconnectFromKnComm() bool {
	if goProcFlt.knComm == 0 {
		return true
	}

	ret, _, _ := procDisconnectFromKnComm.Call(
		uintptr(unsafe.Pointer(goProcFlt.knComm)))
	if ret == 0 {
		fmt.Println("Error procDisconnectFromKnComm")
		return false
	}

	goProcFlt.knComm = 0
	return true
}
