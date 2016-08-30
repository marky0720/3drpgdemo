#pragma once

#include "stdafx.h"

#include "MySOCKServer.h"

#include "MyNPC.h"

/************************************************************************/
// 
//		NPC������
//
//
/************************************************************************/



class MyNPCMgr
{
public:
	MyNPCMgr(Socket::MySOCKServer* s);
	~MyNPCMgr(void);

	//������������npc��Ϣ
	void LoadAllWorldNPC();

	//���npc��list
	void AddNPC(NPCProperty npcprop);

	void SendNPCShopPackage(SOCKET sock,int npcID,int worldMap);

	MyNPC* getNPCbyID(int id,int worldMap);

private:

	std::vector<MyNPC*> npc_list;  //list����������npc

	Socket::MySOCKServer* server;
};
