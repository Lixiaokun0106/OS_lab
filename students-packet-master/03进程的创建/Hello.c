/*
提供该示例代码是为了阐释一个概念，或者进行一个测试，并不代表着
最安全的编码实践，因此不应在应用程序或网站中使用该示例代码。对
于超出本示例代码的预期用途以外的使用所造成的偶然或继发性损失，
北京英真时代科技有限公司不承担任何责任。
*/

#include "EOSApp.h"


int main(int argc, char* argv[])
{
	int i;
	
	for (i = 1; i <= 5; i++) {
		
		printf("Hello,world! %d\n", i);
		
		Sleep(1000);
	}
	
	printf("Bye-bye!\n");
	
	return 0;
}
