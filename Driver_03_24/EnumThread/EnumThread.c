#include <ntifs.h>
// 根据TID返回线程ETHREAD，失败返回NULL
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
	PEPROCESS pEProcess  //要枚举的是哪一个进程的线程
) 
{
	PEPROCESS pEProc = NULL;
	PETHREAD  pEThrd = NULL;
	// 循环遍历线程（假设线程的最大值不超过0x25600）
	ULONG i = 0;
	for (i = 4; i < 0x25600; i += 4) {
		// a. 根据TID返回ETHREAD
		pEThrd = LookupThread((HANDLE)i);
		if (!pEThrd)  continue;
		// b. 获得线程所属进程，如果相等则打印线程信息
		pEProc = IoThreadToProcess(pEThrd);
		if (pEProc == pEProcess) {
			DbgPrint("[THREAD]ETHREAD=%p TID=%ld\n",
				pEThrd, (ULONG)PsGetThreadId(pEThrd));
		}
		// c. 将线程对象引用计数减1
		ObDereferenceObject(pEThrd);
	}
}
