//������δʹ��ҳ���û��㷨
void Lru(int *BlockofMemory, int *PageNumofRef, int BlockCount, int PageNumRefCount)
{
	int i;
	int MissCount = 0;

	printf("************������δʹ��ҳ���û��㷨��************\n");
	
	//���ҳ�����ô���
	OutputPageNumofRef(PageNumofRef, PageNumRefCount);

	for(i = 0; i < PageNumRefCount; i++)
	{
		if(!PageInBlockofMemory(PageNumofRef[i], BlockofMemory, BlockCount)) //ҳ�����ڴ���
		{
			MissCount++;

			//
			// �ڴ���Ӵ��룬ʵ�� LRU ҳ���û��㷨
			//
		}
		else
			OutputBlockofMemory(BlockofMemory, BlockCount, -1, PageNumofRef[i]);
	}

	printf("ȱҳ����Ϊ��%d\n", MissCount);
	printf("LRUȱҳ��Ϊ��%.3f\n", (float)MissCount / PageNumRefCount);
}
