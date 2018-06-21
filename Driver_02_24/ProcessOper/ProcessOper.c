#include <ntifs.h>
VOID DriverUnload(PDRIVER_OBJECT pDriver);
// 1. ����Ҫʹ�ô˺���
NTKERNELAPI NTSTATUS PsSuspendProcess(PEPROCESS pEProcess);

// ����PID���ؽ���EPROCESS��ʧ�ܷ���NULL
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
	//1. �ȸ���ID�õ�EPORCESS
	PEPROCESS pEProcess;
	if ((pEProcess = LookupProcess((HANDLE)Id) )!= NULL)
	{
		//2. ��ͣ����
		if (NT_SUCCESS(PsSuspendProcess(pEProcess)))
			return FALSE;
	}
	return TRUE;

}
NTKERNELAPI NTSTATUS PsResumeProcess(
	PEPROCESS pEProcess);

BOOLEAN KernelResumeProcess(ULONG Id)
{
	//1. �ȸ���ID�õ�EPORCESS
	PEPROCESS pEProcess;
	if ((pEProcess = LookupProcess((HANDLE)Id)) != NULL)
	{
		//2. ��ͣ����
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
	// �򿪽��̣���������Ч�����������
	ZwOpenProcess(
		&hProcess,    // ���ش򿪺�ľ��
		1,            // ����Ȩ��
		&objAttribut, // ��������
		&ClientId);   // ����ID�ṹ
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
	// ѭ���������̣������̵߳����ֵ������0x25600��
	ULONG i = 0;
	for (i = 4; i<0x25600; i = i + 4) {
		// a.����PID����PEPROCESS
		pEProc = LookupProcess((HANDLE)i);
		if (!pEProc) continue;
		// b. ��ӡ������Ϣ
		DbgPrint("EPROCESS=%p PID=%ld PPID=%ld Name=%s\n",
			pEProc, (UINT32)PsGetProcessId(pEProc),
			(UINT32)PsGetProcessInheritedFromUniqueProcessId(pEProc),
			PsGetProcessImageFileName(pEProc));
		// c. �����̶������ü�����1
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

//�����������д������Դ������
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint(("Leave  Driver"));
	//KernelResumeProcess(3364);
}