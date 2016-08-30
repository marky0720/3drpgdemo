#include "StdAfx.h"
#include ".\myrole.h"

//初始化角色
MyRole::MyRole( RoleProperty* p,Package pack,Socket::MySOCKServer* netser)
{
	
	memcpy((char*)&prop,(char*)p,sizeof(RoleProperty));

	memcpy((char*)&package,(char*)&pack,sizeof(Package));

	this->netser=netser;
	

	lastupdatetime=GetTickCount();

	::InitializeCriticalSection(&g_cs_role);//初始化临界区对象

}

MyRole::~MyRole(void)
{
}

//角色加锁
void MyRole::Lock()
{
	::EnterCriticalSection(&g_cs_role);
}


//角色解锁
void MyRole::UnLock()
{
	::LeaveCriticalSection(&g_cs_role);
}