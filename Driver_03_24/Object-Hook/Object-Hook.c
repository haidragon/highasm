#include <ntifs.h>
VOID DriverUnload(PDRIVER_OBJECT objDriver);
typedef ULONG(*OBGETOBJECTTYPE)(PVOID Object);
typedef NTSTATUS(*PARSEPRODECEDURE)(IN PVOID ParseObject,
	IN PVOID ObjectType,
	IN OUT PACCESS_STATE AccessState,
	IN KPROCESSOR_MODE AccessMode,
	IN ULONG Attributes,
	IN OUT PUNICODE_STRING CompleteName,
	IN OUT PUNICODE_STRING RemainingName,
	IN OUT PVOID Context OPTIONAL,
	IN PSECURITY_QUALITY_OF_SERVICE SecurityQos OPTIONAL,
	OUT PVOID *Object);
PARSEPRODECEDURE g_OldFun;
NTSTATUS NewParseProcedure(IN PVOID ParseObject,
	IN PVOID ObjectType,
	IN OUT PACCESS_STATE AccessState,
	IN KPROCESSOR_MODE AccessMode,
	IN ULONG Attributes,
	IN OUT PUNICODE_STRING CompleteName,
	IN OUT PUNICODE_STRING RemainingName,
	IN OUT PVOID Context OPTIONAL,
	IN PSECURITY_QUALITY_OF_SERVICE SecurityQos OPTIONAL,
	OUT PVOID *Object)
{
	KdPrint(("Hook Success\n"));

	return g_OldFun(ParseObject, ObjectType, AccessState, AccessMode, Attributes,
		CompleteName,
		RemainingName, Context, SecurityQos, Object);
}
OBGETOBJECTTYPE g_OBGetObjectType;
//���һ��������ַ����������ܹ�ͨ���ں˶���õ��ں˶��������
VOID GetObjectTypeAddress()
{
	PUCHAR addr;
	UNICODE_STRING pslookup;
	RtlInitUnicodeString(&pslookup, L"ObGetObjectType");
	
	//               ������Ӧ�ò��GetProcAddress
	addr = (PUCHAR)MmGetSystemRoutineAddress(&pslookup);


	g_OBGetObjectType = (OBGETOBJECTTYPE)addr;
}
typedef struct _OBJECT_TYPE_INITIALIZER {
	USHORT Length;
	UCHAR ObjectTypeFlags;
	UCHAR CaseInsensitive;
	UCHAR UnnamedObjectsOnly;
	UCHAR  UseDefaultObject;
	UCHAR  SecurityRequired;
	UCHAR MaintainHandleCount;
	UCHAR MaintainTypeList;
	UCHAR SupportsObjectCallbacks;
	UCHAR CacheAligned;
	ULONG ObjectTypeCode;
	BOOLEAN InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	BOOLEAN   ValidAccessMask;
	BOOLEAN   RetainAccess;
	POOL_TYPE PoolType;
	BOOLEAN DefaultPagedPoolCharge;
	BOOLEAN DefaultNonPagedPoolCharge;
	PVOID DumpProcedure;
	ULONG OpenProcedure;
	PVOID CloseProcedure;
	PVOID DeleteProcedure;
	ULONG ParseProcedure;
	ULONG SecurityProcedure;
	ULONG QueryNameProcedure;
	UCHAR OkayToCloseProcedure;
} OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;
typedef struct _OBJECT_TYPE {
	LIST_ENTRY TypeList;
	UNICODE_STRING Name;
	PVOID DefaultObject;
	ULONG Index;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	OBJECT_TYPE_INITIALIZER TypeInfo;
	ULONG  TypeLock;
	ULONG   Key;
	LIST_ENTRY   CallbackList;
} OBJECT_TYPE, *POBJECT_TYPE;
HANDLE KernelCreateFile(
	IN PUNICODE_STRING pstrFile, // �ļ�·����������
	IN BOOLEAN         bIsDir)   // �Ƿ�Ϊ�ļ���
{
	HANDLE          hFile = NULL;
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG           ulShareAccess =
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	ULONG           ulCreateOpt =
		FILE_SYNCHRONOUS_IO_NONALERT;
	// 1. ��ʼ��OBJECT_ATTRIBUTES������
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes =
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // ���س�ʼ����ϵĽṹ��
		pstrFile,      // �ļ���������
		ulAttributes,  // ��������
		NULL, NULL);   // һ��ΪNULL
	// 2. �����ļ�����
	ulCreateOpt |= bIsDir ?
	FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
	Status = ZwCreateFile(
		&hFile,                // �����ļ����
		GENERIC_ALL,           // �ļ���������
		&objAttrib,            // OBJECT_ATTRIBUTES
		&StatusBlock,          // ���ܺ����Ĳ������
		0,                     // ��ʼ�ļ���С
		FILE_ATTRIBUTE_NORMAL, // �½��ļ�������
		ulShareAccess,         // �ļ�����ʽ
		FILE_OPEN_IF,          // �ļ�������򿪲������򴴽�
		ulCreateOpt,           // �򿪲����ĸ��ӱ�־λ
		NULL,                  // ��չ������
		0);                   // ��չ����������
	if (!NT_SUCCESS(Status))
		return (HANDLE)-1;
	return hFile;
}
void OnHook()
{
	//1 ����һ���ļ����õ�һ���ļ�����
	UNICODE_STRING ustrFilePath;
	RtlInitUnicodeString(&ustrFilePath,
		L"\\??\\D:\\123.txt");
	HANDLE hFile = KernelCreateFile(&ustrFilePath, FALSE);
	PVOID pObject;
	ObReferenceObjectByHandle(hFile, GENERIC_ALL, NULL, KernelMode, &pObject, NULL);

	//2 ͨ������ļ�����õ�OBJECT_TYPE����ṹ��
	OBJECT_TYPE * FileType = NULL;
	FileType = (OBJECT_TYPE *)g_OBGetObjectType(pObject);
	//3 �����������ַ��������
	g_OldFun = (PARSEPRODECEDURE)FileType->TypeInfo.ParseProcedure;
	//4 �����������ַ�滻Ϊ�Լ��ĺ�����
	FileType->TypeInfo.ParseProcedure = (ULONG)NewParseProcedure;
}

void OffHook()
{

}

NTSTATUS DriverEntry(
	PDRIVER_OBJECT  pDriver,
	PUNICODE_STRING strRegPath)
{
	// �����������δ���ò����ľ���
	UNREFERENCED_PARAMETER(strRegPath);
	// ��ӡһ���ַ�������ע������ж�غ������Ա�������ж��
	DbgBreakPoint();//_asm int 3
	__try
	{
		GetObjectTypeAddress();
		OnHook();
	}
	except(1)
	{
		KdPrint(("�ӹ�һ���쳣\n"));
	}
	KdPrint(("My First Dirver!"));
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}
VOID DriverUnload(PDRIVER_OBJECT objDriver) {
	// �����������δ���ò����ľ���
	UNREFERENCED_PARAMETER(objDriver);
	// ʲôҲ������ֻ��ӡһ���ַ���
	KdPrint(("My Dirver is unloading..."));
}