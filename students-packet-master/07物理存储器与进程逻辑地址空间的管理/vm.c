/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
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
	// ����������ַ����л�ý��� ID��
	//
	ProcID = atoi(Arg);
	if(0 == ProcID) {
		fprintf(StdHandle, "Please input a valid process ID.\n");
		return;
	}
	
	//
	// �ɽ��� ID ��ý��̿��ƿ�
	//
	Status = ObRefObjectById(ProcID, PspProcessType, (PVOID*)&pProcCtrlBlock);
	if (!EOS_SUCCESS(Status)) {
		fprintf(StdHandle, "%d is an invalid process ID.\n", ProcID);
		return;
	}
	
	IntState = KeEnableInterrupts(FALSE);	// ���ж�
	
	//
	// �����̿��ƿ��� VAD �����ָ�뱣���������������ʹ��
	//
	pVadList = &pProcCtrlBlock->Pas->VadList;
	
	//
	// ��� VAD �����м�¼����ʼҳ��ţ�����ҳ���
	//
	OutputFormat = "Total Vpn from %d to %d. (0x%X - 0x%X)\n\n";
	fprintf(StdHandle, OutputFormat,
		pVadList->StartingVpn, pVadList->EndVpn,
		pVadList->StartingVpn * PAGE_SIZE, (pVadList->EndVpn + 1) * PAGE_SIZE - 1);
	
	//
	// ���� VAD ����������� VAD ����ʼҳ��ţ�����ҳ��źͰ���������ҳ������
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
	// ͳ������ҳ���������ѷ��������ҳ���δ���������ҳ��
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
	// �������ҳ����ҳ�����Ϳ���ҳ����
	//
	OutputFormat = "Zeroed Physical Page Count: %d.\n";
	fprintf(StdHandle, OutputFormat, MiZeroedPageCount);
	
	OutputFormat = "Free Physical Page Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, MiFreePageCount);
	
	
	//////////////////////////////////////////////////////////////////////////
	//
	// ����һ���µ������ڴ档����û��ʹ�� MEM_COMMIT ��־Ϊ���������ҳ��
	//
	PVOID BaseAddress = 0;
	SIZE_T RegionSize = 1;	
	Status = MmAllocateVirtualMemory(&BaseAddress, &RegionSize, MEM_RESERVE, TRUE);
	if (!EOS_SUCCESS(Status)) {
		fprintf(StdHandle, "Allocate virtual memory at 0x%X faild.\n", BaseAddress);
		goto VM_RETURN;
	}
	
	//
	// ����·�����ڴ�Ļ�ַ�ʹ�С
	//
	OutputFormat = "New VM's base address: 0x%X. Size: 0x%X.\n\n";
	fprintf(StdHandle, OutputFormat, BaseAddress, RegionSize);
	
	//
	// ���� VAD ����������� VAD ����ʼҳ��ţ�����ҳ��źͰ���������ҳ������
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
	// ͳ���ѷ��������ҳ���δ���������ҳ��
	//
	OutputFormat = "\nAllocated Vpn Count: %d.\n";
	fprintf(StdHandle, OutputFormat, AllocatedVpnCount);
	
	FreeVpnCount = TotalVpnCount - AllocatedVpnCount;
	OutputFormat = "Free Vpn Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, FreeVpnCount);
	
	//
	// �������ҳ����ҳ�����Ϳ���ҳ����
	//
	OutputFormat = "Zeroed Physical Page Count: %d.\n";
	fprintf(StdHandle, OutputFormat, MiZeroedPageCount);
	
	OutputFormat = "Free Physical Page Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, MiFreePageCount);
	
	
	//////////////////////////////////////////////////////////////////////////
	//
	// �ͷŸոշ���������ڴ档
	//
	RegionSize = 0;		// ���ͷ������ڴ�Ĵ�С���븳ֵΪ 0
	MmFreeVirtualMemory(&BaseAddress, &RegionSize, MEM_RELEASE, TRUE);
	
	//
	// ����ͷŵĵ������ڴ�Ļ�ַ�ʹ�С
	//
	OutputFormat = "Free VM's base address: 0x%X. Size: 0x%X.\n\n";
	fprintf(StdHandle, OutputFormat, BaseAddress, RegionSize);
	
	//
	// ���� VAD ����������� VAD ����ʼҳ��ţ�����ҳ��źͰ���������ҳ������
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
	// ͳ���ѷ��������ҳ���δ���������ҳ��
	//
	OutputFormat = "\nAllocated Vpn Count: %d.\n";
	fprintf(StdHandle, OutputFormat, AllocatedVpnCount);
	
	FreeVpnCount = TotalVpnCount - AllocatedVpnCount;
	OutputFormat = "Free Vpn Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, FreeVpnCount);
	
	//
	// �������ҳ����ҳ�����Ϳ���ҳ����
	//
	OutputFormat = "Zeroed Physical Page Count: %d.\n";
	fprintf(StdHandle, OutputFormat, MiZeroedPageCount);
	
	OutputFormat = "Free Physical Page Count: %d.\n\n";
	fprintf(StdHandle, OutputFormat, MiFreePageCount);
	
VM_RETURN:	
	KeEnableInterrupts(IntState);	// ���ж�
	
	ObDerefObject(pProcCtrlBlock);
}
