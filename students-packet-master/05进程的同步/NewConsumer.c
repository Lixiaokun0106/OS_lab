/*
提供该示例代码是为了阐释一个概念，或者进行一个测试，并不代表着
最安全的编码实践，因此不应在应用程序或网站中使用该示例代码。对
于超出本示例代码的预期用途以外的使用所造成的偶然或继发性损失，
北京英真时代科技有限公司不承担任何责任。
*/

//
// 消费者线程函数。
//
ULONG Consumer(PVOID Param)
{
	int i;
	int OutIndex = 0;

	for (i = 0; i < PRODUCT_COUNT; i += 2) {

		while(WAIT_TIMEOUT == WaitForSingleObject(FullSemaphoreHandle, 300)){
			printf("Consumer wait for full semaphore timeout\n");
		}
		while(WAIT_TIMEOUT == WaitForSingleObject(FullSemaphoreHandle, 300)){
			printf("Consumer wait for full semaphore timeout\n");
		}
		WaitForSingleObject(MutexHandle, INFINITE);

		printf("\t\t\tConsume a %d\n", Buffer[OutIndex]);
		OutIndex = (OutIndex + 1) % BUFFER_SIZE;
		printf("\t\t\tConsume a %d\n", Buffer[OutIndex]);
		OutIndex = (OutIndex + 1) % BUFFER_SIZE;

		ReleaseMutex(MutexHandle);
		ReleaseSemaphore(EmptySemaphoreHandle, 2, NULL);

		//
		// 休息一会儿。让前 14 个数的消费速度比较慢，后面的较快。
		//
		if (i < 14) {
			Sleep(2000);
		} else {
			Sleep(100);
		}
	}
	
	return 0;
}
