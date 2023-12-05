//最近最久未使用页面置换算法
void Lru(int *BlockofMemory, int *PageNumofRef, int BlockCount, int PageNumRefCount)
{
	int i;
	int MissCount = 0;

	printf("************最近最久未使用页面置换算法：************\n");
	
	//输出页面引用串号
	OutputPageNumofRef(PageNumofRef, PageNumRefCount);

	for(i = 0; i < PageNumRefCount; i++)
	{
		if(!PageInBlockofMemory(PageNumofRef[i], BlockofMemory, BlockCount)) //页不在内存中
		{
			MissCount++;

			//
			// 在此添加代码，实现 LRU 页面置换算法
			//
		}
		else
			OutputBlockofMemory(BlockofMemory, BlockCount, -1, PageNumofRef[i]);
	}

	printf("缺页次数为：%d\n", MissCount);
	printf("LRU缺页率为：%.3f\n", (float)MissCount / PageNumRefCount);
}
