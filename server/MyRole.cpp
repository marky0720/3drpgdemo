#include "StdAfx.h"
#include ".\myrole.h"

//��ʼ����ɫ
MyRole::MyRole( RoleProperty* p,Package pack,Socket::MySOCKServer* netser)
{
	
	memcpy((char*)&prop,(char*)p,sizeof(RoleProperty));

	memcpy((char*)&package,(char*)&pack,sizeof(Package));

	this->netser=netser;
	

	lastupdatetime=GetTickCount();

	::InitializeCriticalSection(&g_cs_role);//��ʼ���ٽ�������

}

MyRole::~MyRole(void)
{
}

//��ɫ����
void MyRole::Lock()
{
	::EnterCriticalSection(&g_cs_role);
}


//��ɫ����
void MyRole::UnLock()
{
	::LeaveCriticalSection(&g_cs_role);
}