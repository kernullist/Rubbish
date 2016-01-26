package goprocfilter

import (
	"syscall"
)

type (
	KNCOMM uintptr
)

// KNCOMM_CB_INFO structure
type KnCommCbInfo struct {
	procOnDataRecv uintptr
	context        uintptr
}

// KnCommLibUser.dll API
var (
	KnCommLibUserDll = syscall.NewLazyDLL("KnCommLibUser.dll")

	procConnectToKnComm      = KnCommLibUserDll.NewProc("ConnectToKnComm")
	procDisconnectFromKnComm = KnCommLibUserDll.NewProc("DisconnectFromKnComm")
	procReplyDataViaKnComm   = KnCommLibUserDll.NewProc("ReplyDataViaKnComm")
)
