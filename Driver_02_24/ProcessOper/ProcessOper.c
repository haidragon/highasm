#include <ntifs.h>
VOID DriverUnload(PDRIVER_OBJECT pDriver);
// 1. 声明要使用此函数
NTKERNELAPI NTSTATUS PsSuspendProcess(PEPROCESS pEProcess);

// 根据PID返回进程EPROCESS，失败返回NULL
PEPROCESS LookupProcess(HANDLE hPid)
{
	PEPROCESS pEProcess = NULL;
	if (NT_SUCCESS(PsLookupProcessByProcessId(
		hPid, &pEProcess)))
		return pEProcess;
	return NULL;
}


BOOLEAN KernelSuspendProcess(ULONG Id)
{
	//1. 先根据ID得到EPORCESS
	PEPROCESS pEProcess;
	if ((pEProcess = LookupProcess((HANDLE)Id) )!= NULL)
	{
		//2. 暂停进程
		if (NT_SUCCESS(PsSuspendProcess(pEProcess)))
			return FALSE;
	}
	return TRUE;

}
NTKERNELAPI NTSTATUS PsResumeProcess(
	PEPROCESS pEProcess);

BOOLEAN KernelResumeProcess(ULONG Id)
{
	//1. 先根据ID得到EPORCESS
	PEPROCESS pEProcess;
	if ((pEProcess = LookupProcess((HANDLE)Id)) != NULL)
	{
		//2. 暂停进程
		if (NT_SUCCESS(PsResumeProcess(pEProcess)))
			return FALSE;
	}
	return TRUE;

}

void KernelKillProcess() {
	HANDLE            hProcess = NULL;
	CLIENT_ID         ClientId = { 0 };
	OBJECT_ATTRIBUTES objAttribut =
	{ sizeof(OBJECT_ATTRIBUTES) };
	ClientId.UniqueProcess = (HANDLE)1234; // PID
	ClientId.UniqueThread = 0;
	// 打开进程，如果句柄有效，则结束进程
	ZwOpenProcess(
		&hProcess,    // 返回打开后的句柄
		1,            // 访问权限
		&objAttribut, // 对象属性
		&ClientId);   // 进程ID结构
	if (hProcess) {
		ZwTerminateProcess(hProcess, 0);
		ZwClose(hProcess);
	};
}
NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(
	IN PEPROCESS pEProcess);
NTKERNELAPI UCHAR* PsGetProcessImageFileName(
	IN PEPROCESS pEProcess);
VOID EnumProcess() {
	PEPROCESS pEProc = NULL;
	// 循环遍历进程（假设线程的最大值不超过0x25600）
	ULONG i = 0;
	for (i = 4; i<0x25600; i = i + 4) {
		// a.根据PID返回PEPROCESS
		pEProc = LookupProcess((HANDLE)i);
		if (!pEProc) continue;
		// b. 打印进程信息
		DbgPrint("EPROCESS=%p PID=%ld PPID=%ld Name=%s\n",
			pEProc, (UINT32)PsGetProcessId(pEProc),
			(UINT32)PsGetProcessInheritedFromUniqueProcessId(pEProc),
			PsGetProcessImageFileName(pEProc));
		// c. 将进程对象引用计数减1
		ObDereferenceObject(pEProc);
		DbgPrint("\n");
	}
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint(("Enter  Driver"));
	DbgBreakPoint();
	//KernelSuspendProcess(3364);
	EnumProcess();

	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}

//在这个函数中写的是资源的清理
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint(("Leave  Driver"));
	//KernelResumeProcess(3364);
}