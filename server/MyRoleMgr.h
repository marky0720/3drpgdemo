#pragma once

#include "stdafx.h"

#include "mysockserver.h"

#include "myrole.h"

#include "MyItemMgr.h"

class MyItemMgr;

class MyRoleMgr
{
public:
	MyRoleMgr(Socket::MySOCKServer* s );

	MyItemMgr* m_pItemMgr;

	~MyRoleMgr(void);

	irr::core::list<STeleport*>* worldtelport_list;  //�����������ͼ�Ĵ��͵�

	//����ɫ�Ƿ���Ҫ�л���ͼ
	void MyRoleMgr::RoleCheckChangeMap(MyRole* role);

	// ������ӽ�ɫ
	void addRole(RoleProperty* r,Package pa);

	//�õ�һ����ɫ
	MyRole* MyRoleMgr::getRoleByID(int ID);

	//��ɫ����npc�̵�,ָ���ĸ�npc �������̵���Ʒ�±� sock��ʾ�ĸ���ɫ
	bool RoleBuyNpcShop(SOCKET roleSOCK,char item_x,char item_y,ShopPackage* shoppack);

	//��ɫ������Ʒ��npc�̵�
	void RoleSellPackageItemToNpcShop(SOCKET sock,char item_x,char item_y);

	//��ɫʹ�ð������hp�ָ�ҩ��
	void UsePackageHPItem(SOCKET sock);

	//�õ��������߽�ɫ��
	int getAllRoleNum();

	//  ��ɫ����money
	int RoleGainMoney(SOCKET sock,int money);

	void RoleGainItem(SOCKET sock,int itembox,int itemparam1,int itemparam2,int itemparam3,int itemparam4,int itemparam5,int itemparam6);


	//��ɫ���ٰ������ĳ��Ʒ.xy����Ʒ�±�.
	void RoleDropItem(SOCKET sock,int x,int y);

	//�õ���ɫ���ڵ������ͼid
	int getRoleWorldMap(SOCKET sock);


	//��ɫ��þ���
	void RoleGainExp(SOCKET sock,int gainexp);

	//����roleSOCK�Ľ�ɫ��Ϣ��sock.����worldMap��ͼ��
	void SendRoleToClient(SOCKET sock,int roleSOCK ,int worldMap);

	//������Ʒ���� ������Ʒ����
	void ChangePackageItem(SOCKET sock,int item1row,int item1col,int item2row,int item2col );

	//�������
	void Response_Package(SOCKET sock);

	//�����ɫ���������
	void SendRolePropToClient(SOCKET sock);

	//���ݿͻ�socket�õ���ɫ��
	char* getRoleNameBySocket(SOCKET);


	//��ɫ��Ҫ����.�������ڵ�ͼ��������.��Ҫ�����л���ͼ
	void Update();

	//��ɫ������
	void ChangeWeapon(Package pack);

	//��ɫ�ӵ�
	void RoleAddPoint(SOCKET sock,RoleProperty* rp);

	//�������н�ɫ��pos��length������.�ھͷ��ؽ�ɫָ��
	RoleProperty* GetNearestRole(vector3df pos,int length ,int worldMap);

	//�����������û����ݷ��͸�����ָ���ͻ�
	void SendAllRolelistToClient(SOCKET sock , int worldMap);

	//����
	void UserUPDATEProc(char* pData,SOCKET sock,int DataLength);

	//�Ƴ���ɫ��Ϣlist
	void RemoveRoleList(SOCKET sock);

	//���½�ɫ��Ϣlist
	void UpdateRoleList(RoleProperty* r);

	//�õ�һ����ɫ�Ĺ�����,sock�����ı�ʶ
	int GetRoleAttackNum(SOCKET sock);

	//�Ƿ��Ѵ���
	bool isalreadyinlist(SOCKET sock);


	std::vector<MyRole*>* getRoleList(){return &rolelist;};

	//��ɫlist����
	void RoleListLock();
	//��ɫlist����
	void RoleListUnLock();
private:

	//����
	Socket::MySOCKServer* server;

	CRITICAL_SECTION g_cs_rolelist;
	std::vector<MyRole*> rolelist;    //�����ӵĿͻ���Ϣ
};
