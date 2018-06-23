#include <ntifs.h>
VOID DriverUnload(PDRIVER_OBJECT pDriver);
ULONG g_OldKiFastCallEntry = 0;


ULONG         g_uSSDT_Index;
PCLIENT_ID    g_pClientPID;
ULONG         g_uProtectPID;
PULONG          g_pAccessMask;

//��Ҫһ���Լ���KiFastCallEntry
_declspec(naked) void MyKiFastCallEntry()
{
	// 1. ��EDXָ����û��ռ�ջ��ȡ����Ӧ������������ZwOpen-
	//     Process��ջ���죨��ʱ���迼���������ú�����ջ�������
	// +00 �û��ռ�Call���ص�ַ
	// +04 �û��ռ�Call���ص�ַ
	// +08 ����1 ( ProcessHandle�����̾�� )
	// +12 ����2 ( DesiredAccess������Ȩ�� )
	// +16 ����3 ( ObjectName���������� )
	// +20 ����4 ( ClientId�������н���PID���߳�ID�Ľṹ�� )
	_asm push DWORD PTR[EDX + 4 * 5];	// 4*5 = ����4
	_asm pop  g_pClientPID;	// �������4��ClientPID
	_asm push EDX;
	_asm add  DWORD PTR[ESP], 4 * 3;	// 4*3 = ����2
	_asm pop  g_pAccessMask;	// �������2��AccessMask
								// 2. ����ָ��PID�Ľ��̲�������
	_asm pushad;
	// 2.1 �жϷ�ΪZwOpenProcess�ĵ��ú�0xBE������ִ�б���
	_asm mov g_uSSDT_Index, eax;
	if (g_uSSDT_Index == 0xBE) {
		// 2.2 �ж������������Ƿ�Ϊ�ܱ������̣���ͬʱ�жϷ���Ȩ
		//     �����Ƿ�����ɽ�������Ȩ��PROCESS_TERMINATE��
		//     �ǵĻ��޸��������ȥ����Ȩ��
		if (((ULONG)g_pClientPID->UniqueProcess == g_uProtectPID))
		{
			*g_pAccessMask = 0;
		}
	}
	_asm popad;
	// 3. ��ת��ϵͳԭKiFastCallEntry������ִ��ʣ�����
	_asm jmp g_OldKiFastCallEntry;
}

void InitInfo()
{

	g_uSSDT_Index = *((CHAR*)ZwOpenProcess + 1);
	g_uSSDT_Index = 0xBE;
	g_uProtectPID = 3920;
	_asm
	{
		push eax;
		push ecx;
		mov ecx, 0x176;
		rdmsr;//���������ָ�176��msr��ֵ�ͱ�������edx��eax��
		mov g_OldKiFastCallEntry,eax;
		pop ecx;
		pop eax;
	}
}
void OnHook()
{
	_asm
	{
		push eax;
		push ecx;
		mov ecx, 0x176;
		mov eax, MyKiFastCallEntry;
		wrmsr;//���������ָ��Ͱ�edx:eax�е����ŵ�176�ŵ�msr�Ĵ�������
		pop ecx;
		pop eax;
	}
}

void OffHook()
{
	_asm
	{
		push eax;
		push ecx;
		mov ecx, 0x176;
		mov eax, g_OldKiFastCallEntry;
		wrmsr;//���������ָ��Ͱ�edx:eax�е����ŵ�176�ŵ�msr�Ĵ�������
		pop ecx;
		pop eax;
	}
}



NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver,PUNICODE_STRING pPath)
{

	UNREFERENCED_PARAMETER(pPath);
	DbgBreakPoint();
	InitInfo();
	OnHook();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}



VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	OffHook();
	UNREFERENCED_PARAMETER(pDriver);
}