/*
提供该示例代码是为了阐释一个概念，或者进行一个测试，并不代表着
最安全的编码实践，因此不应在应用程序或网站中使用该示例代码。对
于超出本示例代码的预期用途以外的使用所造成的偶然或继发性损失，
北京英真时代科技有限公司不承担任何责任。
*/

#include "EOSApp.h"

/*
	二级页表映射机制将物理内存分为 4K 大小的页，每页对应一个
	页框号（PFN: Page Frame Number），物理页 0x0-0x0FFF 对应
	的 PFN 为 0，0x1000-0x1FFF 对应的 PFN 为 1，依此类推。所
	以，将 PFN 左移 12 位可以获得物理页的基址。
	
	CR3 寄存器在其高 20 位中保存了页目录（Page Directory）的
	页框号。低 12 位保留未用。
	 
	 	   31                    12 11         0
	       +-----------------------+------------+
	   CR3 | PFN of Page Direcotry |  Reserved  |
	       +-----------------------+------------+
*/

#define CR3_SHIFT	12	// CR3 寄存器的值右移 12 位获得页目录的页框号

/*
	二级页表映射机制中的页目录和页表在进程的 4G 虚拟地址空间中的
	位置如下图
	
					   进程的 4G 虚拟地址空间
			----------+-----------------------+
			0x00000000|                       |
					  |                       |
					  |                       |
			 用户地址 |                       |
			   空间   |                       |
			 （低 2G）|                       |
					  |                       |
					  |                       |
					  |                       |
					  |                       |
			----------+-----------------------+
			0x80000000|                       |
					  +-----------------------+------------
					  |                       |0xC0000000
					  |                       |
			 内核地址 |                       |
			   空间   +-----------------------+ 1024 个页表（占用从
			 （高 2G）|  一个页目录（占用从   | 0xC0000000 起的 4M）
					  |  0xC0300000 起的 4K） | 
					  +-----------------------+
					  |                       |
					  +-----------------------+------------
			0xFFFFFFFF|                       |
			----------+-----------------------+
*/

#define PDE_BASE		((ULONG_PTR)0xC0300000)		// 页目录在虚拟地址空间的基址
#define PTE_BASE		((ULONG_PTR)0xC0000000)		// 页表在虚拟地址空间的基址

#define PTE_SIZE		0x4		// 页目录项和页表项都是 4 字节
#define PTE_PER_TABLE	0x400	// 页目录中有 1024 个 PDE，页表中有 1024 个 PTE
#define PAGE_SIZE 		0x1000	// 物理页、页目录、页表的大小都是 4096 字节

#define PDI_SHIFT 		22		// PDE 的标号左移 22 位获得虚拟地址的高 10 位
#define PTI_SHIFT 		12		// PTE 的标号左移 12 位获得虚拟地址的 22-12 位

/*
	页目录项（PDE: Page Directory Entry）和页表项（PTE: Page Table Entry）
	都是 4 个字节，各个位（只列出该程序关心的）的功能可以参考下图

					31                   12 11       1 0
					+----------------------+----------+-+
			PDE/PTE |         PFN          |          |P| 
					+----------------------+----------+-+
	        
	第 0 位为 0 时表示该项无效，为 1 时表示有效。高 20 位是 PDE/PTE 映射的
	页框号。
*/
typedef struct _MMPTE_HARDWARE
{
	ULONG Valid : 1;			// 存在位		0
	ULONG DoNotCare : 11;		// 不关心这些位	1-11
	ULONG PageFrameNumber : 20;	// 页框号		12-31
}MMPTE_HARDWARE, *PMMPTE_HARDWARE;

ULONG getcr3();		// 声明在汇编文件中实现的函数，使其可以在下面被调用。


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
	
	ULONG PfnOfPageDirectory;
	ULONG PageTotal = 0;
	ULONG IndexOfDirEntry;
	ULONG IndexOfTableEntry;
	PMMPTE_HARDWARE pPde;
	PMMPTE_HARDWARE pPte;
	ULONG_PTR PageTableBase;
	ULONG_PTR VirtualBase;
	
	const char* OutputFormat = NULL;

	// Sleep(10000);	// 等待 10 秒

	__asm("cli");	// 关中断

	//
	// 输出 CR3 中页目录的页框号
	//
	OutputFormat = "\nCR3->0x%X\n";
	PfnOfPageDirectory = (getcr3() >> CR3_SHIFT);
	printf(OutputFormat, PfnOfPageDirectory);

	//
	// 第一层循环，遍历页目录中的 PDE
	//
	for(IndexOfDirEntry = 0; IndexOfDirEntry < PTE_PER_TABLE; IndexOfDirEntry++)
	{
		pPde = (PMMPTE_HARDWARE)(PDE_BASE + IndexOfDirEntry * PTE_SIZE);

		//
		// 跳过无效的 PDE
		//
		if(!pPde->Valid)
			continue;
		
		//
		// 输出 PDE 信息，格式如下：
		// PDE: 标号 (映射的 4M 虚拟地址的基址)->所映射页表的页框号
		//
		OutputFormat = "PDE: 0x%X (0x%X)->0x%X\n";
		VirtualBase = (IndexOfDirEntry << PDI_SHIFT);
		printf(OutputFormat, IndexOfDirEntry, VirtualBase, pPde->PageFrameNumber);
		
		//
		// 根据 PDE 的标号计算其映射的页表所在虚拟地址的基址
		//
		PageTableBase = PTE_BASE + IndexOfDirEntry * PAGE_SIZE;
		
		//
		// 第二层循环，遍历页表中的 PTE
		//
		for(IndexOfTableEntry = 0; IndexOfTableEntry < PTE_PER_TABLE; IndexOfTableEntry++)
		{
			pPte = (PMMPTE_HARDWARE)(PageTableBase + IndexOfTableEntry * PTE_SIZE);
			
			//
			// 跳过无效的 PTE
			//
			if(!pPte->Valid)
				continue;
			
			//
			// 输出 PTE 信息，格式如下：
			// PTE: 标号 (映射的 4K 虚拟地址的基址)->所映射物理页的页框号
			//
			OutputFormat = "\t\tPTE: 0x%X (0x%X)->0x%X\n";
			VirtualBase = (IndexOfDirEntry << PDI_SHIFT) | (IndexOfTableEntry << PTI_SHIFT);
			printf(OutputFormat, IndexOfTableEntry, VirtualBase, pPte->PageFrameNumber);
			
			//
			// 统计占用的物理页数
			//
			PageTotal++;
		} // 第二层循环结束
	} // 第一层循环结束
	
	//
	// 输出占用的物理页数，和物理内存数
	//
	OutputFormat = "\nPhysical Page Total: %d\n";
	printf(OutputFormat, PageTotal);
	OutputFormat = "Physical Memory Total: %d\n\n";
	printf(OutputFormat, PageTotal * PAGE_SIZE);
	
	__asm("sti");	// 开中断
	
	// Sleep(60000);	// 等待 60 秒

	return 0;
}
