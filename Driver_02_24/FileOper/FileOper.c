#include <ntifs.h>
VOID DriverUnload(PDRIVER_OBJECT pDriver);
#define _countof(arr) sizeof(arr)/sizeof(arr[0])
//�����ļ�
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

//��ȡ�ļ���С
ULONG64 KernelGetFileSize(IN HANDLE hfile)
{
	// ��ѯ�ļ�״̬
	IO_STATUS_BLOCK           StatusBlock = { 0 };
	FILE_STANDARD_INFORMATION fsi = { 0 };
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	Status = ZwQueryInformationFile(
		hfile,        // �ļ����
		&StatusBlock, // ���ܺ����Ĳ������
		&fsi,         // �������һ��������������������Ϣ
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation);
	if (!NT_SUCCESS(Status))
		return 0;
	return fsi.EndOfFile.QuadPart;
}

ULONG64 KernelReadFile(
	IN  HANDLE         hfile,    // �ļ����
	IN  PLARGE_INTEGER Offset,   // �����￪ʼ��ȡ
	IN  ULONG          ulLength, // ��ȡ�����ֽ�
	OUT PVOID          pBuffer)  // �������ݵĻ���
{
	// 1. ��ȡ�ļ�
	IO_STATUS_BLOCK StatusBlock = { 0 };
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	Status = ZwReadFile(
		hfile,        // �ļ����
		NULL,         // �ź�״̬(һ��ΪNULL)
		NULL, NULL,   // ����
		&StatusBlock, // ���ܺ����Ĳ������
		pBuffer,      // �����ȡ���ݵĻ���
		ulLength,     // ��Ҫ��ȡ�ĳ���
		Offset,       // ��ȡ����ʼƫ��
		NULL);        // һ��ΪNULL
	if (!NT_SUCCESS(Status))  return 0;
	// 2. ����ʵ�ʶ�ȡ�ĳ���
	return StatusBlock.Information;
}

ULONG64 KernelWriteFile(
	IN HANDLE         hfile,    // �ļ����
	IN PLARGE_INTEGER Offset,   // �����￪ʼд��
	IN ULONG          ulLength, // д������ֽ�
	IN PVOID          pBuffer)  // ��д�������
{
	// 1. д���ļ�
	IO_STATUS_BLOCK StatusBlock = { 0 };
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	Status = ZwWriteFile(
		hfile,        // �ļ����
		NULL,         // �ź�״̬(һ��ΪNULL)
		NULL, NULL,   // ����
		&StatusBlock, // ���ܺ����Ĳ������
		pBuffer,      // ��д�������
		ulLength,     // ��Ҫд��ĳ���
		Offset,       // д�����ʼƫ��
		NULL);        // һ��ΪNULL
	if (!NT_SUCCESS(Status))  return 0;
	// 2. ����ʵ��д��ĳ���
	// 2. ����ʵ��д��ĳ���
	return StatusBlock.Information;
}

NTSTATUS KernelDeleteFile(IN PUNICODE_STRING pstrFile)
{
	// 1. ��ʼ��OBJECT_ATTRIBUTES������
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes =
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // ���س�ʼ����ϵĽṹ��
		pstrFile,      // �ļ���������
		ulAttributes,  // ��������
		NULL,          // ��Ŀ¼(һ��ΪNULL)
		NULL);         // ��ȫ����(һ��ΪNULL)
					   // 2. ɾ��ָ���ļ�/�ļ���
	return ZwDeleteFile(&objAttrib);
}

VOID ZwMyCopyFile(
	PUNICODE_STRING  SouPath,//Դ��ַ
	PUNICODE_STRING DenPath  //Ŀ�ĵ�ַ
)
{

	//1 ��Դ��ַ�ļ�
	HANDLE hSorHandle = KernelCreateFile(SouPath, FALSE);
	//2 ��ȡ��С
	ULONG64 FileSize = KernelGetFileSize(hSorHandle);
	//3 ����ռ䣬��ȡ����
	PVOID buf = ExAllocatePool(NonPagedPool, (SIZE_T)FileSize);
	RtlZeroMemory(buf, (SIZE_T)FileSize);
	LARGE_INTEGER Offset = {0,0};
	KernelReadFile(hSorHandle, &Offset, (SIZE_T)FileSize, buf);
	//4 ��Ŀ�ĵ�ַ�ļ�
	HANDLE hDenHandle = KernelCreateFile(DenPath, FALSE);
	//5 д������
	KernelWriteFile(hDenHandle, &Offset, (SIZE_T)FileSize, buf);
	//6 �رվ��
	ZwClose(hSorHandle);
	ZwClose(hDenHandle);
}

