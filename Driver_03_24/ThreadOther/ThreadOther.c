#include <ntifs.h>
//��ȡ����ϵͳ�汾
ULONG KernelGetVersion()
{
	ULONG NtBuildNumber;
	RTL_OSVERSIONINFOW osi =
	{ sizeof(RTL_OSVERSIONINFOW) };
	RtlGetVersion(&osi);
	return osi.dwBuildNumber;
}

//��ȡʱ��
VOID Test_GetCurrentTime()
{
	LARGE_INTEGER CurrentTime;
	LARGE_INTEGER LocalTime;
	TIME_FIELDS   TimeFiled;
	// 1. ��ȡ��������ʱ��
	KeQuerySystemTime(&CurrentTime);
	// 2. ת���ɱ���ʱ��
	ExSystemTimeToLocalTime(&CurrentTime, &LocalTime);
	// 3. ת��Ϊʱ���ֶ�
	RtlTimeToTimeFields(&LocalTime, &TimeFiled);
	DbgPrint("Time : %4d-%2d-%2d %2d:%2d:%2d\n",
		TimeFiled.Year, TimeFiled.Month, TimeFiled.Day,
		TimeFiled.Hour, TimeFiled.Minute, TimeFiled.Second);
}

