# [개발 환경]
go 1.5.3
gcc 4.9.3
kncomm 1.0.0.0

# [사용 예제]
```go
package main

import (
	"fmt"
	"goprocfilter"
	"strings"
)

func MyCallback(ParentPid, ProcessId uint32, ProcessPath string) (isAllowed bool) {
	fmt.Println("\n[New Process Info]")
	fmt.Println("Parent Pid : ", ParentPid)
	fmt.Println("Process Pid : ", ProcessId)
	fmt.Println("Process Path : ", ProcessPath)

	if strings.EqualFold(ProcessPath, "\\??\\C:\\Windows\\System32\\Calc.exe") == true {
		fmt.Println("Block to Execute!!!")
		isAllowed = false
	} else {
		isAllowed = true
	}
	return
}

func main() {
	goprocflt := goprocfilter.MakeKnComm(MyCallback)

	if goprocflt.ConnectToKnComm() == false {
		fmt.Println("Error ConnectToKnComm")
		return
	}

	fmt.Println("Go Process Filter Started... Press Enter When Exit")
	fmt.Scanln()

	goprocflt.DisconnectFromKnComm()
}
```
