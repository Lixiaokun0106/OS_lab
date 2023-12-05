/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/


/*
������ĺ�����ʵ���� SCAN ���̵����㷨�������㷨����
���㷨���ȿ��Ǵ�ͷ�ƶ��ķ���Ȼ���ٿ����ƶ�������̡�
*/


PREQUEST
IopDiskSchedule(
	VOID
	)
{
	PLIST_ENTRY pListEntry;
	PREQUEST pRequest;
	LONG Offset;
	
	ULONG InsideShortestDistance = 0xFFFFFFFF;
	ULONG OutsideShortestDistance = 0xFFFFFFFF;
	PREQUEST pNextRequest = NULL;
	
	//
	// ��Ҫ�����������һ�λ�����
	//
	while (TRUE) {
	
		for (pListEntry = RequestListHead.Next;	// ��������еĵ�һ������������ͷָ�����һ������
	     	 pListEntry != &RequestListHead;	// �������������ͷʱ����ѭ����
	     	 pListEntry = pListEntry->Next) {
		
			//
			// �����������������ָ��
			//
			pRequest = CONTAINING_RECORD(pListEntry, REQUEST, ListEntry);
			
			//
			// ���������Ӧ���߳������ʵĴŵ��뵱ǰ��ͷ���ڴŵ���ƫ�ƣ�������������ʾ��
			//
			Offset = pRequest->Cylinder - CurrentCylinder;
			
			if (0 == Offset) {
				//
				// ����߳�Ҫ���ʵĴŵ��뵱ǰ��ͷ���ڴŵ���ͬ�����������ء�
				//
				pNextRequest = pRequest;
				goto RETURN;
			} else if (ScanInside && Offset > 0) {
				//
				// ��¼�����ƶ�������̵��߳�
				//
				if (Offset < InsideShortestDistance) {
			
					InsideShortestDistance = Offset;
					pNextRequest = pRequest;
				}
			} else if (!ScanInside && Offset < 0) {
				//
				// ��¼�����ƶ�������̵��߳�
				//
				if (-Offset < OutsideShortestDistance) {
			
					OutsideShortestDistance = -Offset;
					pNextRequest = pRequest;
				}
			}
		}
		
		//
		// �����һ�α���û���̷߳���ָ�������ϵĴŵ����ͱ任����
		// ���еڶ��α������������α�����һ�����ҵ����ʵ��̡߳�
		//
		if (NULL == pNextRequest)
			ScanInside = !ScanInside;
		else
			break;
	}
	
RETURN:
	return pNextRequest;
}
