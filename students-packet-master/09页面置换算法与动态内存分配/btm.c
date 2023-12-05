
#include <stdio.h>
#include <windows.h>
#define MAX_SIZE 1000
#define ALLOC_MIN_SIZE 10//最小分配空间大小.

#define RED FOREGROUND_RED
#define GREEN FOREGROUND_GREEN
#define BLUE FOREGROUND_BLUE

typedef struct Bound{
	union {
		struct Bound * preLink;//头部域前驱
		struct Bound * upLink;//尾部域，指向结点头部
	};
	int tag;//0标示空闲,1表示占用
	int size;//仅仅表示 可用空间，不包括 头部 和 尾部空间
	struct Bound * nextLink;//头部后继指针.
}*Space;

#define FOOT_LOC(p) ((p)+(p->size)-1)//尾部域位置

void initSpace(Space * freeSpace,Space * pav){
	//有2个空间是为了 查找空间的邻接点，防止出界用的。
	*freeSpace = (Space)malloc((MAX_SIZE+2)*sizeof(struct Bound));
	Space head = *freeSpace;
	head->tag = 1;//设置边界已占用
	head++;//指向第一个节点..
	head->tag = 0;//设置节点空闲.
	head->preLink = head->nextLink = head;//循环双链表..
	head->size = MAX_SIZE;
	*pav = head;//设置头指针
	Space foot = FOOT_LOC(head);
	foot->tag = 0;
	foot->upLink = head;
	foot++;
	foot->tag = 1;//设置 边界 已占用
}
Space userSpace[MAX_SIZE] = {NULL};//用户空间数组.
int usCount = 0;

Space allocBoundTag(Space * pav,int size){
	Space p = * pav;
	for (;p != NULL && p->size < size && p->nextLink != *pav ; p = p->nextLink) ;
	if (p == NULL || p->size < size ){
		return NULL;
	}
	*pav = p->nextLink;
	if (p->size - size > ALLOC_MIN_SIZE){//从高位截取p,不破坏指针间的关系.
		p->size -= size;
		Space foot = FOOT_LOC(p);
		foot->upLink = p;
		foot->tag = 0;
		p = foot + 1;
		p->size = size;
		foot = FOOT_LOC(p);
		p->tag = foot->tag = 1;
		foot->upLink = p;
	}
	else{//分配后剩余空间小于 ALLOC_MIN_SIZE 
		if (p = *pav){//只剩下一个空间了，清空指针
			*pav = NULL;
			p->tag = 1;
		}
		else{//直接分配 p->size个空间出去
			Space foot = FOOT_LOC(p);
			foot->tag =p->tag = 1;
			p->preLink->nextLink = p->nextLink;
			p->nextLink->preLink = p->preLink;
		}
	}
	userSpace[usCount] = p;
	usCount++;
	return p;
}
//回收空间，合并 邻接空闲空间.
void reclaimBoundTag(Space * pav,Space sp){
	Space pre = NULL;//前一个空间
	Space next = NULL;//后一个空间..
	Space foot = NULL;
	int pTag = -1;
	int nTag = -1;
	int i = 0;

	if(*pav != NULL)
	{
		pre = (sp -1)->upLink;//前一个空间
		pTag = pre->tag;
		next = sp + sp->size;//后一个空间..	
		nTag = next->tag;	
	}
	
	if ((*pav != NULL && pTag == 1 && nTag == 1) || *pav == NULL){//前后都被占用，直接插入在表头.
		Space foot = FOOT_LOC(sp);
		foot->tag = sp->tag = 0;
		if (*pav == NULL){
			*pav = sp->preLink = sp->nextLink = sp;
		}
		else{
			sp->nextLink = *pav;
			sp->preLink = (*pav)->preLink;
			(*pav)->preLink = sp;
			sp->preLink->nextLink = sp;
			*pav = sp;//将头指针指向刚释放的空间
		}
	}
	else if(pTag == 0 && nTag == 1){//前面的可以合并..
		pre->size += sp->size;
		foot = FOOT_LOC(pre);
		foot->tag = 0;
		foot->upLink = pre;
	}
	else if(pTag == 1 && nTag == 0){//后面的可以合并
		if(*pav == next)
		{
			*pav = sp;
		}
		sp->preLink = next->preLink;
		sp->nextLink = next->nextLink;
		next->preLink->nextLink = sp;
		next->nextLink->preLink = sp;
		sp->size += next->size;
		foot = FOOT_LOC(sp);
		sp->tag = foot->tag = 0;
		foot->upLink = sp;
	}
	else{//前后都可以合并
		if(*pav == next)
		{
			*pav = pre;
		}
		pre->size += sp->size + next->size;
		pre->nextLink = next->nextLink;
		next->nextLink->preLink = pre;
		foot = FOOT_LOC(pre);
		foot->upLink = pre;
	}
	//设置用户空间
	for (i = 0; i < usCount; i++){
		if (sp == userSpace[i]){
			userSpace[i] = NULL;
		}
	}
}

/* 设置字体颜色 */
void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_INTENSITY | color);
}

void print(Space s){
	printf("0x%0x    %6d\t%10d           0x%0x  0x%0x\n",s,s->size,s->tag, s->preLink,s->nextLink);
}

void printSpace(Space pav){
	SetColor(RED|GREEN|BLUE);
	printf("空间首地址  空间大小  块标志(0:空闲,1:占用)  前驱地址  后继地址\n");
	SetColor(GREEN);
	int i = 0;
	Space p = NULL, us = NULL;

	if (pav != NULL)
	{
		p = pav;
		print(p);
		for (p = p->nextLink; p != pav; p = p->nextLink){
			print(p);
			i++;
		}
	}
	for (i = 0; i < usCount; i++){
		us = userSpace[i];
		if (us){
			printf("0x%0x    %6d\t%10d\t\n",us, us->size, us->tag);  
		}
	}
}

int main(int argc, char* argv[])
{
	Space  freeSpace = NULL,pav = NULL;
	initSpace(&freeSpace,&pav);
	int item = 0, i = 0;
	unsigned long start = 0;
	unsigned long joblength;
	Space us = NULL;

	while(1)
	{
		SetColor(RED|BLUE|GREEN);
		printf("选择功能项：(0-退出  1-分配内存  2-回收内存  3-显示内存)：");
		scanf("%d",&item);
		switch(item)
		{
			case 0:
				exit(0);
			case 1:
				SetColor(RED|GREEN);
				printf("所需内存长度：");
				scanf("%*c%ld",&joblength);
				allocBoundTag(&pav,joblength);
				break;
			case 2:
				SetColor(RED|BLUE);
				printf("输入要回收分区的首地址：");
				scanf("%x",&start);
				for (i = 0; i < usCount; i++){
					us = userSpace[i];
					if ((unsigned long)us == start){	 
						reclaimBoundTag(&pav, us);	
						break;
					}
				}
				if(i == usCount)
				{
					SetColor(RED);
					printf("输入要回收分区的首地址不符合要求\n");
				}
				if(pav->size == MAX_SIZE)
				{
					usCount = 0;
				}
				break;
			case 3:		
				printSpace(pav);
			
				break;
			default:
				printf("没有该选项\n");
		}
	}

	return 0;
}