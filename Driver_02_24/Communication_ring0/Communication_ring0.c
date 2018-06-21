#include <ntddk.h>
VOID DriverUnload(PDRIVER_OBJECT pDriver);
NTSTATUS DefaultProc(DEVICE_OBJECT *DeviceObject, IRP *Irp);
NTSTATUS ReadProc(DEVICE_OBJECT *objDeivce, IRP *pIrp);
NTSTATUS WriteProc(DEVICE_OBJECT *objDeivce, IRP *pIrp);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	KdPrint(("Enter  Driver"));
	DbgBreakPoint();
	PDEVICE_OBJECT pDevice = NULL;
	UNICODE_STRING strDeviceName;
	UNICODE_STRING strSymbolicName;
	RtlInitUnicodeString(&strDeviceName, L"\\Device\\Hello");
	RtlInitUnicodeString(&strSymbolicName, L"\\DosDevices\\Hello");
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
	//1.2 �����豸�Ķ�д��ʽ
	pDevice->Flags |= DO_BUFFERED_IO;//��������д��ʽ
	//             |= DO_DIRECT_IO;  //ֱ�Ӷ�д��ʽ��ʹ��MDL��ӳ�䱨֤��ȫ
	;



	//2 ���豸��һ����������
	//�豸��ֻ�ܸ��ں˲�ʹ�ã����û�����Ҫ�����豸��ʹ�õ��Ƿ���������
	IoCreateSymbolicLink(&strSymbolicName, &strDeviceName);
	//3 ��дIRP������
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriver->MajorFunction[i] = DefaultProc;
	}
	pDriver->MajorFunction[IRP_MJ_READ] = ReadProc;
	pDriver->MajorFunction[IRP_MJ_WRITE]= WriteProc;
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}

//�����������д������Դ������
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	KdPrint(("Leave  Driver"));
	//1 ɾ����������
	UNICODE_STRING strSymbolicName;
	RtlInitUnicodeString(&strSymbolicName, L"\\DosDevices\\Hello");
	IoDeleteSymbolicLink(&strSymbolicName);
	//2 ɾ���豸
	IoDeleteDevice(pDriver->DeviceObject);
}

NTSTATUS DefaultProc(
	DEVICE_OBJECT *objDeivce,
	IRP *pIrp
)
{
	UNREFERENCED_PARAMETER(objDeivce);
	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);

	switch (pStack->MajorFunction)//����ֶ���IRP��������
	{
	case IRP_MJ_READ:

	case IRP_MJ_WRITE:
		//...
	}




	// ����IRP���״̬
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// ����IRP�����˶����ֽ�
	pIrp->IoStatus.Information = 0;
	// ����IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}

NTSTATUS ReadProc(
	DEVICE_OBJECT *objDeivce,
	IRP *pIrp
)
{
	UNREFERENCED_PARAMETER(objDeivce);
	//��ȡ������
	UCHAR *buf = NULL;
	if (pIrp->UserBuffer != NULL)
	{
		buf = pIrp->UserBuffer;
	}
	if (pIrp->AssociatedIrp.SystemBuffer != NULL)
	{
		buf = pIrp->AssociatedIrp.SystemBuffer;
	}
	if (pIrp->MdlAddress != NULL)
	{
		buf = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
	}
	//�򻺳�����д����
	RtlCopyMemory(buf, L"Hello15PB",20);



	// ����IRP���״̬
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// ����IRP�����˶����ֽ�
	pIrp->IoStatus.Information = 20;
	// ����IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}

NTSTATUS WriteProc(
	DEVICE_OBJECT *objDeivce,
	IRP *pIrp
)
{
	UNREFERENCED_PARAMETER(objDeivce);
	UCHAR *buf=NULL;
	//��û�������С
	PIO_STACK_LOCATION pStack =  IoGetCurrentIrpStackLocation(pIrp);
	ULONG uBufSize = pStack->Parameters.Write.Length;

	//��ȡ������
	if (pIrp->UserBuffer!=NULL)
	{
		buf = pIrp->UserBuffer;
	}
	if (pIrp->AssociatedIrp.SystemBuffer!=NULL)
	{
		buf = pIrp->AssociatedIrp.SystemBuffer;
	}
	if (pIrp->MdlAddress!=NULL)
	{
		buf = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
	}
	KdPrint(("%S", buf));
	// ����IRP���״̬
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// ����IRP�����˶����ֽ�
	pIrp->IoStatus.Information = 0;
	// ����IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}