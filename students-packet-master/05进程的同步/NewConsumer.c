/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/

//
// �������̺߳�����
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
		// ��Ϣһ�������ǰ 14 �����������ٶȱȽ���������ĽϿ졣
		//
		if (i < 14) {
			Sleep(2000);
		} else {
			Sleep(100);
		}
	}
	
	return 0;
}
