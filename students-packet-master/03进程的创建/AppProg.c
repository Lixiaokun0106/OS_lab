/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/

#include "EOSApp.h"


//
// �̺߳���
//
ULONG AppThread(PVOID Param);

//
// main �������������壺
// argc - argv ����ĳ��ȣ���С����Ϊ 1��argc - 1 Ϊ�����в�����������
// argv - �ַ���ָ�����飬���鳤��Ϊ�����в������� + 1������ argv[0] �̶�ָ��ǰ
//        ������ִ�еĿ�ִ���ļ���·���ַ�����argv[1] ��������ָ��ָ�����������
//        ������
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
	
	HANDLE AppThreadHandle;


	//
	// �����̡߳�
	//
	AppThreadHandle = CreateThread( 0,			// Ĭ�϶�ջ��С
								   AppThread,	// �̺߳�����ڵ�ַ
								   NULL,		// �̺߳�������
								   0,			// ������־
								   NULL );		// �߳� ID

	if (NULL == AppThreadHandle) {
		return 4;
	}


	WaitForSingleObject(AppThreadHandle, INFINITE);

	CloseHandle(AppThreadHandle);

	return 0;
}

//
// �̺߳�����
//
ULONG AppThread(PVOID Param) 
{
	printf("Hello world");
	
	return 0;
}


