#pragma once

#include "mysockserver.h"


#include ".\myrolemgr.h"

class MyRoleMgr;

class MyItemMgr
{
public:
	MyItemMgr(Socket::MySOCKServer* serve,MyRoleMgr* m_pRoleMgr);
	~MyItemMgr(void);

	// ������ӵ�����Ʒ;
	void addItem(SceneItem* item);
	//��ָ���û��������е�����Ʒ
	void SendAllSceneItem(SOCKET sock   ,int worldMap);

	int getAllItemNum( int worldMap);

	//��Ʒ������
	void ItemCollect(RoleProperty* role ,Package* pack);

	//��ָ���û��������е�����Ʒ����
	void SendAllSceneItemNum(SOCKET sock , int worldMap);

	//�����Ʒ������Ʒ����  ��Ʒ������,�Լ���ͼid    ItemDroprate����Ʒ���� 0-100   100-ItemDroprate���ǽ�Ǯ����
	void RandomItemDrop(int birthworldMap,vector3df pos,int ItemDroprate);



private:

	MyRoleMgr* m_pRoleMgr;

	//list����
	void ItemListLock();

	//list����
	void ItemListUnLock();


	CRITICAL_SECTION g_cs_itemlist;
	std::vector<SceneItem*> SceneItem_list;  //list�������Ʒ


	//����
	Socket::MySOCKServer* server;

};
