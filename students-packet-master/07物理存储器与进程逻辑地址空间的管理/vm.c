/*
提供该示例代码是为了阐释一个概念，或者进行一个测试，并不代表着
最安全的编码实践，因此不应在应用程序或网站中使用该示例代码。对
于超出本示例代码的预期用途以外的使用所造成的偶然或继发性损失，
北京英真时代科技有限公司不承担任何责任。
*/


PRIVATE
VOID
ConsoleCmdVM(
	IN HANDLE StdHandle,
	IN PCSTR Arg
	)
{
	BOOL IntState;
	ULONG ProcID;
	PPROCESS pProcCtrlBlock;
	PMMVAD_LIST pVadList;
	PLIST_ENTRY pListEntry;
	PMMVAD pVad;
	ULONG Index, TotalVpnCount, AllocatedVpnCount, FreeVpnCount, VpnCount;
	STATUS Status;
	
	const char* OutputFormat = NULL;
	
	//
	// 从命令参数字符串中获得进程 ID。
	//
	ProcID = atoi(Arg);
	if(0 == ProcID) {
		fprintf(StdHandle, "Please input a valid process ID.\n");
		return;
	}
	
	//
	// 由进程 ID 获得进程控制块
	//
	Status = ObRefObjectById(ProcID, PspProcessType, (PVOID*)&pProcCtrlBlock);
	if (!EOS_SUCCESS(Status)) {
		fprintf(StdHandle, "%d is an invalid process ID.\n", ProcID);
		return;
	}
	
	IntState = KeEnableInterrupts(FALSE);	// 关中断
	
	//
	// 将进程控制块中 VAD 链表的指针保存下来，方便后面使用
	//
	pVadList = &pProcCtrlBlock->Pas->VadList;
	
	//
	// 输出 VAD 链表中记录的起始页框号，结束页框号
	//
	OutputFormat = "Total Vpn from %d to %d. (0x%X - 0x%X)\n\n";
	fprintf(StdHandle, OutputFormat,
		pVadList->StartingVpn, pVadList->EndVpn,
		pVadList->StartingVpn * PAGE_SIZE, (pVadList->EndVpn + 1) * PAGE_SIZE - 1);
	
	//
	// 遍历 VAD 链表，输出所有 VAD 的起始页框号，结束页框号和包含的虚拟页框数量
	//
	Index = AllocatedVpnCount = 0;
	for(pListEntry = pVadList->VadListHead.Next;
		pListEntry != &pVadList->VadListHead;
		pListEntry = pListEntry->Next) {
	
		Index++;
		pVad = CONTAINING_RECORD(pListEntry, MMVAD, VadListEntry);
		
		VpnCount = pVad->EndVpn - pVad->StartingVpn + 1;
		OutputFormat = "%d# Vad Include %d Vpn From %d to %d. (0x%X - 0x%X)\n";
		fprintf(StdHandle, OutputFormat,
			Index, VpnCount, pVad->StartingVpn, pVad->EndVpn,
			pVad->StartingVpn * PAGE_SIZE, (pVad->EndVpn + 1) * PAGE_SIZE - 1);
		
		AllocatedVpnCount += VpnCount;
	}
	
	//
	// 统计虚拟页框总数、已分配的虚拟页框和未分配的虚拟页框
	//
	TotalVpnCount = pVadList->EndVpn - pVadList->StartingVpn + 1;
	OutputFormat = "\nTotal Vpn Count: %d.\n";
	fprintf(StdHandle, OutputFormat, TotalVpnCount);
	
	OutputFormat = "Allocated Vpn Count: %d.\n";
	fprintf(StdHandle, OutputFormat, AllocatedVpnCount);
	
	FreeVpnCount = TotalVpnCount - AllocatedVpnCount;
	OutputFormat = "Free Vpn Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, FreeVpnCount);
	
	//
	// 输出物理页的零页数量和空闲页数量
	//
	OutputFormat = "Zeroed Physical Page Count: %d.\n";
	fprintf(StdHandle, OutputFormat, MiZeroedPageCount);
	
	OutputFormat = "Free Physical Page Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, MiFreePageCount);
	
	
	//////////////////////////////////////////////////////////////////////////
	//
	// 分配一块新的虚拟内存。但是没有使用 MEM_COMMIT 标志为其分配物理页。
	//
	PVOID BaseAddress = 0;
	SIZE_T RegionSize = 1;	
	Status = MmAllocateVirtualMemory(&BaseAddress, &RegionSize, MEM_RESERVE, TRUE);
	if (!EOS_SUCCESS(Status)) {
		fprintf(StdHandle, "Allocate virtual memory at 0x%X faild.\n", BaseAddress);
		goto VM_RETURN;
	}
	
	//
	// 输出新分配的内存的基址和大小
	//
	OutputFormat = "New VM's base address: 0x%X. Size: 0x%X.\n\n";
	fprintf(StdHandle, OutputFormat, BaseAddress, RegionSize);
	
	//
	// 遍历 VAD 链表，输出所有 VAD 的起始页框号，结束页框号和包含的虚拟页框数量
	//
	Index = AllocatedVpnCount = 0;
	for(pListEntry = pVadList->VadListHead.Next;
		pListEntry != &pVadList->VadListHead;
		pListEntry = pListEntry->Next) {
	
		Index++;
		pVad = CONTAINING_RECORD(pListEntry, MMVAD, VadListEntry);
		
		VpnCount = pVad->EndVpn - pVad->StartingVpn + 1;
		OutputFormat = "%d# Vad Include %d Vpn From %d to %d. (0x%X - 0x%X)\n";
		fprintf(StdHandle, OutputFormat,
			Index, VpnCount, pVad->StartingVpn, pVad->EndVpn,
			pVad->StartingVpn * PAGE_SIZE, (pVad->EndVpn + 1) * PAGE_SIZE - 1);
		
		AllocatedVpnCount += VpnCount;
	}
	
	//
	// 统计已分配的虚拟页框和未分配的虚拟页框
	//
	OutputFormat = "\nAllocated Vpn Count: %d.\n";
	fprintf(StdHandle, OutputFormat, AllocatedVpnCount);
	
	FreeVpnCount = TotalVpnCount - AllocatedVpnCount;
	OutputFormat = "Free Vpn Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, FreeVpnCount);
	
	//
	// 输出物理页的零页数量和空闲页数量
	//
	OutputFormat = "Zeroed Physical Page Count: %d.\n";
	fprintf(StdHandle, OutputFormat, MiZeroedPageCount);
	
	OutputFormat = "Free Physical Page Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, MiFreePageCount);
	
	
	//////////////////////////////////////////////////////////////////////////
	//
	// 释放刚刚分配的虚拟内存。
	//
	RegionSize = 0;		// 所释放虚拟内存的大小必须赋值为 0
	MmFreeVirtualMemory(&BaseAddress, &RegionSize, MEM_RELEASE, TRUE);
	
	//
	// 输出释放的的虚拟内存的基址和大小
	//
	OutputFormat = "Free VM's base address: 0x%X. Size: 0x%X.\n\n";
	fprintf(StdHandle, OutputFormat, BaseAddress, RegionSize);
	
	//
	// 遍历 VAD 链表，输出所有 VAD 的起始页框号，结束页框号和包含的虚拟页框数量
	//
	Index = AllocatedVpnCount = 0;
	for(pListEntry = pVadList->VadListHead.Next;
		pListEntry != &pVadList->VadListHead;
		pListEntry = pListEntry->Next) {
	
		Index++;
		pVad = CONTAINING_RECORD(pListEntry, MMVAD, VadListEntry);
		
		VpnCount = pVad->EndVpn - pVad->StartingVpn + 1;
		OutputFormat = "%d# Vad Include %d Vpn From %d to %d. (0x%X - 0x%X)\n";
		fprintf(StdHandle, OutputFormat,
			Index, VpnCount, pVad->StartingVpn, pVad->EndVpn,
			pVad->StartingVpn * PAGE_SIZE, (pVad->EndVpn + 1) * PAGE_SIZE - 1);
		
		AllocatedVpnCount += VpnCount;
	}
	
	//
	// 统计已分配的虚拟页框和未分配的虚拟页框
	//
	OutputFormat = "\nAllocated Vpn Count: %d.\n";
	fprintf(StdHandle, OutputFormat, AllocatedVpnCount);
	
	FreeVpnCount = TotalVpnCount - AllocatedVpnCount;
	OutputFormat = "Free Vpn Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, FreeVpnCount);
	
	//
	// 输出物理页的零页数量和空闲页数量
	//
	OutputFormat = "Zeroed Physical Page Count: %d.\n";
	fprintf(StdHandle, OutputFormat, MiZeroedPageCount);
	
	OutputFormat = "Free Physical Page Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, MiFreePageCount);
	
VM_RETURN:	
	KeEnableInterrupts(IntState);	// 开中断
	
	ObDerefObject(pProcCtrlBlock);
}
