#pragma once

#include "stdafx.h"
#include "MyNPC.h"

#include "MySceneManager.h"
/************************************************************************/
// 
//		NPC管理器.
//		
//
/************************************************************************/


class MyNPCMgr
{
public:
	MyNPCMgr( MySceneManager* mm);
	~MyNPCMgr(void);


	void MyNPCMgr::setVisiable(bool v);

	//插入npc消息到链表,来自服务器
	void insertNPCData(char* pData,int DataLength);

	//接收事件
	void OnEvent (const irr::SEvent & e);



	//添加一个npc到场景中
	void AddNPC(NPCProperty prop);
	
	//根据id得到npc
	MyNPC* GetNpcByID(int id);

	//移出场景所有NPC.用于切换场景时的.
	void RemoveAllNPC();

	//场景npc加载
	void LoadSceneNPC(int worldMap);

	//场景npc更新
	void update();

	void UIRender();


	bool IsselectedNPC(){return NPCselected;};
	MyNPC* GetselectedNPC(){return selectedNPCnode;};

	bool MouseInNPC_GUI;
private:
	IrrlichtDevice* dev;
	irr::scene::ISceneManager *smgr; 

	MySceneManager* m_pMySceneMgr;

	std::vector<ClientMsg*> msglist;    //存server传来的消息链,更新客户信息的

	CRITICAL_SECTION g_cs_npclist,g_cs_msglist; //临界

	std::vector<MyNPC*> npc_list;  //list存当前场景所有npc

	//当前npc所属场景地图id
	int currWorldMap;

	//被鼠标拾取的npc
	bool NPCselected;
	MyNPC* selectedNPCnode;
};
