/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/

#include "EOSApp.h"


//
// main �������������壺
// argc - argv ����ĳ��ȣ���С����Ϊ 1��argc - 1 Ϊ�����в�����������
// argv - �ַ���ָ�����飬���鳤��Ϊ�����в������� + 1������ argv[0] �̶�ָ��ǰ
//        ������ִ�еĿ�ִ���ļ���·���ַ�����argv[1] ��������ָ��ָ�����������
//        ������
//        ����ͨ������������ "a:\hello.exe -a -b" �������̺�hello.exe �� main ��
//        ���Ĳ��� argc ��ֵΪ 3��argv[0] ָ���ַ��� "a:\hello.exe"��argv[1] ָ��
//        �����ַ��� "-a"��argv[2] ָ������ַ��� "-b"��
//
int main(int argc, char* argv[])
{
	//
	// �������� EOS Ӧ�ó���ǰҪ�ر�ע����������⣺
	//
	// 1�����Ҫ�ڵ���Ӧ�ó���ʱ�ܹ����Խ����ں˲���ʾ��Ӧ��Դ�룬
	//    ����ʹ�� EOS ������Ŀ����������ȫ�汾�� SDK �ļ��У�Ȼ
	//    ��ʹ�ô��ļ��и���Ӧ�ó�����Ŀ�е� SDK �ļ��У����� EOS
	//    ������Ŀ�ڴ����ϵ�λ�ò��ܸı䡣
	//
	
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcInfoOne, ProcInfoTwo;
	ULONG ulExitCode;	// �ӽ����˳���
	INT nResult = 0;	// main ��������ֵ��0 ��ʾ�ɹ����� 0 ��ʾʧ�ܡ�

	printf("Create two processes and wait for the processes exit...\n\n");

	//
	// ʹ�ӽ��̺͸�����ʹ����ͬ�ı�׼�����
	//
	StartupInfo.StdInput = GetStdHandle(STD_INPUT_HANDLE);
	StartupInfo.StdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	StartupInfo.StdError = GetStdHandle(STD_ERROR_HANDLE);

	//
	// Ϊһ��Ӧ�ó���ͬʱ���������ӽ��̡�
	//
	if (CreateProcess("A:\\Hello.exe", NULL, 0, &StartupInfo, &ProcInfoOne)
		&& CreateProcess("A:\\Hello.exe", NULL, 0, &StartupInfo, &ProcInfoTwo)) {

		//
		// �����ӽ��̳ɹ����ȴ��ӽ������н�����
		//
		WaitForSingleObject(ProcInfoOne.ProcessHandle, INFINITE);
		WaitForSingleObject(ProcInfoTwo.ProcessHandle, INFINITE);

		//
		// �õ�������ӽ��̵��˳��롣
		//
		GetExitCodeProcess(ProcInfoOne.ProcessHandle, &ulExitCode);
		printf("\nThe process one exit with %d.\n", ulExitCode);
		GetExitCodeProcess(ProcInfoTwo.ProcessHandle, &ulExitCode);
		printf("\nThe process two exit with %d.\n", ulExitCode);

		//
		// �رղ���ʹ�õľ����
		//
		CloseHandle(ProcInfoOne.ProcessHandle);
		CloseHandle(ProcInfoOne.ThreadHandle);
		CloseHandle(ProcInfoTwo.ProcessHandle);
		CloseHandle(ProcInfoTwo.ThreadHandle);

	} else {

		printf("CreateProcess Failed, Error code: 0x%X.\n", GetLastError());
		nResult = 1;
	}

	return nResult;
}
