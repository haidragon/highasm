#include <ntifs.h>
NTSTATUS ZwOpenThread(
	_Out_  PHANDLE ThreadHandle,
	_In_   ACCESS_MASK DesiredAccess,
	_In_   POBJECT_ATTRIBUTES ObjectAttributes,
	_In_   PCLIENT_ID ClientId);
typedef NTSTATUS(__fastcall *ZWTERMINATETHREAD)(
	HANDLE hThread,
	ULONG uExitCode);
//结束线程，暂停线程，恢复线程，这些函数没有导出，
//就得自己去找
ZWTERMINATETHREAD ZwTerminateThread = 0x83e79afc; //函数地址，是自己找到的，没有导出
void KernelKillThread(ULONG tID) {
	HANDLE            hThread = NULL;
	CLIENT_ID         ClientId = { 0 };
	OBJECT_ATTRIBUTES objAttribut =
	{ sizeof(OBJECT_ATTRIBUTES) };
	ClientId.UniqueProcess = 0;
	ClientId.UniqueThread = (HANDLE)tID; // TID  
										  //打开线程，如果句柄有效，则结束线程
	ZwOpenThread(&hThread, 1, &objAttribut, &ClientId);
	if (hThread) {
		ZwTerminateThread(hThread, 0);
		ZwClose(hThread);
	}
}
