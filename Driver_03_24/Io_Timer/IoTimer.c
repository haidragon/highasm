#include <ntifs.h>

KEVENT g_kEvent;

VOID DriverUnload(PDRIVER_OBJECT pDriver);

VOID TimerProc(
	DEVICE_OBJECT *DeviceObject,
	PVOID Context
)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	static ULONG i = 0;
	i++;
	KdPrint(("�ⲿ�������%d ,�ڲ��仯��%d\n", Context, i));
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgBreakPoint();


	//��ʼ��һЩ����
	PDEVICE_OBJECT pDevice = NULL;
	UNICODE_STRING strDeviceName;
	RtlInitUnicodeString(&strDeviceName, L"\\Device\\Hello");
	//1 ����һ���豸
	IoCreateDevice(
		pDriver,            //��������ָ��
		0,                  //�豸��չ��С����0
		&strDeviceName,     //�豸����
		FILE_DEVICE_UNKNOWN,//�豸����
		0,                  //�豸����
		FALSE,              //�豸�Ƿ��ռ
		&pDevice            //���������õ��豸ָ��
	);

	// ��ʼ��һ��IO��ʱ��
	IoInitializeTimer(
		pDevice,
		TimerProc,
		(PVOID)100
	);

	//����IO��ʱ��
	IoStartTimer(pDevice);

	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	//ֹͣIO��ʱ��
	IoStopTimer(pDriver->DeviceObject);
	IoDeleteDevice(pDriver->DeviceObject);
}