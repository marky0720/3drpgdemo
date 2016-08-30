#pragma once

#include "stdafx.h"

#include "MySOCKServer.h"

#include "MyNPC.h"

/************************************************************************/
// 
//		NPC管理器
//
//
/************************************************************************/



class MyNPCMgr
{
public:
	MyNPCMgr(Socket::MySOCKServer* s);
	~MyNPCMgr(void);

	//加载世界所有npc信息
	void LoadAllWorldNPC();

	//添加npc到list
	void AddNPC(NPCProperty npcprop);

	void SendNPCShopPackage(SOCKET sock,int npcID,int worldMap);

	MyNPC* getNPCbyID(int id,int worldMap);

private:

	std::vector<MyNPC*> npc_list;  //list存世界所有npc

	Socket::MySOCKServer* server;
};
