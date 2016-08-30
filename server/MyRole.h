#pragma once

#include "stdafx.h"

#include "mysockserver.h"
/*
 
  角色类

 */

class MyRole
{
public:
	MyRole( RoleProperty* prop,Package package,Socket::MySOCKServer* netser);
	~MyRole(void);

	RoleProperty* getProp(){return &prop;};
	Package* getPackage(){return &package;};


	//最后一次更新的时间 
	DWORD lastupdatetime; 

	//角色加锁
	void Lock();
	//角色解锁
	void UnLock();
private:
	//角色的临界锁
	CRITICAL_SECTION g_cs_role;

	//角色属性
	RoleProperty prop;
	
	//角色包裹
	Package package;

	//网络
	Socket::MySOCKServer* netser;


};
