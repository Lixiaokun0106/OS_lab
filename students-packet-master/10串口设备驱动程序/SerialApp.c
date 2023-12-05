/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/

#include "EOSApp.h"


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

	static char Buffer[512]; // EOS ��ջ�ռ����ޣ�ʹ�� static �ؼ��־Ͳ���ջ�Ϸ����ڴ��ˡ�
	int c;
	HANDLE hCom1;
	ULONG nLength;
	ULONG nResult;	
	
	printf("This program is used to test serial port, and it's used with SerialChat.exe, make sure SerialChat.exe is running on Windows.\n");

	//
	// �Կɶ�����д�ķ�ʽ�򿪴����豸 COM1
	//
	hCom1 = CreateFile( "COM1",
						GENERIC_READ | GENERIC_WRITE,
						0,
						OPEN_EXISTING,
						0 );

	if (INVALID_HANDLE_VALUE == hCom1) {
		printf("CreateFile error:%d\n", GetLastError());
		return 1;
	}

	for (;;) {

		//
		// ��ȡ�ӿ���̨������ַ�����
		//
		printf(">>");
		gets(Buffer);
		nLength = strlen(Buffer);

		//
		// ���ӿ���̨������ַ������͵������豸���ַ���������'\0'Ҳ���ͣ���
		//
		if (!WriteFile(hCom1, Buffer, nLength + 1, &nResult)) {
			printf("WriteFile error:%d\n", GetLastError());
			break;
		}

		//
		// ���������ǡ�exit�����˳�����
		//
		if (0 == stricmp(Buffer, "exit")) {
			break;
		}

		//
		// �Ӵ����豸��������ַ�����ʾ������̨��ֱ���յ��ַ�����������'\0'Ϊֹ��
		//
		printf("<<");

		for (;;) {

			if (!ReadFile(hCom1, Buffer, 1, &nResult)) {
				printf("ReadFile error:%d\n", GetLastError());
				goto RETURN;
			}

			if ('\0' == *Buffer) {
				putchar('\n');
				break;
			}

			putchar(*Buffer);
		}
	}

RETURN:
	//
	// �رմ����豸 COM1
	//
	CloseHandle(hCom1);
	
	return 0;
}
