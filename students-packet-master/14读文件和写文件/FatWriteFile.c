/*
�ṩ��ʾ��������Ϊ�˲���һ��������߽���һ�����ԣ�����������
�ȫ�ı���ʵ������˲�Ӧ��Ӧ�ó������վ��ʹ�ø�ʾ�����롣��
�ڳ�����ʾ�������Ԥ����;�����ʹ������ɵ�żȻ��̷�����ʧ��
����Ӣ��ʱ���Ƽ����޹�˾���е��κ����Ρ�
*/


STATUS
FatWriteFile(
	IN PVCB Vcb,
	IN PFCB File,
	IN ULONG Offset,
	IN ULONG BytesToWrite,
	OUT PVOID Buffer,
	OUT PULONG BytesWriten
	)
{
	STATUS Status;

	// �����ڽ��·���Ĵز������β��ʱ������֪��ǰһ���صĴغţ�
	// ���Զ����ˡ�ǰһ���غš��͡���ǰ�غš�����������
	USHORT PrevClusterNum, CurrentClusterNum;
	USHORT NewClusterNum;
	ULONG ClusterIndex;
	ULONG FirstSectorOfCluster;
	ULONG OffsetInSector;
	
	ULONG i;

	// д�����ʼλ�ò��ܳ����ļ���С������Ӱ�������ļ���С�����Ӵأ�����ԭ�򣿣�
	if (Offset > File->FileSize)
		return STATUS_SUCCESS;

	// ���ݴصĴ�С������д�����ʼλ���ڴ����ĵڼ������У��� 0 ��ʼ������
	ClusterIndex = Offset / FatBytesPerCluster(&Vcb->Bpb);
	
	// ˳�Ŵ���������д�����ʼλ�����ڴصĴغš�
	PrevClusterNum = 0;
	CurrentClusterNum = File->FirstCluster;
	for (i = ClusterIndex; i > 0; i--) {
		PrevClusterNum = CurrentClusterNum;
		CurrentClusterNum = FatGetFatEntryValue(Vcb, PrevClusterNum);	
	}

	// ���д�����ʼλ�û�û�ж�Ӧ�Ĵأ������Ӵ�
	if (0 == CurrentClusterNum || CurrentClusterNum >= 0xFF8) {

		// Ϊ�ļ�����һ�����д�
		FatAllocateOneCluster(Vcb, &NewClusterNum);

		// ���·���Ĵذ�װ��������
		if (0 == File->FirstCluster)
			File->FirstCluster = NewClusterNum;
		else
			FatSetFatEntryValue(Vcb, PrevClusterNum, NewClusterNum);
		
		CurrentClusterNum = NewClusterNum;
	}

	// ���㵱ǰ�صĵ�һ�������������š��ش� 2 ��ʼ������
	FirstSectorOfCluster = Vcb->FirstDataSector + (CurrentClusterNum - 2) * Vcb->Bpb.SectorsPerCluster;
	
	// ����дλ���������ڵ��ֽ�ƫ�ơ�
	OffsetInSector = Offset % Vcb->Bpb.BytesPerSector;

	// Ϊ�˼򵥣���ʱֻ����һ���ذ���һ�������������
	// ����ֻ����д���������һ��������Χ�ڵ������
	Status = IopReadWriteSector( Vcb->DiskDevice,
									FirstSectorOfCluster,
									OffsetInSector,
									(PCHAR)Buffer,
									BytesToWrite,
									FALSE );

	if (!EOS_SUCCESS(Status))
		return Status;

	// ����ļ�����������������޸��ļ��ĳ��ȡ�
	if (Offset + BytesToWrite > File->FileSize) {
		File->FileSize = Offset + BytesToWrite;
		
		// ����������ļ�����Ҫͬ���޸��ļ��ڴ����϶�Ӧ�� DIRENT �ṹ
		// �塣Ŀ¼�ļ��� DIRENT �ṹ���е� FileSize ��ԶΪ 0�������޸ġ�
		if (!File->AttrDirectory)
			FatWriteDirEntry(Vcb, File);
	}
	
	// ����ʵ��д����ֽ�����
	*BytesWriten = BytesToWrite;

	return STATUS_SUCCESS;
}
