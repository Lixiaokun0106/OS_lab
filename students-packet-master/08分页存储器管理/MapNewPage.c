/*
提供该示例代码是为了阐释一个概念，或者进行一个测试，并不代表着
最安全的编码实践，因此不应在应用程序或网站中使用该示例代码。对
于超出本示例代码的预期用途以外的使用所造成的偶然或继发性损失，
北京英真时代科技有限公司不承担任何责任。
*/

ULONG PfnArray[2];

//
// 访问未映射物理内存的虚拟地址会触发异常。
// 必须注释或者删除该行代码才能执行后面的代码。
//
*((PINT)0xE0000000) = 100;	
	
//
// 从内核申请两个未用的物理页。
// 由 PfnArray 数组返回两个物理页的页框号。
//
MiAllocateZeroedPages(2, PfnArray);
OutputFormat = "New page frame number: 0x%X, 0x%X\n";
fprintf(StdHandle, OutputFormat, PfnArray[0], PfnArray[1]);

//
// 使用 PfnArray[0] 页做为页表，映射基址为 0xE00000000 的 4M 虚拟地址。
//
IndexOfDirEntry = (0xE0000000 >> 22);	// 虚拟地址的高 10 位是 PDE 标号
((PMMPTE_HARDWARE)0xC0300000)[IndexOfDirEntry].PageFrameNumber = PfnArray[0];
((PMMPTE_HARDWARE)0xC0300000)[IndexOfDirEntry].Valid = 1;		// 有效
((PMMPTE_HARDWARE)0xC0300000)[IndexOfDirEntry].Writable = 1;	// 可写
MiFlushEntireTlb();	// 刷新快表

//
// 根据 PDE 的标号计算其映射的页表所在虚拟地址的基址
//
PageTableBase = 0xC0000000 + IndexOfDirEntry * PAGE_SIZE;

//
// 将 PfnArray[1] 放入页表 PfnArray[0] 的两个 PTE 中，
// 分别映射基址为 0xE0000000 和 0xE0001000 的 4K 虚拟地址
//
IndexOfTableEntry = (0xE0000000 >> 12) & 0x3FF;	// 虚拟地址的 12-22 位是 PTE 标号
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].PageFrameNumber = PfnArray[1];
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].Valid = 1;		// 有效
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].Writable = 1;	// 可写
MiFlushEntireTlb();	// 刷新快表

IndexOfTableEntry = (0xE0001000 >> 12) & 0x3FF;	// 虚拟地址的 12-22 位是 PTE 标号
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].PageFrameNumber = PfnArray[1];
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].Valid = 1;		// 有效
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].Writable = 1;	// 可写
MiFlushEntireTlb();	// 刷新快表

//
// 测试
//
OutputFormat = "Read Memory 0xE0001000: %d\n";
fprintf(StdHandle, OutputFormat, *((PINT)0xE0001000));
*((PINT)0xE0000000) = 100;	// 写共享内存
fprintf(StdHandle, OutputFormat, *((PINT)0xE0001000));
