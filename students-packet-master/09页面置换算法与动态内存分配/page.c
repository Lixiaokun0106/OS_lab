#include <stdio.h>
#include <stdlib.h>


//����ڴ��ҳ�����
int OutputBlockofMemory(int *BlockofMemory, int BlockCount, int ReplacePage, int PageNum)
{
	int i;

	printf("����ҳ�� %d ��", PageNum);
	printf("�ڴ��е�ҳ���Ϊ:\t");
	for(i = 0; i < BlockCount; i++)
	{
		if(BlockofMemory[i] < 0)
			printf("#  ");
		else
			printf("%d  ", BlockofMemory[i]);
	}

	if(ReplacePage != -1)
		printf("\t��̭ҳ���Ϊ:%d", ReplacePage);

	printf("\n");

	return -1;
}

//���ҳ�����ô���
void OutputPageNumofRef(int* PageNumofRef, int PageNumRefCount)
{
	int i = 0;
	printf("ҳ�����ô�Ϊ:\t");
	for(i = 0; i < PageNumRefCount; i++)
		printf("%d  ", PageNumofRef[i]);
	printf("\n");
}

//�ڴ��ҳ�������
void ResetBlockofMemory(int *BlockofMemory, int BlockCount)
{
	int i;
	for(i = 0; i < BlockCount; i++)
		BlockofMemory[i] = -1;
}

//�ж�ҳ�Ƿ����ڴ��У����ҳ���ڴ��У�����1�����򷵻�0��
int PageInBlockofMemory(int PageNum, int *BlockofMemory, int BlockCount)
{
	int i;
	for(i = 0; i < BlockCount; i++)
		if(PageNum == BlockofMemory[i])
			return 1;
	return 0;
}

//�´η��ʴ���
//����j:  ҳ�����ڴ���е�λ��
//����i�� ҳ�����ҳ������ô��е�λ��
int DistanceOpt(int *BlockofMemory, int *PageNumofRef, int j, int i, int PageNumRefCount)
{
	int k;
	for(k = i + 1; k < PageNumRefCount; k++)
		if(BlockofMemory[j] == PageNumofRef[k])
			return k;
	return PageNumRefCount;
}

//���ҳ���û��㷨
void Opt(int *BlockofMemory, int *PageNumofRef, int BlockCount, int PageNumRefCount)
{
	int i, j, k;
	int MaxIndex1, MaxIndex2;
	int MissCount = 0;
	int ReplacePage;
	int EmptyBlockCount = BlockCount;

	printf("**********���ҳ���û��㷨��**********\n");

	//���ҳ�����ô���
	OutputPageNumofRef(PageNumofRef, PageNumRefCount);

	for(i = 0; i < PageNumRefCount; i++)
	{
		if(!PageInBlockofMemory(PageNumofRef[i], BlockofMemory, BlockCount)) //ҳ�����ڴ���
		{
			MissCount++;

			if(EmptyBlockCount > 0)
			{
				BlockofMemory[BlockCount - EmptyBlockCount] = PageNumofRef[i];
				OutputBlockofMemory(BlockofMemory, BlockCount, -1, PageNumofRef[i]);
				EmptyBlockCount--;
			}
			else
			{
				MaxIndex1 = MaxIndex2 = 0;
				//���δ���ʱ�䲻�����ʵ�ҳ
				for(j = 0; j < BlockCount; j++)
				{
					MaxIndex2 = DistanceOpt(BlockofMemory, PageNumofRef, j, i, PageNumRefCount);
					if(MaxIndex1 < MaxIndex2)
					{
						MaxIndex1 = MaxIndex2;
						k = j;
					}
				}
				ReplacePage = BlockofMemory[k];
				BlockofMemory[k] = PageNumofRef[i];
				OutputBlockofMemory(BlockofMemory, BlockCount, ReplacePage, PageNumofRef[i]);
			}
		}
		else
		{
			OutputBlockofMemory(BlockofMemory,BlockCount, -1, PageNumofRef[i]);
		}
	}

	printf("ȱҳ����Ϊ: %d\n", MissCount);
	printf("OPTȱҳ��Ϊ: %.3f\n", (float)MissCount / PageNumRefCount);
}

//�Ƚ��ȳ�ҳ���û��㷨
void Fifo(int *BlockofMemory,int *PageNumofRef,int BlockCount,int PageNumRefCount)
{
	int i;
	int ReplacePage;
	int ReplaceIndex = 0;
	int MissCount = 0;
	int EmptyBlockCount = BlockCount;

	printf("**********�Ƚ��ȳ�ҳ���û��㷨��**********\n");
	
	//���ҳ�����ô���
	OutputPageNumofRef(PageNumofRef,PageNumRefCount);

	for(i = 0; i < PageNumRefCount; i++)
	{
		if(!PageInBlockofMemory(PageNumofRef[i], BlockofMemory, BlockCount)) //ҳ�����ڴ���
		{
			MissCount++;

			if(EmptyBlockCount > 0)
			{
				BlockofMemory[BlockCount - EmptyBlockCount] = PageNumofRef[i];
				OutputBlockofMemory(BlockofMemory, BlockCount, -1, PageNumofRef[i]);
				EmptyBlockCount--;
			}
			else
			{
				ReplacePage = BlockofMemory[ReplaceIndex];
				BlockofMemory[ReplaceIndex] = PageNumofRef[i];
				ReplaceIndex = (ReplaceIndex + 1) % BlockCount;
				OutputBlockofMemory(BlockofMemory, BlockCount, ReplacePage, PageNumofRef[i]);
			}
		}
		else
			OutputBlockofMemory(BlockofMemory, BlockCount, -1, PageNumofRef[i]);
	}
	printf("ȱҳ����Ϊ��%d\n", MissCount);
	printf("FIFOȱҳ��Ϊ��%.3f\n", (float)MissCount / PageNumRefCount);
}

int main()
{
	int *BlockofMemory;		//�ڴ������
	const int BlockCount = 5;
	int PageNumofRef[] = {7,0,1,2,0,3,0,4,2,3,0,1,1,7,0,1,0,3};  //ҳ������ô�
	int PageNumRefCount = sizeof(PageNumofRef) / sizeof(PageNumofRef[0]);

	BlockofMemory = (int*)malloc(BlockCount * sizeof(int));
	if(BlockofMemory == (int*)NULL)
	{
		printf("�ڴ�������\n");
		exit(1);
	}

	ResetBlockofMemory(BlockofMemory, BlockCount);
	Opt(BlockofMemory, PageNumofRef, BlockCount, PageNumRefCount);

	ResetBlockofMemory(BlockofMemory,BlockCount);
	Fifo(BlockofMemory, PageNumofRef, BlockCount, PageNumRefCount);

	free(BlockofMemory);

	return 0;
}
