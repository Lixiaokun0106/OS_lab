/*
提供该示例代码是为了阐释一个概念，或者进行一个测试，并不代表着
最安全的编码实践，因此不应在应用程序或网站中使用该示例代码。对
于超出本示例代码的预期用途以外的使用所造成的偶然或继发性损失，
北京英真时代科技有限公司不承担任何责任。
*/


PRIVATE
VOID
ConsoleCmdPhysicalMemory(
	IN HANDLE StdHandle
	)
{
	BOOL IntState;
	ULONG_PTR PfnArray[1];

	IntState = KeEnableInterrupts(FALSE);	// 关中断
	
	//
	// 输出物理页数量和物理内存数量（以字节为单位）
	//
	fprintf(StdHandle, "Page Count: %d.\n", MiTotalPageFrameCount);
	fprintf(StdHandle, "Memory Count: %d * %d = %d Byte.\n",
		MiTotalPageFrameCount, PAGE_SIZE,
		MiTotalPageFrameCount * PAGE_SIZE);
	
	//
	// 输出零页数量和空闲页数量
	//
	fprintf(StdHandle, "\nZeroed Page Count: %d.\n", MiZeroedPageCount);
	fprintf(StdHandle, "Free Page Count: %d.\n", MiFreePageCount);
	
	//
	// 输出已使用的物理页数量
	//
	fprintf(StdHandle, "\nUsed Page Count: %d.\n", MiTotalPageFrameCount - MiZeroedPageCount - MiFreePageCount);
	
	
	//////////////////////////////////////////////////////////////////////////
	// 分配一个物理页
	//
	MiAllocateAnyPages(1, PfnArray);
	
	fprintf(StdHandle, "\n****** After Allocate One Page ******\n");
	fprintf(StdHandle, "Zeroed Page Count: %d.\n", MiZeroedPageCount);
	fprintf(StdHandle, "Free Page Count: %d.\n", MiFreePageCount);
	fprintf(StdHandle, "Used Page Count: %d.\n", MiTotalPageFrameCount - MiZeroedPageCount - MiFreePageCount);
	
	
	//////////////////////////////////////////////////////////////////////////
	//
	// 然后再释放这个物理页
	//
	MiFreePages(1, PfnArray);
	
	fprintf(StdHandle, "\n****** After Free One Page ******\n");
	fprintf(StdHandle, "Zeroed Page Count: %d.\n", MiZeroedPageCount);
	fprintf(StdHandle, "Free Page Count: %d.\n", MiFreePageCount);
	fprintf(StdHandle, "Used Page Count: %d.\n", MiTotalPageFrameCount - MiZeroedPageCount - MiFreePageCount);
	
	KeEnableInterrupts(IntState);	// 开中断
}
