#include <ntifs.h>
// ����TID�����߳�ETHREAD��ʧ�ܷ���NULL
PETHREAD LookupThread(HANDLE hTid)
{
	PETHREAD pEThread = NULL;
	if (NT_SUCCESS(PsLookupThreadByThreadId(
		hTid,
		&pEThread)))
		return pEThread;
	return NULL;
}
VOID EnumThread(
	PEPROCESS pEProcess  //Ҫö�ٵ�����һ�����̵��߳�
) 
{
	PEPROCESS pEProc = NULL;
	PETHREAD  pEThrd = NULL;
	// ѭ�������̣߳������̵߳����ֵ������0x25600��
	ULONG i = 0;
	for (i = 4; i < 0x25600; i += 4) {
		// a. ����TID����ETHREAD
		pEThrd = LookupThread((HANDLE)i);
		if (!pEThrd)  continue;
		// b. ����߳��������̣����������ӡ�߳���Ϣ
		pEProc = IoThreadToProcess(pEThrd);
		if (pEProc == pEProcess) {
			DbgPrint("[THREAD]ETHREAD=%p TID=%ld\n",
				pEThrd, (ULONG)PsGetThreadId(pEThrd));
		}
		// c. ���̶߳������ü�����1
		ObDereferenceObject(pEThrd);
	}
}
