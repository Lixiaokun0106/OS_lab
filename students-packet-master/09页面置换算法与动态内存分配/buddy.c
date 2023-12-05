
#include <stdio.h>
#include <math.h>
#include <windows.h>

#define RED FOREGROUND_RED
#define GREEN FOREGROUND_GREEN
#define BLUE FOREGROUND_BLUE

#define M 10	/* 可利用空间总容量(2的幂次)，子表的个数为M+1 */
#define N 100	/* 占用块个数的最大值 */

//伙伴系统可利用空间表的结构
typedef struct _Node{
	struct _Node *llink;	//指向前驱节点
	int bflag;				//块标志，0：空闲，1：占用
	int bsize;				//块大小，值为2的幂次k
	struct _Node *rlink;	//头部域，指向后继节点
}Node;

//可利用空间表的头结点
typedef struct HeadNode{
	int nodesize;		//链表的空闲块大小
	Node *first;		//链表的表头指针
}FreeList[M+1];

Node* start;   // 全局变量start为整个生成空间的首地址

/* avail[0...M]为可利用空间表，n为申请分配量。若有不小于n的空闲块，
则分配相应的存储块，并返回首地址；否则，返回NULL */
Node* AllocBuddy(FreeList *avail, unsigned long n)
{
	int i, k;
	Node *pa, *pi, *pre, *suc;
	//查找满足分配要求的子表即空闲块大小大于n的表
	for(k = 0; k <= M && ((*avail)[k].nodesize <= n  || !(*avail)[k].first); ++k)
		;

	if(k>M) 	//	分配失败返回NULL
	{
		printf("内存分配失败！\n");
		return NULL;
	}
	else{		//进行分配
		pa = (*avail)[k].first;			//指向可分配表的第一个节点
		pre = pa->llink;				//分别指向前驱和后继
		suc = pa->rlink;				
		
		if(pa == suc)					//分配后该子表变成空表
			(*avail)[k].first = NULL;
			
		else{							//从子表删去*pa节点
			pre->rlink = suc;
			suc->llink = pre;
			(*avail)[k].first = suc;
		}
		
		/* 将剩余块插入相应子表 */
		for(i = 1; (*avail)[k-i].nodesize >= n+1; ++i)
		{
			pi = pa + (int)pow(2,k-i);
			pi->rlink = pi;
			pi->llink = pi;
			pi->bflag = 0;
			pi->bsize = k-i;
			(*avail)[k-i].first = pi;
			
		}
		pa->bflag = 1;
		pa->bsize = k - (--i);
	}
	return pa;
}

//返回相对起始地址为p，块大小为pow(2,k)的块的伙伴地址
Node* buddy(Node* n)
{	
	if((n-start)%(int)pow(2,n->bsize+1)==0)		//p为前块
		return n + (int)pow(2,n->bsize);
	else										//p为后块
		return n - (int)pow(2,n->bsize);
}

// 伙伴系统的回收算法 将p所指的释放块回收到可利用空间表pav中
void Reclaim(FreeList *pav,Node* *p)
{ 
	
	//
	//	此处代TODO: 在此添加代码
	//

}

/* 设置字体颜色 */
void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_INTENSITY | color);
}

//输出所有可用空间表
void print_free(FreeList p)
{
	int i;
	Node* head;
	//设置字体颜色为白色
	SetColor(RED|BLUE|GREEN);
									
	printf("可利用空间：\n");
	printf("      块的大小    块的首地址\t块标志(0:空闲 1:占用)  前驱地址  后继地址\n");
	
	SetColor(GREEN);
											
	for(i = 0; i <= M; i++)
	{
		if(p[i].first)		//第i个可利用空间表不同
		{
			head = p[i].first;	//head指向链表的第一个节点的头部域（首地址）
			do
			{
				printf("\t%d\t   0x%x\t    %10d         0x%x  0x%x\n",(int)pow(2,head->bsize), head, head->bflag, head->llink, head->rlink);
				head = head->rlink;
			}while(head != p[i].first);
		}
	}
	printf("\n");
}

// 输出p数组所指的已分配空间
void print_used(Node* p[])
{ 
	int i;
	//设置字体颜色为白色
	SetColor(RED|BLUE|GREEN);
									
	printf("已利用空间：\n");
	printf("     占用块块号\t占用块的首地址\t块大小\t块标志(0:空闲 1:占用)\n");
	
	SetColor(RED);
											
	for(i = 0; i < N; i++)
		if(p[i]) // 指针不为0(指向一个占用块) 
			printf("\t  %d\t   0x%x\t %d\t  %d\n",i, p[i],(int)pow(2,p[i]->bsize), p[i]->bflag); 
	printf("\n");
}

int main(int argc, char* argv[])
{
        /* 注意：在应用程序中不能使用断点等调试功能 */
	int i, j, item;
	unsigned long joblength;
	FreeList avail;
	Node* used_block[N] = {NULL};	//used_block数组为占用块的首地址
	
	//初始化可利用空间
	for(i = 0; i <= M; i++)
	{
		avail[i].nodesize = (int)pow(2,i);
		avail[i].first = NULL;
	}
	
	// 在最大链表中生成一个结点 
	start = avail[M].first = (Node*)malloc(avail[M].nodesize * sizeof(Node));
	if(start) // 生成结点成功 
	{
		start->llink = start->rlink = start; // 初始化该结点 
		start->bflag = 0;
		start->bsize = M;
		int color = 8;
		i = 0;
		while(1)
		{
			//设置字体颜色为白色
			SetColor(RED|BLUE|GREEN);
			printf("选择功能项：(0-退出  1-分配主存  2-回收主存  3-显示主存):");
			scanf("%d",&item);
			switch(item)
			{
				case 0:
					exit(0);
				case 1:
					SetColor(RED|GREEN);
					printf("输入作业所需长度：");
					scanf("%ld",&joblength);
					if(avail[M].first && (int)pow(2, avail[M].first->bsize) == 1024)
					{
						i = 0;
					}
					used_block[i] = AllocBuddy(&avail,joblength);
					++i;
					break;
				case 2:
					SetColor(RED|BLUE);
					printf("输入要回收块的块号：");
					scanf("%d",&j);
					if(i > 0 && j < i)
					{
						Reclaim(&avail,&used_block[j]);
					}
					else
					{
						SetColor(RED);
						if(i == 0)
						{
							printf("没有可回收的内存块,请分配后，再进行回收!\n");
						}
						else
						{
							printf("输入的块号不合法\n");
						}		
					}	
					break;
				case 3:	
					print_free(avail);
					print_used(used_block);
					break;
				default:
					printf("没有该选项\n");
			}
		}
	}	
}
