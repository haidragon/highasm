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
	KdPrint(("外部传入的是%d ,内部变化：%d\n", Context, i));
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgBreakPoint();


	//初始化一些变量
	PDEVICE_OBJECT pDevice = NULL;
	UNICODE_STRING strDeviceName;
	RtlInitUnicodeString(&strDeviceName, L"\\Device\\Hello");
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

	// 初始化一个IO计时器
	IoInitializeTimer(
		pDevice,
		TimerProc,
		(PVOID)100
	);

	//启动IO计时器
	IoStartTimer(pDevice);

	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	//停止IO计时器
	IoStopTimer(pDriver->DeviceObject);
	IoDeleteDevice(pDriver->DeviceObject);
}