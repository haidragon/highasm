#include <ntifs.h>
//������
void RegCreateKey(LPWSTR KeyName) {
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING usKeyName;
	NTSTATUS ntStatus;
	HANDLE hRegister;
	RtlInitUnicodeString(&usKeyName, KeyName);
	InitializeObjectAttributes(&objectAttributes,
		&usKeyName,
		OBJ_CASE_INSENSITIVE,//�Դ�Сд����
		NULL,
		NULL);
	ntStatus = ZwCreateKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes,
		0, NULL, REG_OPTION_NON_VOLATILE, NULL);
	if (NT_SUCCESS(ntStatus)) {
		ZwClose(hRegister);
		DbgPrint("ZwCreateKey success!\n");
	}
	else
		DbgPrint("ZwCreateKey failed!\n");
}
//ɾ����
void RegDeleteKey(LPWSTR KeyName) {
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING usKeyName;
	NTSTATUS ntStatus;
	HANDLE hRegister;
	RtlInitUnicodeString(&usKeyName, KeyName);
	InitializeObjectAttributes(&objectAttributes,
		&usKeyName,
		OBJ_CASE_INSENSITIVE,//�Դ�Сд����
		NULL,
		NULL);
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus)) {
		ntStatus = ZwDeleteKey(hRegister);
		ZwClose(hRegister);
		DbgPrint("ZwDeleteKey success!\n");
	}
	else
		DbgPrint("ZwDeleteKey failed!\n");
}
//���ü�ֵ
void RegSetValueKey(LPWSTR KeyName, LPWSTR ValueName, DWORD DataType, PVOID DataBuffer, DWORD DataLength) {
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING usKeyName, usValueName;
	NTSTATUS ntStatus;
	HANDLE hRegister;
	ULONG Type;
	RtlInitUnicodeString(&usKeyName, KeyName);
	RtlInitUnicodeString(&usValueName, ValueName);
	InitializeObjectAttributes(&objectAttributes,
		&usKeyName,
		OBJ_CASE_INSENSITIVE,//�Դ�Сд����
		NULL,
		NULL);
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ZwSetValueKey(hRegister, &usValueName, 0, DataType, DataBuffer, DataLength);
		ZwFlushKey(hRegister);
		ZwClose(hRegister);
		DbgPrint("ZwSetValueKey success!\n");
	}
	else
		DbgPrint("ZwSetValueKey failed!\n");
}
//��ȡ��ֵ
NTSTATUS RegQueryValueKey(LPWSTR KeyName, LPWSTR ValueName,
	PKEY_VALUE_PARTIAL_INFORMATION *pkvpi)
{
	ULONG ulSize;
	NTSTATUS ntStatus;
	PKEY_VALUE_PARTIAL_INFORMATION pvpi;
	OBJECT_ATTRIBUTES objectAttributes;
	HANDLE hRegister;
	UNICODE_STRING usKeyName;
	UNICODE_STRING usValueName;
	RtlInitUnicodeString(&usKeyName, KeyName);
	RtlInitUnicodeString(&usValueName, ValueName);
	InitializeObjectAttributes(&objectAttributes,
		&usKeyName,
		OBJ_CASE_INSENSITIVE,//�Դ�Сд����
		NULL,
		NULL);
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);

	ntStatus = ZwQueryValueKey(hRegister,
		&usValueName,
		KeyValuePartialInformation,
		NULL,
		0,
		&ulSize);
	if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND || ulSize == 0)
	{
		DbgPrint("ZwQueryValueKey 1 failed!\n");
		return STATUS_UNSUCCESSFUL;
	}
	pvpi = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePool(PagedPool, ulSize);
	ntStatus = ZwQueryValueKey(hRegister,
		&usValueName,
		KeyValuePartialInformation,
		pvpi,
		ulSize,
		&ulSize);
	if (!NT_SUCCESS(ntStatus))
	{
		DbgPrint("ZwQueryValueKey 2 failed!\n");
		return STATUS_UNSUCCESSFUL;
	}
	//�����pvpi��û���ͷŵ�����Ҫ�ͷš�
	ExFreePool(pvpi);
	*pkvpi = pvpi;
	DbgPrint("ZwQueryValueKey success!\n");
	return STATUS_SUCCESS;
}

