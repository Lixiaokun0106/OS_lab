/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/

ULONG PfnArray[2];

//
// ����δӳ�������ڴ�������ַ�ᴥ���쳣��
// ����ע�ͻ���ɾ�����д������ִ�к���Ĵ��롣
//
*((PINT)0xE0000000) = 100;	
	
//
// ���ں���������δ�õ�����ҳ��
// �� PfnArray ���鷵����������ҳ��ҳ��š�
//
MiAllocateZeroedPages(2, PfnArray);
OutputFormat = "New page frame number: 0x%X, 0x%X\n";
fprintf(StdHandle, OutputFormat, PfnArray[0], PfnArray[1]);

//
// ʹ�� PfnArray[0] ҳ��Ϊҳ��ӳ���ַΪ 0xE00000000 �� 4M �����ַ��
//
IndexOfDirEntry = (0xE0000000 >> 22);	// �����ַ�ĸ� 10 λ�� PDE ���
((PMMPTE_HARDWARE)0xC0300000)[IndexOfDirEntry].PageFrameNumber = PfnArray[0];
((PMMPTE_HARDWARE)0xC0300000)[IndexOfDirEntry].Valid = 1;		// ��Ч
((PMMPTE_HARDWARE)0xC0300000)[IndexOfDirEntry].Writable = 1;	// ��д
MiFlushEntireTlb();	// ˢ�¿��

//
// ���� PDE �ı�ż�����ӳ���ҳ�����������ַ�Ļ�ַ
//
PageTableBase = 0xC0000000 + IndexOfDirEntry * PAGE_SIZE;

//
// �� PfnArray[1] ����ҳ�� PfnArray[0] ������ PTE �У�
// �ֱ�ӳ���ַΪ 0xE0000000 �� 0xE0001000 �� 4K �����ַ
//
IndexOfTableEntry = (0xE0000000 >> 12) & 0x3FF;	// �����ַ�� 12-22 λ�� PTE ���
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].PageFrameNumber = PfnArray[1];
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].Valid = 1;		// ��Ч
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].Writable = 1;	// ��д
MiFlushEntireTlb();	// ˢ�¿��

IndexOfTableEntry = (0xE0001000 >> 12) & 0x3FF;	// �����ַ�� 12-22 λ�� PTE ���
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].PageFrameNumber = PfnArray[1];
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].Valid = 1;		// ��Ч
((PMMPTE_HARDWARE)PageTableBase)[IndexOfTableEntry].Writable = 1;	// ��д
MiFlushEntireTlb();	// ˢ�¿��

//
// ����
//
OutputFormat = "Read Memory 0xE0001000: %d\n";
fprintf(StdHandle, OutputFormat, *((PINT)0xE0001000));
*((PINT)0xE0000000) = 100;	// д�����ڴ�
fprintf(StdHandle, OutputFormat, *((PINT)0xE0001000));
