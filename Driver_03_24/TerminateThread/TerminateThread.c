#include <ntifs.h>
NTSTATUS ZwOpenThread(
	_Out_  PHANDLE ThreadHandle,
	_In_   ACCESS_MASK DesiredAccess,
	_In_   POBJECT_ATTRIBUTES ObjectAttributes,
	_In_   PCLIENT_ID ClientId);
typedef NTSTATUS(__fastcall *ZWTERMINATETHREAD)(
	HANDLE hThread,
	ULONG uExitCode);
//�����̣߳���ͣ�̣߳��ָ��̣߳���Щ����û�е�����
//�͵��Լ�ȥ��
ZWTERMINATETHREAD ZwTerminateThread = 0x83e79afc; //������ַ�����Լ��ҵ��ģ�û�е���
void KernelKillThread(ULONG tID) {
	HANDLE            hThread = NULL;
	CLIENT_ID         ClientId = { 0 };
	OBJECT_ATTRIBUTES objAttribut =
	{ sizeof(OBJECT_ATTRIBUTES) };
	ClientId.UniqueProcess = 0;
	ClientId.UniqueThread = (HANDLE)tID; // TID  
										  //���̣߳���������Ч��������߳�
	ZwOpenThread(&hThread, 1, &objAttribut, &ClientId);
	if (hThread) {
		ZwTerminateThread(hThread, 0);
		ZwClose(hThread);
	}
}