//ɾ����ֵ
void RegDeleteValueKey(LPWSTR KeyName, LPWSTR ValueName) {
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING usKeyName, usValueName;
	NTSTATUS ntStatus;
	HANDLE hRegister;
	RtlInitUnicodeString(&usKeyName, KeyName);
	RtlInitUnicodeString(&usValueName, ValueName);
	InitializeObjectAttributes(&objectAttributes,
		&usKeyName,
		OBJ_CASE_INSENSITIVE,//�Դ�Сд����
		NULL,
		NULL);
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ZwDeleteValueKey(hRegister, &usValueName);
		ZwFlushKey(hRegister);
		ZwClose(hRegister);
		DbgPrint("ZwDeleteValueKey success!\n");
	}
	else
		DbgPrint("ZwDeleteValueKey failed!\n");
}
//ö���ӽ�
VOID EnumSubKeyTest() {
	WCHAR MY_KEY_NAME[] = L"\\Registry\\Machine\\Software";
	UNICODE_STRING RegUnicodeString;
	HANDLE hRegister;
	OBJECT_ATTRIBUTES objectAttributes;
	NTSTATUS ntStatus;
	ULONG ulSize, i;
	UNICODE_STRING uniKeyName;
	PKEY_FULL_INFORMATION pfi;
	//��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(&RegUnicodeString, MY_KEY_NAME);
	//��ʼ��objectAttributes
	InitializeObjectAttributes(&objectAttributes,
		&RegUnicodeString,
		OBJ_CASE_INSENSITIVE,//�Դ�Сд����   
		NULL, NULL );
							 //��ע���

		ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus)) {
		DbgPrint("Open register successfully\n");
	}
	if (NT_SUCCESS(ntStatus))
	{
		DbgPrint("Open register successfully\n");
	}
	//��һ�ε���ZwQueryKeyΪ�˻�ȡKEY_FULL_INFORMATION���ݵĳ���
	ZwQueryKey(hRegister, KeyFullInformation, NULL, 0, &ulSize);
	pfi = (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);
	//�ڶ��ε���ZwQueryKeyΪ�˻�ȡKEY_FULL_INFORMATION���ݵ�����
	ZwQueryKey(hRegister, KeyFullInformation, pfi, ulSize, &ulSize);
	for (i = 0; i < pfi->SubKeys; i++)
	{
		PKEY_BASIC_INFORMATION pbi;
		//��һ�ε���ZwEnumerateKeyΪ�˻�ȡKEY_BASIC_INFORMATION���ݵĳ���
		ZwEnumerateKey(hRegister, i, KeyBasicInformation, NULL, 0, &ulSize);
		pbi = (PKEY_BASIC_INFORMATION)ExAllocatePool(PagedPool, ulSize);
		//�ڶ��ε���ZwEnumerateKeyΪ�˻�ȡKEY_BASIC_INFORMATION���ݵ�����
		ZwEnumerateKey(hRegister, i, KeyBasicInformation, pbi, ulSize, &ulSize);
		uniKeyName.Length = (USHORT)pbi->NameLength;
		uniKeyName.MaximumLength = (USHORT)pbi->NameLength;
		uniKeyName.Buffer = pbi->Name;
		DbgPrint("The %d sub item name:%wZ\n", i, &uniKeyName);
		ExFreePool(pbi);
	}
	ExFreePool(pfi);
	ZwClose(hRegister);
}
//ö���ӽ�ֵ
VOID EnumSubValueTest()
{
	WCHAR MY_KEY_NAME[] = L"\\Registry\\Machine\\Software\\Microsoft\\.NETFramework";
	UNICODE_STRING RegUnicodeString;
	HANDLE hRegister;
	OBJECT_ATTRIBUTES objectAttributes;
	ULONG ulSize, i;
	UNICODE_STRING uniKeyName;
	PKEY_FULL_INFORMATION pfi;
	NTSTATUS ntStatus;
	//��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(&RegUnicodeString, MY_KEY_NAME);
	//��ʼ��objectAttributes
	InitializeObjectAttributes(&objectAttributes,
		&RegUnicodeString,

		OBJ_CASE_INSENSITIVE,//�Դ�Сд����
		NULL,
		NULL);
	//��ע���
	ntStatus = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(ntStatus))
	{
		DbgPrint("Open register successfully\n");
	}
	//��ѯVALUE�Ĵ�С
	ZwQueryKey(hRegister, KeyFullInformation, NULL, 0, &ulSize);
	pfi = (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);
	ZwQueryKey(hRegister, KeyFullInformation, pfi, ulSize, &ulSize);

	for (i = 0; i < pfi->Values; i++)
	{
		PKEY_VALUE_BASIC_INFORMATION pvbi;
		//��ѯ����VALUE�Ĵ�С
		ZwEnumerateValueKey(hRegister, i, KeyValueBasicInformation, NULL, 0, &ulSize);
		pvbi = (PKEY_VALUE_BASIC_INFORMATION)ExAllocatePool(PagedPool, ulSize);
		//��ѯ����VALUE������
		ZwEnumerateValueKey(hRegister, i, KeyValueBasicInformation, pvbi, ulSize, &ulSize);
		uniKeyName.Length = (USHORT)pvbi->NameLength;
		uniKeyName.MaximumLength = (USHORT)pvbi->NameLength;
		uniKeyName.Buffer = pvbi->Name;
		DbgPrint("The %d sub value name:%wZ\n", i, &uniKeyName);
		if (pvbi->Type == REG_SZ)
			DbgPrint("The sub value type:REG_SZ\n");
		else if (pvbi->Type == REG_MULTI_SZ)
			DbgPrint("The sub value type:REG_MULTI_SZ\n");
		else if (pvbi->Type == REG_DWORD)
			DbgPrint("The sub value type:REG_DWORD\n");
		else if (pvbi->Type == REG_BINARY)
			DbgPrint("The sub value type:REG_BINARY\n");
		ExFreePool(pvbi);
	}
	ExFreePool(pfi);
	ZwClose(hRegister);
}
