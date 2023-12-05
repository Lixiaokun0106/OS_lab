/*
提供该示例代码是为了阐释一个概念，或者进行一个测试，并不代表着
最安全的编码实践，因此不应在应用程序或网站中使用该示例代码。对
于超出本示例代码的预期用途以外的使用所造成的偶然或继发性损失，
北京英真时代科技有限公司不承担任何责任。
*/

#include "EOSApp.h"


//
// 线程函数
//
ULONG AppThread(PVOID Param);

//
// main 函数参数的意义：
// argc - argv 数组的长度，大小至少为 1，argc - 1 为命令行参数的数量。
// argv - 字符串指针数组，数组长度为命令行参数个数 + 1。其中 argv[0] 固定指向当前
//        进程所执行的可执行文件的路径字符串，argv[1] 及其后面的指针指向各个命令行
//        参数。
//
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
	
	HANDLE AppThreadHandle;


	//
	// 创建线程。
	//
	AppThreadHandle = CreateThread( 0,			// 默认堆栈大小
								   AppThread,	// 线程函数入口地址
								   NULL,		// 线程函数参数
								   0,			// 创建标志
								   NULL );		// 线程 ID

	if (NULL == AppThreadHandle) {
		return 4;
	}


	WaitForSingleObject(AppThreadHandle, INFINITE);

	CloseHandle(AppThreadHandle);

	return 0;
}

//
// 线程函数。
//
ULONG AppThread(PVOID Param) 
{
	printf("Hello world");
	
	return 0;
}


