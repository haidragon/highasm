#include <ntifs.h>


VOID DriverUnload(PDRIVER_OBJECT pDriver);


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgBreakPoint();

	__try
	{
		int *p = 0;
		*p = 100;
	}
	except(1)
	{
		KdPrint(("����:�ӹ���һ���쳣"));
	}
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
}