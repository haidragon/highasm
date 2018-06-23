#include <ntifs.h>
#pragma pack(1)

//SSDTHook�У�������2���ؼ����⣺
//1 ��ô�ҵ�SSDT��������ǵ����ģ�����ֱ�������Ϳ���
//2 SSDT��ֻ���ģ���ô��
//  2.1 ͨ���޸�CR0 ȥ���ں˲��ҳ����
//  2.2 ����ͨ��MDL��ӳ��ķ�ʽ��ȥ�޸ġ�


typedef struct _ServiceDesriptorEntry
{
	ULONG *ServiceTableBase;        // ������ַ
	ULONG *ServiceCounterTableBase; // �������ַ
	ULONG NumberOfServices;         // ������ĸ���
	UCHAR *ParamTableBase;          // �������ַ
}SSDTEntry, *PSSDTEntry;
#pragma pack()
ULONG         g_uProtectPID;
// ����SSDT
NTSYSAPI SSDTEntry KeServiceDescriptorTable;

VOID DriverUnload(PDRIVER_OBJECT pDriver);

typedef NTSTATUS(NTAPI *NTOPENPROCESS)(__out PHANDLE  ProcessHandle,
	__in ACCESS_MASK  DesiredAccess,
	__in POBJECT_ATTRIBUTES  ObjectAttributes,
	__in_opt PCLIENT_ID  ClientId
	);

NTOPENPROCESS g_OldOpenProcess = NULL;

NTSTATUS NTAPI MyNtOpenProcess(__out PHANDLE  ProcessHandle,
	__in ACCESS_MASK  DesiredAccess,
	__in POBJECT_ATTRIBUTES  ObjectAttributes,
	__in_opt PCLIENT_ID  ClientId
)
{
	if ((ULONG)ClientId->UniqueProcess == g_uProtectPID)
	{
		return STATUS_ABANDONED;
	}
	return  g_OldOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
}



void InitInfo()
{
	g_uProtectPID = 3436;
	g_OldOpenProcess = (NTOPENPROCESS) KeServiceDescriptorTable.ServiceTableBase[0xBE];

}
void OffProtect()
{
	__asm { //�ر��ڴ汣��
		push eax;
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
		pop eax;
	}

}
void OnProtect()
{
	__asm { //�ָ��ڴ汣��
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
	}

}


void OnHook()
{
	//�ص�ҳ����
	OffProtect();
	KeServiceDescriptorTable.ServiceTableBase[0xBE] = (ULONG)MyNtOpenProcess;
	//����ҳ����
	OnProtect();
}

void OffHook()
{
	//�ص�ҳ����
	OffProtect();
	KeServiceDescriptorTable.ServiceTableBase[0xBE] = (ULONG)g_OldOpenProcess;
	//����ҳ����
	OnProtect();
}



NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
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