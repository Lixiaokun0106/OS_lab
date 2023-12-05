/***

Copyright (c) 2008 ����Ӣ��ʱ���Ƽ����޹�˾����������Ȩ����

ֻ�������� EOS ����Դ����Э�飨�μ� License.txt���е��������ʹ����Щ���롣
����������ܣ�����ʹ����Щ���롣

�ļ���: start.c

����: EOS �ں˵���ں�����



*******************************************************************************/

#include "ki.h"
#include "mm.h"
#include "ob.h"
#include "ps.h"
#include "io.h"

VOID
KiSystemStartup(
	PVOID LoaderBlock
	)
/*++

����������
	ϵͳ����ڵ㣬Kernel.dll��Loader���ص��ڴ������￪ʼִ�С�

������
	LoaderBlock - Loader���ݵļ��ز�����ṹ��ָ�룬�ڴ������Ҫʹ�á�

����ֵ��
	�ޣ����������Զ���᷵�أ���

ע�⣺
	KiSystemStartup��Loader�����ISRջ��ִ�У������ڵ�ǰ�̣߳����Բ��ܵ����κο�
	�ܵ��������ĺ�����ֻ�ܶԸ���ģ����м򵥵ĳ�ʼ����

--*/
{
	//
	// ��ʼ�����������жϡ�
	//

	// TODO:
	//
	KiInitializeProcessor();// ��ʼ��ȫ����������
	KiInitializeInterrupt();// ��ʼ���ж���������

	//
	// ��ʼ���ɱ���жϿ������Ϳɱ�̶�ʱ��������
	//

	// TODO:
	KiInitializePic();//��ʼ���ɱ���жϿ�����
	KiInitializePit();//�ɱ�̶�ʱ������

	//
	// �Ը�������ģ��ִ�е�һ����ʼ����˳�����ҡ�
	//

	// TODO:
	MmInitializeSystem1(LoaderBlock);// ��ʼ���ڴ����ģ���һ����
	ObInitializeSystem1();// ��ʼ���������ģ���һ����
	PsInitializeSystem1();// ��ʼ������ģ���һ����
	IoInitializeSystem1();// ��ʼ��IO����ģ���һ����
	
	//
	// ����ϵͳ�������̡�
	//
	
	// TODO:
	PsCreateSystemProcess(KiSystemProcessRoutine);// ����ϵͳ���̡�
	//
	// ִ�е�����ʱ�����к�����Ȼ��ʹ���� Loader ��ʼ���Ķ�ջ������ϵͳ�߳�
	// ���Ѵ��ھ���״̬��ִ���̵߳��Ⱥ�ϵͳ�߳̿�ʼʹ�ø��Ե��̶߳�ջ���С�
	//
	
	// TODO:
	KeThreadSchedule();
	//
	// ��������Զ���᷵�ء�
	//
	ASSERT(FALSE);
}
