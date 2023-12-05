/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/


PRIVATE
VOID
ConsoleCmdPhysicalMemory(
	IN HANDLE StdHandle
	)
{
	BOOL IntState;
	ULONG_PTR PfnArray[1];

	IntState = KeEnableInterrupts(FALSE);	// ���ж�
	
	//
	// �������ҳ�����������ڴ����������ֽ�Ϊ��λ��
	//
	fprintf(StdHandle, "Page Count: %d.\n", MiTotalPageFrameCount);
	fprintf(StdHandle, "Memory Count: %d * %d = %d Byte.\n",
		MiTotalPageFrameCount, PAGE_SIZE,
		MiTotalPageFrameCount * PAGE_SIZE);
	
	//
	// �����ҳ�����Ϳ���ҳ����
	//
	fprintf(StdHandle, "\nZeroed Page Count: %d.\n", MiZeroedPageCount);
	fprintf(StdHandle, "Free Page Count: %d.\n", MiFreePageCount);
	
	//
	// �����ʹ�õ�����ҳ����
	//
	fprintf(StdHandle, "\nUsed Page Count: %d.\n", MiTotalPageFrameCount - MiZeroedPageCount - MiFreePageCount);
	
	
	//////////////////////////////////////////////////////////////////////////
	// ����һ������ҳ
	//
	MiAllocateAnyPages(1, PfnArray);
	
	fprintf(StdHandle, "\n****** After Allocate One Page ******\n");
	fprintf(StdHandle, "Zeroed Page Count: %d.\n", MiZeroedPageCount);
	fprintf(StdHandle, "Free Page Count: %d.\n", MiFreePageCount);
	fprintf(StdHandle, "Used Page Count: %d.\n", MiTotalPageFrameCount - MiZeroedPageCount - MiFreePageCount);
	
	
	//////////////////////////////////////////////////////////////////////////
	//
	// Ȼ�����ͷ��������ҳ
	//
	MiFreePages(1, PfnArray);
	
	fprintf(StdHandle, "\n****** After Free One Page ******\n");
	fprintf(StdHandle, "Zeroed Page Count: %d.\n", MiZeroedPageCount);
	fprintf(StdHandle, "Free Page Count: %d.\n", MiFreePageCount);
	fprintf(StdHandle, "Used Page Count: %d.\n", MiTotalPageFrameCount - MiZeroedPageCount - MiFreePageCount);
	
	KeEnableInterrupts(IntState);	// ���ж�
}
