#include <ntifs.h>


VOID DriverUnload(PDRIVER_OBJECT pDriver);
// ����PID���ؽ���EPROCESS��ʧ�ܷ���NULL
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

	//1 ����ID��ý����ں˶���
	PEPROCESS pEprocess = LookupProcess((HANDLE)uId);

	//2 �ҿ����˽�����ȥ
	//��Ҫע�⣺���ܹҿ�֮�󣬽��ڴ��е��������û����ַ�洢���ǲ��Եġ�
	//��Ϊ���ҿ���Ŀ�����֮���û����ַ����Ŀ����̵��ˣ�Ҳ�ʹ洢��
	//Ŀ������У�����Ŀ����̵��Ǹ���ַ��һ����Ч��������ɱ�����
	KeStackAttachProcess(
		pEprocess,
		&ks
	);

	//3 �޸��ڴ�
	char * p = (char *)0x1dfaf0;
	p[5] = '1';
	p[6] = '5';
	p[7] = 'p';
	p[8] = 'b';
	p[9] = '\0';
	//4 ����ҿ�
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