#include "EOSApp.h"

//
// main �������������壺
// argc - argv ����ĳ��ȣ���С����Ϊ 1��argc - 1 Ϊ�����в�����������
// argv - �ַ���ָ�����飬���鳤��Ϊ�����в������� + 1������ argv[0] �̶�ָ��ǰ
//        ������ִ�еĿ�ִ���ļ���·���ַ�����argv[1] ��������ָ��ָ�����������
//        ������
//        ����ͨ������������ "a:\hello.exe -a -b" �������̺�hello.exe �� main ��
//        ���Ĳ��� argc ��ֵΪ 3��argv[0] ָ���ַ��� "a:\hello.exe"��argv[1] ָ��
//        �����ַ��� "-a"��argv[2] ָ������ַ��� "-b"��
//
int main(int argc, char* argv[])
{
	//
	// �����Ҫ�ڵ���Ӧ�ó���ʱ�ܹ����Խ����ں˲���ʾ��Ӧ��Դ�룬
	// ����ʹ�� EOS �ں���Ŀ����������ȫ�汾�� SDK �ļ��У�Ȼ
	// ��ʹ�øո����ɵ� SDK �ļ��и��Ǵ�Ӧ�ó�����Ŀ�е� SDK �ļ�
	// �У����� EOS �ں���Ŀ�ڴ����ϵ�λ�ò��ܸı䡣
	//

	/* TODO: �ڴ˴������Լ��Ĵ��� */
	int *p;
	p=VirtualAlloc(NULL,sizeof(int),MEM_RESERVE|MEM_COMMIT);
	printf("Allocate %d bytes virtual memory at 0x%x\n\n",sizeof(int),p);
	printf("Virtual memory original value: 0x%x\n",*p);
	*p=0xFFFFFFFF;
	printf("Virtual memory new value: 0x%x\n\n",*p);
	printf("Wait for 10 second\n\n");
	Sleep(10000);//ms
	int result=VirtualFree(p,0,MEM_RELEASE);
	if(result==1){printf("Release virtual memory success!\n\n");}
	else {return 0;}

	//
	// ��ʼ��ѭ��������Ӧ�ó�����̾Ͳ��������
	//
	printf("Endless loop!\n");
	for(;;) {
	}

	return 0;
}