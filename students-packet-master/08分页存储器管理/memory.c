/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/

#include "EOSApp.h"

/*
	����ҳ��ӳ����ƽ������ڴ��Ϊ 4K ��С��ҳ��ÿҳ��Ӧһ��
	ҳ��ţ�PFN: Page Frame Number��������ҳ 0x0-0x0FFF ��Ӧ
	�� PFN Ϊ 0��0x1000-0x1FFF ��Ӧ�� PFN Ϊ 1���������ơ���
	�ԣ��� PFN ���� 12 λ���Ի������ҳ�Ļ�ַ��
	
	CR3 �Ĵ�������� 20 λ�б�����ҳĿ¼��Page Directory����
	ҳ��š��� 12 λ����δ�á�
	 
	 	   31                    12 11         0
	       +-----------------------+------------+
	   CR3 | PFN of Page Direcotry |  Reserved  |
	       +-----------------------+------------+
*/

#define CR3_SHIFT	12	// CR3 �Ĵ�����ֵ���� 12 λ���ҳĿ¼��ҳ���

/*
	����ҳ��ӳ������е�ҳĿ¼��ҳ���ڽ��̵� 4G �����ַ�ռ��е�
	λ������ͼ
	
					   ���̵� 4G �����ַ�ռ�
			----------+-----------------------+
			0x00000000|                       |
					  |                       |
					  |                       |
			 �û���ַ |                       |
			   �ռ�   |                       |
			 ���� 2G��|                       |
					  |                       |
					  |                       |
					  |                       |
					  |                       |
			----------+-----------------------+
			0x80000000|                       |
					  +-----------------------+------------
					  |                       |0xC0000000
					  |                       |
			 �ں˵�ַ |                       |
			   �ռ�   +-----------------------+ 1024 ��ҳ��ռ�ô�
			 ���� 2G��|  һ��ҳĿ¼��ռ�ô�   | 0xC0000000 ��� 4M��
					  |  0xC0300000 ��� 4K�� | 
					  +-----------------------+
					  |                       |
					  +-----------------------+------------
			0xFFFFFFFF|                       |
			----------+-----------------------+
*/

#define PDE_BASE		((ULONG_PTR)0xC0300000)		// ҳĿ¼�������ַ�ռ�Ļ�ַ
#define PTE_BASE		((ULONG_PTR)0xC0000000)		// ҳ���������ַ�ռ�Ļ�ַ

#define PTE_SIZE		0x4		// ҳĿ¼���ҳ����� 4 �ֽ�
#define PTE_PER_TABLE	0x400	// ҳĿ¼���� 1024 �� PDE��ҳ������ 1024 �� PTE
#define PAGE_SIZE 		0x1000	// ����ҳ��ҳĿ¼��ҳ��Ĵ�С���� 4096 �ֽ�

#define PDI_SHIFT 		22		// PDE �ı������ 22 λ��������ַ�ĸ� 10 λ
#define PTI_SHIFT 		12		// PTE �ı������ 12 λ��������ַ�� 22-12 λ

/*
	ҳĿ¼�PDE: Page Directory Entry����ҳ���PTE: Page Table Entry��
	���� 4 ���ֽڣ�����λ��ֻ�г��ó�����ĵģ��Ĺ��ܿ��Բο���ͼ

					31                   12 11       1 0
					+----------------------+----------+-+
			PDE/PTE |         PFN          |          |P| 
					+----------------------+----------+-+
	        
	�� 0 λΪ 0 ʱ��ʾ������Ч��Ϊ 1 ʱ��ʾ��Ч���� 20 λ�� PDE/PTE ӳ���
	ҳ��š�
*/
typedef struct _MMPTE_HARDWARE
{
	ULONG Valid : 1;			// ����λ		0
	ULONG DoNotCare : 11;		// ��������Щλ	1-11
	ULONG PageFrameNumber : 20;	// ҳ���		12-31
}MMPTE_HARDWARE, *PMMPTE_HARDWARE;

