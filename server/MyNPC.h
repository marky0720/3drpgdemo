#pragma once

#include "stdafx.h"

#include "MySOCKServer.h"

/************************************************************************/
// 
//		NPC类.一个代表一个npc.
//
//
/************************************************************************/



class MyNPC
{
public:
	MyNPC(NPCProperty* npcporp,Socket::MySOCKServer* netser);
	~MyNPC(void);

	//如果npc有出售的功能就传递个指针
	ShopPackage* shoppack;

	//npc属性
	NPCProperty npc_prop;

	//读npc商店配置文件
	void ReadNPCShopConfig();

	void PushItemInShopPack(SItem item );

private:



	//网络
	Socket::MySOCKServer* netser;



};
