#include <ntifs.h>


VOID DriverUnload(PDRIVER_OBJECT pDriver);
// 根据PID返回进程EPROCESS，失败返回NULL
PEPROCESS LookupProcess(HANDLE hPid)
{
	PEPROCESS pEProcess = NULL;
	if (NT_SUCCESS(PsLookupProcessByProcessId(
		hPid, &pEProcess)))
		return pEProcess;
	return NULL;
}


VOID ChangeData(ULONG uId)
{
	KAPC_STATE ks;

	//1 根据ID获得进程内核对象
	PEPROCESS pEprocess = LookupProcess((HANDLE)uId);

	//2 挂靠到此进程上去
	//需要注意：不能挂靠之后，将内存中的数据往用户层地址存储，是不对的。
	//因为当挂靠到目标进程之后，用户层地址就是目标进程的了，也就存储到
	//目标进程中，而且目标进程的那个地址不一定有效，可能造成崩溃。
	KeStackAttachProcess(
		pEprocess,
		&ks
	);

	//3 修改内存
	char * p = (char *)0x1dfaf0;
	p[5] = '1';
	p[6] = '5';
	p[7] = 'p';
	p[8] = 'b';
	p[9] = '\0';
	//4 解除挂靠
	KeUnstackDetachProcess(&ks);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgBreakPoint();

	ChangeData(1716);


	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
}