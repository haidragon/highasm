#include <ntifs.h>

KEVENT g_kEvent;

VOID DriverUnload(PDRIVER_OBJECT pDriver);

VOID ThreadProc(PVOID StartContext)
{
	ULONG uId = (ULONG)PsGetCurrentThreadId();
	KdPrint(("%wZ,%d\n", StartContext, uId));

	//ִ�м������������¼���Ϊ����̬��
	KeSetEvent(&g_kEvent, 0, TRUE);
	//ʹ���ں��̵߳�ʱ����Ҫע��һ�㣬���߳�ִ����ϵ�ʱ�򣬱���������������
	//�������
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
	//1 ����һ��ϵͳ�̣߳��ܹ�ִ�м򵥴��룬������֤�����̲߳���ͬһ���߳�
	ULONG uId = (ULONG)PsGetCurrentThreadId();
	KdPrint(("%wZ,Id:%d\n", &szString,uId));
	// ��ʼ���¼�����
	KeInitializeEvent(&g_kEvent, SynchronizationEvent, FALSE);
	PsCreateSystemThread(
		&hThread,
		0,
		NULL,
		NULL,//������дNULL,˵�����������ں��߳�
		&Id,
		ThreadProc,//�ص�����
		&szString
	);
	KeWaitForSingleObject(
		&g_kEvent,
		Executive,
		KernelMode,
		FALSE,
		0         //���ں˲�ĵȴ�������0�����õȴ�
	);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}


VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);

}