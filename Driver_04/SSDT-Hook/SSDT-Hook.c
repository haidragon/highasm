#include <ntifs.h>
#pragma pack(1)

//SSDTHook中，我们有2个关键问题：
//1 怎么找到SSDT，这个表是导出的，所以直接声明就可以
//2 SSDT是只读的，怎么改
//  2.1 通过修改CR0 去掉内核层的页保护
//  2.2 可以通过MDL重映射的方式，去修改。


typedef struct _ServiceDesriptorEntry
{
	ULONG *ServiceTableBase;        // 服务表基址
	ULONG *ServiceCounterTableBase; // 计数表基址
	ULONG NumberOfServices;         // 表中项的个数
	UCHAR *ParamTableBase;          // 参数表基址
}SSDTEntry, *PSSDTEntry;
#pragma pack()
ULONG         g_uProtectPID;
// 导入SSDT
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
	__asm { //关闭内存保护
		push eax;
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
		pop eax;
	}

}
void OnProtect()
{
	__asm { //恢复内存保护
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
	}

}


void OnHook()
{
	//关掉页保护
	OffProtect();
	KeServiceDescriptorTable.ServiceTableBase[0xBE] = (ULONG)MyNtOpenProcess;
	//开启页保护
	OnProtect();
}

void OffHook()
{
	//关掉页保护
	OffProtect();
	KeServiceDescriptorTable.ServiceTableBase[0xBE] = (ULONG)g_OldOpenProcess;
	//开启页保护
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