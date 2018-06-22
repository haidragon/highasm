#include <ntifs.h>

KEVENT g_kEvent;

VOID DriverUnload(PDRIVER_OBJECT pDriver);

VOID ThreadProc(PVOID StartContext)
{
	ULONG uId = (ULONG)PsGetCurrentThreadId();
	KdPrint(("%wZ,%d\n", StartContext, uId));

	//执行即将结束，将事件置为激发态。
	KeSetEvent(&g_kEvent, 0, TRUE);
	//使用内核线程的时候，需要注意一点，当线程执行完毕的时候，必须主动调用下面
	//这个函数
	PsTerminateSystemThread(STATUS_SUCCESS);
}








NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgBreakPoint();


	HANDLE hThread = 0;
	CLIENT_ID Id = {0};
	UNICODE_STRING szString;
	RtlInitUnicodeString(&szString, L"Hello 15PB");
	//1 创建一个系统线程，能够执行简单代码，并且验证和主线程不是同一个线程
	ULONG uId = (ULONG)PsGetCurrentThreadId();
	KdPrint(("%wZ,Id:%d\n", &szString,uId));
	// 初始化事件对象
	KeInitializeEvent(&g_kEvent, SynchronizationEvent, FALSE);
	PsCreateSystemThread(
		&hThread,
		0,
		NULL,
		NULL,//这里填写NULL,说明创建的是内核线程
		&Id,
		ThreadProc,//回调函数
		&szString
	);
	KeWaitForSingleObject(
		&g_kEvent,
		Executive,
		KernelMode,
		FALSE,
		0         //再内核层的等待函数，0是永久等待
	);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}


VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);

}