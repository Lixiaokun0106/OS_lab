/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/


/*
������ĺ�����ʵ���� SSTF (Shortest Seek Time First) ���̵����㷨��
���㷨ѡ���������̣߳���Ҫ����ʵĴŵ��뵱ǰ��ͷ���ڵĴŵ����������
��ʹÿ�ε�Ѱ��ʱ����̣��������㷨ȴ���ܱ�֤ƽ��Ѱ��ʱ����̡�
*/


PREQUEST
IopDiskSchedule(
	VOID
	)
{
	PLIST_ENTRY pListEntry;
	PREQUEST pRequest;
	LONG Offset;
	
	ULONG ShortestDistance = 0xFFFFFFFF;
	PREQUEST pNextRequest;
	
	//
	// �����������ѡ���������������Ӧ���߳������ʵĴŵ��뵱ǰ��ͷ���ڵĴŵ����������
	//
	for (pListEntry = RequestListHead.Next;	// ��������еĵ�һ������������ͷָ�����һ������
	     pListEntry != &RequestListHead;	// �������������ͷʱ����ѭ����
	     pListEntry = pListEntry->Next) {
	
		//
		// �����������������ָ��
		//
		pRequest = CONTAINING_RECORD(pListEntry, REQUEST, ListEntry);
		
		//
		// ���������Ӧ���߳������ʵĴŵ��뵱ǰ��ͷ���ڴŵ���ƫ��
		//
		Offset = pRequest->Cylinder - CurrentCylinder;
		
		//
		// ��¼����̵ľ���Ͷ�Ӧ�������ָ��
		// ע�⣬���� ShortestDistance ����ʼ��Ϊ�����޷������ͣ�
		// �����ڵ�һ��ѭ��ʱ���������Ĳ������ʽ��ֵһ��Ϊ�档
		//
		if (abs(Offset) < ShortestDistance) {
		
			ShortestDistance = abs(Offset);
			pNextRequest = pRequest;
		}
	}
	
	//
	// ѭ��������pNextRequest ָ��������Ӧ���߳�������
	// �Ĵŵ��뵱ǰ��ͷ���ڵĴŵ�������̡� 
	//
	return pNextRequest;
}
