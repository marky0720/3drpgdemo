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

	irr::core::list<STeleport*>* worldtelport_list;  //存所有世界地图的传送点

	//检查角色是否需要切换地图
	void MyRoleMgr::RoleCheckChangeMap(MyRole* role);

	// 场景添加角色
	void addRole(RoleProperty* r,Package pa);

	//得到一个角色
	MyRole* MyRoleMgr::getRoleByID(int ID);

	//角色购买npc商店,指定哪个npc 和他的商店物品下标 sock表示哪个角色
	bool RoleBuyNpcShop(SOCKET roleSOCK,char item_x,char item_y,ShopPackage* shoppack);

	//角色出售物品给npc商店
	void RoleSellPackageItemToNpcShop(SOCKET sock,char item_x,char item_y);

	//角色使用包裹里的hp恢复药剂
	void UsePackageHPItem(SOCKET sock);

	//得到所有在线角色数
	int getAllRoleNum();

	//  角色更新money
	int RoleGainMoney(SOCKET sock,int money);

	void RoleGainItem(SOCKET sock,int itembox,int itemparam1,int itemparam2,int itemparam3,int itemparam4,int itemparam5,int itemparam6);


	//角色销毁包裹里的某物品.xy是物品下标.
	void RoleDropItem(SOCKET sock,int x,int y);

	//得到角色所在的世界地图id
	int getRoleWorldMap(SOCKET sock);


	//角色获得经验
	void RoleGainExp(SOCKET sock,int gainexp);

	//发送roleSOCK的角色信息给sock.都在worldMap地图内
	void SendRoleToClient(SOCKET sock,int roleSOCK ,int worldMap);

	//包裹物品交换 两个物品行列
	void ChangePackageItem(SOCKET sock,int item1row,int item1col,int item2row,int item2col );

	//请求包裹
	void Response_Package(SOCKET sock);

	//请求角色的属性面版
	void SendRolePropToClient(SOCKET sock);

	//根据客户socket得到角色名
	char* getRoleNameBySocket(SOCKET);


	//角色需要更新.比如他在地图传送区域.就要帮他切换地图
	void Update();

	//角色换武器
	void ChangeWeapon(Package pack);

	//角色加点
	void RoleAddPoint(SOCKET sock,RoleProperty* rp);

	//遍历所有角色在pos的length距离内.在就返回角色指针
	RoleProperty* GetNearestRole(vector3df pos,int length ,int worldMap);

	//将所有在线用户数据发送给其他指定客户
	void SendAllRolelistToClient(SOCKET sock , int worldMap);

	//更新
	void UserUPDATEProc(char* pData,SOCKET sock,int DataLength);

	//移除角色信息list
	void RemoveRoleList(SOCKET sock);

	//更新角色信息list
	void UpdateRoleList(RoleProperty* r);

	//得到一个角色的攻击力,sock是他的标识
	int GetRoleAttackNum(SOCKET sock);

	//是否已存在
	bool isalreadyinlist(SOCKET sock);


	std::vector<MyRole*>* getRoleList(){return &rolelist;};

	//角色list加锁
	void RoleListLock();
	//角色list解锁
	void RoleListUnLock();
private:

	//网络
	Socket::MySOCKServer* server;

	CRITICAL_SECTION g_cs_rolelist;
	std::vector<MyRole*> rolelist;    //存连接的客户信息
};
