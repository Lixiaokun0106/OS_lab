
#include <stdio.h>
#include <math.h>
#include <windows.h>

#define RED FOREGROUND_RED
#define GREEN FOREGROUND_GREEN
#define BLUE FOREGROUND_BLUE

#define M 10	/* �����ÿռ�������(2���ݴ�)���ӱ�ĸ���ΪM+1 */
#define N 100	/* ռ�ÿ���������ֵ */

//���ϵͳ�����ÿռ��Ľṹ
typedef struct _Node{
	struct _Node *llink;	//ָ��ǰ���ڵ�
	int bflag;				//���־��0�����У�1��ռ��
	int bsize;				//���С��ֵΪ2���ݴ�k
	struct _Node *rlink;	//ͷ����ָ���̽ڵ�
}Node;

//�����ÿռ���ͷ���
typedef struct HeadNode{
	int nodesize;		//����Ŀ��п��С
	Node *first;		//����ı�ͷָ��
}FreeList[M+1];

Node* start;   // ȫ�ֱ���startΪ�������ɿռ���׵�ַ

/* avail[0...M]Ϊ�����ÿռ��nΪ��������������в�С��n�Ŀ��п飬
�������Ӧ�Ĵ洢�飬�������׵�ַ�����򣬷���NULL */
Node* AllocBuddy(FreeList *avail, unsigned long n)
{
	int i, k;
	Node *pa, *pi, *pre, *suc;
	//�����������Ҫ����ӱ����п��С����n�ı�
	for(k = 0; k <= M && ((*avail)[k].nodesize <= n  || !(*avail)[k].first); ++k)
		;

	if(k>M) 	//	����ʧ�ܷ���NULL
	{
		printf("�ڴ����ʧ�ܣ�\n");
		return NULL;
	}
	else{		//���з���
		pa = (*avail)[k].first;			//ָ��ɷ����ĵ�һ���ڵ�
		pre = pa->llink;				//�ֱ�ָ��ǰ���ͺ��
		suc = pa->rlink;				
		
		if(pa == suc)					//�������ӱ��ɿձ�
			(*avail)[k].first = NULL;
			
		else{							//���ӱ�ɾȥ*pa�ڵ�
			pre->rlink = suc;
			suc->llink = pre;
			(*avail)[k].first = suc;
		}
		
		/* ��ʣ��������Ӧ�ӱ� */
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

//���������ʼ��ַΪp�����СΪpow(2,k)�Ŀ�Ļ���ַ
Node* buddy(Node* n)
{	
	if((n-start)%(int)pow(2,n->bsize+1)==0)		//pΪǰ��
		return n + (int)pow(2,n->bsize);
	else										//pΪ���
		return n - (int)pow(2,n->bsize);
}

// ���ϵͳ�Ļ����㷨 ��p��ָ���ͷſ���յ������ÿռ��pav��
void Reclaim(FreeList *pav,Node* *p)
{ 
	
	//
	//	�˴���TODO: �ڴ���Ӵ���
	//

}

/* ����������ɫ */
void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_INTENSITY | color);
}

//������п��ÿռ��
void print_free(FreeList p)
{
	int i;
	Node* head;
	//����������ɫΪ��ɫ
	SetColor(RED|BLUE|GREEN);
									
	printf("�����ÿռ䣺\n");
	printf("      ��Ĵ�С    ����׵�ַ\t���־(0:���� 1:ռ��)  ǰ����ַ  ��̵�ַ\n");
	
	SetColor(GREEN);
											
	for(i = 0; i <= M; i++)
	{
		if(p[i].first)		//��i�������ÿռ��ͬ
		{
			head = p[i].first;	//headָ������ĵ�һ���ڵ��ͷ�����׵�ַ��
			do
			{
				printf("\t%d\t   0x%x\t    %10d         0x%x  0x%x\n",(int)pow(2,head->bsize), head, head->bflag, head->llink, head->rlink);
				head = head->rlink;
			}while(head != p[i].first);
		}
	}
	printf("\n");
}

// ���p������ָ���ѷ���ռ�
void print_used(Node* p[])
{ 
	int i;
	//����������ɫΪ��ɫ
	SetColor(RED|BLUE|GREEN);
									
	printf("�����ÿռ䣺\n");
	printf("     ռ�ÿ���\tռ�ÿ���׵�ַ\t���С\t���־(0:���� 1:ռ��)\n");
	
	SetColor(RED);
											
	for(i = 0; i < N; i++)
		if(p[i]) // ָ�벻Ϊ0(ָ��һ��ռ�ÿ�) 
			printf("\t  %d\t   0x%x\t %d\t  %d\n",i, p[i],(int)pow(2,p[i]->bsize), p[i]->bflag); 
	printf("\n");
}

int main(int argc, char* argv[])
{
        /* ע�⣺��Ӧ�ó����в���ʹ�öϵ�ȵ��Թ��� */
	int i, j, item;
	unsigned long joblength;
	FreeList avail;
	Node* used_block[N] = {NULL};	//used_block����Ϊռ�ÿ���׵�ַ
	
	//��ʼ�������ÿռ�
	for(i = 0; i <= M; i++)
	{
		avail[i].nodesize = (int)pow(2,i);
		avail[i].first = NULL;
	}
	
	// ���������������һ����� 
	start = avail[M].first = (Node*)malloc(avail[M].nodesize * sizeof(Node));
	if(start) // ���ɽ��ɹ� 
	{
		start->llink = start->rlink = start; // ��ʼ���ý�� 
		start->bflag = 0;
		start->bsize = M;
		int color = 8;
		i = 0;
		while(1)
		{
			//����������ɫΪ��ɫ
			SetColor(RED|BLUE|GREEN);
			printf("ѡ�����(0-�˳�  1-��������  2-��������  3-��ʾ����):");
			scanf("%d",&item);
			switch(item)
			{
				case 0:
					exit(0);
				case 1:
					SetColor(RED|GREEN);
					printf("������ҵ���賤�ȣ�");
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
					printf("����Ҫ���տ�Ŀ�ţ�");
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
							printf("û�пɻ��յ��ڴ��,�������ٽ��л���!\n");
						}
						else
						{
							printf("����Ŀ�Ų��Ϸ�\n");
						}		
					}	
					break;
				case 3:	
					print_free(avail);
					print_used(used_block);
					break;
				default:
					printf("û�и�ѡ��\n");
			}
		}
	}	
}
