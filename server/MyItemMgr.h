#pragma once

#include "mysockserver.h"


#include ".\myrolemgr.h"

class MyRoleMgr;

class MyItemMgr
{
public:
	MyItemMgr(Socket::MySOCKServer* serve,MyRoleMgr* m_pRoleMgr);
	~MyItemMgr(void);

	// 场景添加掉落物品;
	void addItem(SceneItem* item);
	//向指定用户发送所有掉落物品
	void SendAllSceneItem(SOCKET sock   ,int worldMap);

	int getAllItemNum( int worldMap);

	//物品被捡了
	void ItemCollect(RoleProperty* role ,Package* pack);

	//向指定用户发送所有掉落物品数量
	void SendAllSceneItemNum(SOCKET sock , int worldMap);

	//随机物品掉落物品生成  物品出生点,以及地图id    ItemDroprate是物品暴率 0-100   100-ItemDroprate就是金钱暴率
	void RandomItemDrop(int birthworldMap,vector3df pos,int ItemDroprate);



private:

	MyRoleMgr* m_pRoleMgr;

	//list加锁
	void ItemListLock();

	//list解锁
	void ItemListUnLock();


	CRITICAL_SECTION g_cs_itemlist;
	std::vector<SceneItem*> SceneItem_list;  //list存地面物品


	//网络
	Socket::MySOCKServer* server;

};
