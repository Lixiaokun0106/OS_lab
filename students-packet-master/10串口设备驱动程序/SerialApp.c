/*
提供该示例代码是为了阐释一个概念，或者进行一个测试，并不代表着
最安全的编码实践，因此不应在应用程序或网站中使用该示例代码。对
于超出本示例代码的预期用途以外的使用所造成的偶然或继发性损失，
北京英真时代科技有限公司不承担任何责任。
*/

#include "EOSApp.h"


int main(int argc, char* argv[])
{
	//
	// 启动调试 EOS 应用程序前要特别注意下面的问题：
	//
	// 1、如果要在调试应用程序时能够调试进入内核并显示对应的源码，
	//    必须使用 EOS 核心项目编译生成完全版本的 SDK 文件夹，然
	//    后使用此文件夹覆盖应用程序项目中的 SDK 文件夹，并且 EOS
	//    核心项目在磁盘上的位置不能改变。
	//

	static char Buffer[512]; // EOS 的栈空间有限，使用 static 关键字就不从栈上分配内存了。
	int c;
	HANDLE hCom1;
	ULONG nLength;
	ULONG nResult;	
	
	printf("This program is used to test serial port, and it's used with SerialChat.exe, make sure SerialChat.exe is running on Windows.\n");

	//
	// 以可读、可写的方式打开串口设备 COM1
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
		// 读取从控制台输入的字符串。
		//
		printf(">>");
		gets(Buffer);
		nLength = strlen(Buffer);

		//
		// 将从控制台输入的字符串发送到串口设备（字符串结束符'\0'也发送）。
		//
		if (!WriteFile(hCom1, Buffer, nLength + 1, &nResult)) {
			printf("WriteFile error:%d\n", GetLastError());
			break;
		}

		//
		// 如果输入的是“exit”则退出程序。
		//
		if (0 == stricmp(Buffer, "exit")) {
			break;
		}

		//
		// 从串口设备逐个接收字符并显示到控制台，直到收到字符串结束符号'\0'为止。
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
	// 关闭串口设备 COM1
	//
	CloseHandle(hCom1);
	
	return 0;
}
