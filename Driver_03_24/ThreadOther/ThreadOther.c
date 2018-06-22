#include <ntifs.h>
//获取操作系统版本
ULONG KernelGetVersion()
{
	ULONG NtBuildNumber;
	RTL_OSVERSIONINFOW osi =
	{ sizeof(RTL_OSVERSIONINFOW) };
	RtlGetVersion(&osi);
	return osi.dwBuildNumber;
}

//获取时间
VOID Test_GetCurrentTime()
{
	LARGE_INTEGER CurrentTime;
	LARGE_INTEGER LocalTime;
	TIME_FIELDS   TimeFiled;
	// 1. 获取格林威治时间
	KeQuerySystemTime(&CurrentTime);
	// 2. 转换成本地时间
	ExSystemTimeToLocalTime(&CurrentTime, &LocalTime);
	// 3. 转换为时间字段
	RtlTimeToTimeFields(&LocalTime, &TimeFiled);
	DbgPrint("Time : %4d-%2d-%2d %2d:%2d:%2d\n",
		TimeFiled.Year, TimeFiled.Month, TimeFiled.Day,
		TimeFiled.Hour, TimeFiled.Minute, TimeFiled.Second);
}

