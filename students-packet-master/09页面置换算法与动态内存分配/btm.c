
#include <stdio.h>
#include <windows.h>
#define MAX_SIZE 1000
#define ALLOC_MIN_SIZE 10//��С����ռ��С.

#define RED FOREGROUND_RED
#define GREEN FOREGROUND_GREEN
#define BLUE FOREGROUND_BLUE

typedef struct Bound{
	union {
		struct Bound * preLink;//ͷ����ǰ��
		struct Bound * upLink;//β����ָ����ͷ��
	};
	int tag;//0��ʾ����,1��ʾռ��
	int size;//������ʾ ���ÿռ䣬������ ͷ�� �� β���ռ�
	struct Bound * nextLink;//ͷ�����ָ��.
}*Space;

#define FOOT_LOC(p) ((p)+(p->size)-1)//β����λ��

void initSpace(Space * freeSpace,Space * pav){
	//��2���ռ���Ϊ�� ���ҿռ���ڽӵ㣬��ֹ�����õġ�
	*freeSpace = (Space)malloc((MAX_SIZE+2)*sizeof(struct Bound));
	Space head = *freeSpace;
	head->tag = 1;//���ñ߽���ռ��
	head++;//ָ���һ���ڵ�..
	head->tag = 0;//���ýڵ����.
	head->preLink = head->nextLink = head;//ѭ��˫����..
	head->size = MAX_SIZE;
	*pav = head;//����ͷָ��
	Space foot = FOOT_LOC(head);
	foot->tag = 0;
	foot->upLink = head;
	foot++;
	foot->tag = 1;//���� �߽� ��ռ��
}
Space userSpace[MAX_SIZE] = {NULL};//�û��ռ�����.
int usCount = 0;

Space allocBoundTag(Space * pav,int size){
	Space p = * pav;
	for (;p != NULL && p->size < size && p->nextLink != *pav ; p = p->nextLink) ;
	if (p == NULL || p->size < size ){
		return NULL;
	}
	*pav = p->nextLink;
	if (p->size - size > ALLOC_MIN_SIZE){//�Ӹ�λ��ȡp,���ƻ�ָ���Ĺ�ϵ.
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
	else{//�����ʣ��ռ�С�� ALLOC_MIN_SIZE 
		if (p = *pav){//ֻʣ��һ���ռ��ˣ����ָ��
			*pav = NULL;
			p->tag = 1;
		}
		else{//ֱ�ӷ��� p->size���ռ��ȥ
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
//���տռ䣬�ϲ� �ڽӿ��пռ�.
void reclaimBoundTag(Space * pav,Space sp){
	Space pre = NULL;//ǰһ���ռ�
	Space next = NULL;//��һ���ռ�..
	Space foot = NULL;
	int pTag = -1;
	int nTag = -1;
	int i = 0;

	if(*pav != NULL)
	{
		pre = (sp -1)->upLink;//ǰһ���ռ�
		pTag = pre->tag;
		next = sp + sp->size;//��һ���ռ�..	
		nTag = next->tag;	
	}
	
	if ((*pav != NULL && pTag == 1 && nTag == 1) || *pav == NULL){//ǰ�󶼱�ռ�ã�ֱ�Ӳ����ڱ�ͷ.
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
			*pav = sp;//��ͷָ��ָ����ͷŵĿռ�
		}
	}
	else if(pTag == 0 && nTag == 1){//ǰ��Ŀ��Ժϲ�..
		pre->size += sp->size;
		foot = FOOT_LOC(pre);
		foot->tag = 0;
		foot->upLink = pre;
	}
	else if(pTag == 1 && nTag == 0){//����Ŀ��Ժϲ�
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
	else{//ǰ�󶼿��Ժϲ�
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
	//�����û��ռ�
	for (i = 0; i < usCount; i++){
		if (sp == userSpace[i]){
			userSpace[i] = NULL;
		}
	}
}

/* ����������ɫ */
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
	printf("�ռ��׵�ַ  �ռ��С  ���־(0:����,1:ռ��)  ǰ����ַ  ��̵�ַ\n");
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
		printf("ѡ�����(0-�˳�  1-�����ڴ�  2-�����ڴ�  3-��ʾ�ڴ�)��");
		scanf("%d",&item);
		switch(item)
		{
			case 0:
				exit(0);
			case 1:
				SetColor(RED|GREEN);
				printf("�����ڴ泤�ȣ�");
				scanf("%*c%ld",&joblength);
				allocBoundTag(&pav,joblength);
				break;
			case 2:
				SetColor(RED|BLUE);
				printf("����Ҫ���շ������׵�ַ��");
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
					printf("����Ҫ���շ������׵�ַ������Ҫ��\n");
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
				printf("û�и�ѡ��\n");
		}
	}

	return 0;
}