ULONG getcr3();		// �����ڻ���ļ���ʵ�ֵĺ�����ʹ����������汻���á�


int main(int argc, char* argv[])
{
	//
	// �������� EOS Ӧ�ó���ǰҪ�ر�ע����������⣺
	//
	// 1�����Ҫ�ڵ���Ӧ�ó���ʱ�ܹ����Խ����ں˲���ʾ��Ӧ��Դ�룬
	//    ����ʹ�� EOS ������Ŀ����������ȫ�汾�� SDK �ļ��У�Ȼ
	//    ��ʹ�ô��ļ��и���Ӧ�ó�����Ŀ�е� SDK �ļ��У����� EOS
	//    ������Ŀ�ڴ����ϵ�λ�ò��ܸı䡣
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

	// Sleep(10000);	// �ȴ� 10 ��

	__asm("cli");	// ���ж�

	//
	// ��� CR3 ��ҳĿ¼��ҳ���
	//
	OutputFormat = "\nCR3->0x%X\n";
	PfnOfPageDirectory = (getcr3() >> CR3_SHIFT);
	printf(OutputFormat, PfnOfPageDirectory);

	//
	// ��һ��ѭ��������ҳĿ¼�е� PDE
	//
	for(IndexOfDirEntry = 0; IndexOfDirEntry < PTE_PER_TABLE; IndexOfDirEntry++)
	{
		pPde = (PMMPTE_HARDWARE)(PDE_BASE + IndexOfDirEntry * PTE_SIZE);

		//
		// ������Ч�� PDE
		//
		if(!pPde->Valid)
			continue;
		
		//
		// ��� PDE ��Ϣ����ʽ���£�
		// PDE: ��� (ӳ��� 4M �����ַ�Ļ�ַ)->��ӳ��ҳ���ҳ���
		//
		OutputFormat = "PDE: 0x%X (0x%X)->0x%X\n";
		VirtualBase = (IndexOfDirEntry << PDI_SHIFT);
		printf(OutputFormat, IndexOfDirEntry, VirtualBase, pPde->PageFrameNumber);
		
		//
		// ���� PDE �ı�ż�����ӳ���ҳ�����������ַ�Ļ�ַ
		//
		PageTableBase = PTE_BASE + IndexOfDirEntry * PAGE_SIZE;
		
		//
		// �ڶ���ѭ��������ҳ���е� PTE
		//
		for(IndexOfTableEntry = 0; IndexOfTableEntry < PTE_PER_TABLE; IndexOfTableEntry++)
		{
			pPte = (PMMPTE_HARDWARE)(PageTableBase + IndexOfTableEntry * PTE_SIZE);
			
			//
			// ������Ч�� PTE
			//
			if(!pPte->Valid)
				continue;
			
			//
			// ��� PTE ��Ϣ����ʽ���£�
			// PTE: ��� (ӳ��� 4K �����ַ�Ļ�ַ)->��ӳ������ҳ��ҳ���
			//
			OutputFormat = "\t\tPTE: 0x%X (0x%X)->0x%X\n";
			VirtualBase = (IndexOfDirEntry << PDI_SHIFT) | (IndexOfTableEntry << PTI_SHIFT);
			printf(OutputFormat, IndexOfTableEntry, VirtualBase, pPte->PageFrameNumber);
			
			//
			// ͳ��ռ�õ�����ҳ��
			//
			PageTotal++;
		} // �ڶ���ѭ������
	} // ��һ��ѭ������
	
	//
	// ���ռ�õ�����ҳ�����������ڴ���
	//
	OutputFormat = "\nPhysical Page Total: %d\n";
	printf(OutputFormat, PageTotal);
	OutputFormat = "Physical Memory Total: %d\n\n";
	printf(OutputFormat, PageTotal * PAGE_SIZE);
	
	__asm("sti");	// ���ж�
	
	// Sleep(60000);	// �ȴ� 60 ��

	return 0;
}
