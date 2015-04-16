#if !defined(_H____INCLUDED_)
#define _H____INCLUDED_


#include "mymonster.h"

#include "NetClient.h"

#include "mycharactermgr.h"


/************************************************************************/
// 
//		怪物对象管理类
//		
//
/************************************************************************/


class MyCharacterMgr;
class MySceneManager;

class MyMonstersMgr
{
public:
	MyMonstersMgr(  MySceneManager* mysmgr,Socket::CNetClient* net);
	~MyMonstersMgr(void);
	//接收事件
	 void OnEvent (const irr::SEvent& e);

	 //设置所有怪物是否可见
	 void setAllMonsterVisiable(bool v);

	 //怪物的网络数据到达
	void insertMonsterData(const char* pData,const unsigned int DataLength);

	void AddMonster(MonsterProperty prop);
	MyMonster* GetMonsterByID(int id);

	//移出所有在线怪物.用于切换场景时的.
	void RemoveAllMonsters();
	
	//向服务器请求所有的怪物信息
	void getMonsterCMD_MonsterAll();

	int getAllMonsterNum(){return monster_list.getSize();};

	void Update();
	void UIRender();

	void SetCharacter(MyCharacter* _user);

	MyMonster* GetMonster(ISceneNode* node);

	bool GetNodeIsselected(){return Nodeselected;};
	MyMonster* GetselectedMonster(){return selectednode;};

	void Quit(){m_bquit=true;};

private:

void MyMonstersMgr::MonsterCMD_LOGIN_PROC(char* pData,unsigned long length);
void MyMonstersMgr::MonsterCMD_UPDATE_PROC(char* pData,unsigned long length);
void MyMonstersMgr::MonsterCMD_UnderAttack_PROC(char* pData,unsigned long length);

	void  DoListThread( );



	MySceneManager* m_pMySceneMgr;

	CRITICAL_SECTION g_cs_msglist; //临界
	core::list<ClientMsg*> msglist;    //存server传来的消息链,更新客户信息的


	CRITICAL_SECTION g_cs_monsterlist; //临界
		//定义怪物链表
	core::list<MyMonster*>  monster_list;
	core::list<MyMonster*>::Iterator list_oper;

	//引擎指针
	IrrlichtDevice *device;		

	Socket::CNetClient* net;

	//场景管理
	scene::ISceneManager* smgr;


	//当前角色
	MyCharacter* user;

	//被鼠标拾取的怪物
	MyMonster* selectednode;
	bool Nodeselected;
	
	ITimer* timer;
	u32 lasttime;


	//显示被选怪物的text

	vector2d<s32> MonsterTextShowPos;

	bool m_bquit; //退出

};

#endif