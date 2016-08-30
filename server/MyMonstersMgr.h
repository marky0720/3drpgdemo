

/*
* 怪物对象管理类
*/

#if !defined(_H____INCLUDED_)
#define _H____INCLUDED_

#include "stdafx.h"


#include "mysockserver.h"


#include "mymonster.h"

#include "myitemmgr.h"

class MyMonstersMgr
{
public:
	MyMonstersMgr(MyRoleMgr* role_mgr ,Socket::MySOCKServer* ser ,MyItemMgr* m_pItem_mgr);
	~MyMonstersMgr(void);

	//读取所有怪物信息
	void ReadAllMonstersInfo();

	//加载场景地图刷怪文件
	void ReadSceneMonster();

	SMonsterInfo* getMonsterBaseInfoByTypeID(int type);



	void AddMonster(int TypeID,int ID,vector3df pos ,int worldmap);

	MyMonster* GetMonsterByID(int id);

	//得到所有怪物数量
	int getAllMonsterNum();

	//得到怪物所在世界地图
	int getMonsterWorldMap(int monsterID);

	void Update();

	//怪物遍历所有角色,看是否在视线内.在就返回角色指针
	RoleProperty* GetNearestRole(MyMonster* m);
	//根据怪物id让怪物被攻击
	MyMonster*  MyMonsterUnderAttack(int ID,int damage,SOCKET attacker);

	MyMonster* GetMonster(ISceneNode* node);




	void SendAMonsterToClient(SOCKET sock,int monsterID , int worldMap);
	void SendAllMonsterListToClient(SOCKET sock  , int worldMap);

	double getRandNum(int a,int b)
	{
		struct _timeb timebuffer;//定义一个结构
		_ftime(&timebuffer);//初始化timebuffer
		srand(timebuffer.millitm);//timebuffer.millitm获得毫秒

		int RANGE_MIN = a;
		int RANGE_MAX = b;

		double rand100 = (((double) rand() /(double) RAND_MAX) * RANGE_MAX + RANGE_MIN);
		return rand100;
	};

	int count;



private:
	MyItemMgr* m_pItem_mgr;

	MyRoleMgr* role_mgr ;



	//全局怪物文件配置信息
	std::list<SMonsterInfo*> monsters_info_list; //存全局怪物文件配置信息

	//怪物list加锁
	void MonsterListLock();

	//怪物list解锁
	void MonsterListUnLock();

	//定义怪物链表
	std::vector<MyMonster*> monster_list; //存怪物信息

	CRITICAL_SECTION g_cs_monsterlist; //临界


	//网络
	Socket::MySOCKServer* server;



	u32 lasttime;


};

#endif