BOOLEAN KernelFindFirstFile(
	IN  HANDLE                     hFile, // �ļ����
	IN  ULONG                      ulLen, // ��Ϣ����
	OUT PFILE_BOTH_DIR_INFORMATION  pDir, // �ļ���Ϣ
	IN  ULONG                      uFirstlLen, // ��Ϣ����
	OUT PFILE_BOTH_DIR_INFORMATION pFirstDir // ��һ���ļ���Ϣ
) {
	NTSTATUS                   Status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK            StatusBlock = { 0 };
	// 1. ��ȡ��һ���ļ���Ϣ�����Ƿ�ɹ�
	Status = ZwQueryDirectoryFile(
		hFile, NULL, NULL, NULL,// �ļ����
		&StatusBlock, // ���ܺ����Ĳ������
		pFirstDir,         // �ļ���Ϣ
		uFirstlLen,        // ���ļ���Ϣ�������ݳ���
		FileBothDirectoryInformation, // ��ѯģʽ
		TRUE,              // �Ƿ񷵻�һ����ʼ��Ϣ
		NULL,              // �ļ����ָ����ļ�(һ��ΪNULL)
		FALSE);            // �Ƿ��Ŀ¼��ʼ�ĵ�һ��ɨ��
						   // 2. ���ɹ������ȡ�ļ��б�
	if (NT_SUCCESS(Status) == FALSE) {
		return FALSE;
	}
	Status = ZwQueryDirectoryFile(
		hFile, NULL, NULL, NULL,// �ļ����
		&StatusBlock, // ���ܺ����Ĳ������
		pDir,         // �ļ���Ϣ
		ulLen,        // ���ļ���Ϣ�������ݳ���
		FileBothDirectoryInformation, // ��ѯģʽ
		FALSE,        // �Ƿ񷵻�һ����ʼ��Ϣ
		NULL,         // �ļ����ָ����ļ�(һ��ΪNULL)
		FALSE);       // �Ƿ��Ŀ¼��ʼ�ĵ�һ��ɨ��
	return NT_SUCCESS(Status);
}

BOOLEAN KernelFindNextFile(
	IN  PFILE_BOTH_DIR_INFORMATION pDirList,//
	OUT PFILE_BOTH_DIR_INFORMATION pDirInfo,
	IN OUT LONG * Loc) {
	// �������һ����ƶ�ָ��ָ����һ��
	PFILE_BOTH_DIR_INFORMATION pDir = (PFILE_BOTH_DIR_INFORMATION)((PCHAR)pDirList + *Loc);
	LONG StructLenth = 0;
	if (pDir->FileName[0] != 0)
	{
		StructLenth = sizeof(FILE_BOTH_DIR_INFORMATION);
		memcpy(pDirInfo, pDir, StructLenth + pDir->FileNameLength);
		*Loc = *Loc + pDir->NextEntryOffset;
		if (pDir->NextEntryOffset == 0) {
			*Loc = *Loc + StructLenth + pDir->FileNameLength;
		}
		return TRUE;
	}
	return FALSE;
}
NTSTATUS EnmuFile()
{
	UNICODE_STRING ustrFolder = { 0 };
	WCHAR          szSymbol[0x512] = L"\\??\\";
	UNICODE_STRING ustrPath =
		RTL_CONSTANT_STRING(L"C:\\");
	HANDLE         hFile = NULL;
	SIZE_T         nFileInfoSize =
		sizeof(FILE_BOTH_DIR_INFORMATION) + 270 * sizeof(WCHAR);
	SIZE_T         nSize = nFileInfoSize * 0x256; //���������0x256���ļ�
	char           strFileName[0x256] = { 0 };
	PFILE_BOTH_DIR_INFORMATION pFileTemp = NULL;
	PFILE_BOTH_DIR_INFORMATION pFileList = NULL;
	pFileList = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePool(PagedPool, nSize);
	pFileTemp = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePool(PagedPool,
		nFileInfoSize);
	// 1. ��·����װΪ���ӷ������������ļ�
	wcscat_s(szSymbol, _countof(szSymbol), ustrPath.Buffer);
	RtlInitUnicodeString(&ustrFolder, szSymbol);
	hFile = KernelCreateFile(&ustrFolder, TRUE);
	if (KernelFindFirstFile(hFile, nSize, pFileList, nFileInfoSize, pFileTemp))
	{
		LONG Loc = 0;
		do
		{
			RtlZeroMemory(strFileName, 0x256);
			RtlCopyMemory(strFileName,
				pFileTemp->FileName,
				pFileTemp->FileNameLength);
			if (strcmp(strFileName, "..") == 0
				|| strcmp(strFileName, ".") == 0)
				continue;
			if (pFileTemp->FileAttributes
				& FILE_ATTRIBUTE_DIRECTORY)
				DbgPrint("[Ŀ¼]%S\n", strFileName);
			else
				DbgPrint("[�ļ�]%S\n", strFileName);
			memset(pFileTemp, 0, nFileInfoSize);
		} while (KernelFindNextFile(pFileList, pFileTemp, &Loc));
	}
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint(("Enter  Driver"));
	DbgBreakPoint();
	//UNICODE_STRING SorPath;
	//UNICODE_STRING DenPath;
	//RtlInitUnicodeString(&SorPath, L"\\??\\D:\\test1\\123.txt");
	//RtlInitUnicodeString(&DenPath, L"\\??\\D:\\Test2\\456.txt");
	//ZwMyCopyFile(&SorPath, &DenPath);
	EnmuFile();
	return STATUS_SUCCESS;
}

//�����������д������Դ������
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint(("Leave  Driver"));
}