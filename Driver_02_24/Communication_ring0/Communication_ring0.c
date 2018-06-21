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
	//1 创建一个设备
	IoCreateDevice(
		pDriver,            //驱动对象指针
		0,                  //设备扩展大小，传0
		&strDeviceName,     //设备名称
		FILE_DEVICE_UNKNOWN,//设备类型
		0,                  //设备特征
		FALSE,              //设备是否独占
		&pDevice            //传出创建好的设备指针
	);
	//1.2 设置设备的读写方式
	pDevice->Flags |= DO_BUFFERED_IO;//缓冲区读写方式
	//             |= DO_DIRECT_IO;  //直接读写方式，使用MDL重映射报证安全
	;



	//2 给设备绑定一个符号链接
	//设备名只能给内核层使用，在用户层想要访问设备，使用的是符号链接名
	IoCreateSymbolicLink(&strSymbolicName, &strDeviceName);
	//3 填写IRP处理函数
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriver->MajorFunction[i] = DefaultProc;
	}
	pDriver->MajorFunction[IRP_MJ_READ] = ReadProc;
	pDriver->MajorFunction[IRP_MJ_WRITE]= WriteProc;
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}

//在这个函数中写的是资源的清理
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	KdPrint(("Leave  Driver"));
	//1 删除符号链接
	UNICODE_STRING strSymbolicName;
	RtlInitUnicodeString(&strSymbolicName, L"\\DosDevices\\Hello");
	IoDeleteSymbolicLink(&strSymbolicName);
	//2 删除设备
	IoDeleteDevice(pDriver->DeviceObject);
}

NTSTATUS DefaultProc(
	DEVICE_OBJECT *objDeivce,
	IRP *pIrp
)
{
	UNREFERENCED_PARAMETER(objDeivce);
	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);

	switch (pStack->MajorFunction)//这个字段是IRP的类型码
	{
	case IRP_MJ_READ:

	case IRP_MJ_WRITE:
		//...
	}




	// 设置IRP完成状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// 设置IRP操作了多少字节
	pIrp->IoStatus.Information = 0;
	// 处理IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}

NTSTATUS ReadProc(
	DEVICE_OBJECT *objDeivce,
	IRP *pIrp
)
{
	UNREFERENCED_PARAMETER(objDeivce);
	//获取缓冲区
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
	//向缓冲区中写数据
	RtlCopyMemory(buf, L"Hello15PB",20);



	// 设置IRP完成状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// 设置IRP操作了多少字节
	pIrp->IoStatus.Information = 20;
	// 处理IRP
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
	//获得缓冲区大小
	PIO_STACK_LOCATION pStack =  IoGetCurrentIrpStackLocation(pIrp);
	ULONG uBufSize = pStack->Parameters.Write.Length;

	//获取缓冲区
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
	// 设置IRP完成状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// 设置IRP操作了多少字节
	pIrp->IoStatus.Information = 0;
	// 处理IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}