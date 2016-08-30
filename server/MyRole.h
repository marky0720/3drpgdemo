#pragma once

#include "stdafx.h"

#include "mysockserver.h"
/*
 
  ��ɫ��

 */

class MyRole
{
public:
	MyRole( RoleProperty* prop,Package package,Socket::MySOCKServer* netser);
	~MyRole(void);

	RoleProperty* getProp(){return &prop;};
	Package* getPackage(){return &package;};


	//���һ�θ��µ�ʱ�� 
	DWORD lastupdatetime; 

	//��ɫ����
	void Lock();
	//��ɫ����
	void UnLock();
private:
	//��ɫ���ٽ���
	CRITICAL_SECTION g_cs_role;

	//��ɫ����
	RoleProperty prop;
	
	//��ɫ����
	Package package;

	//����
	Socket::MySOCKServer* netser;


};
