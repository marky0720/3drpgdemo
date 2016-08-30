#pragma once

#include "stdafx.h"

#include "MySOCKServer.h"

/************************************************************************/
// 
//		NPC��.һ������һ��npc.
//
//
/************************************************************************/



class MyNPC
{
public:
	MyNPC(NPCProperty* npcporp,Socket::MySOCKServer* netser);
	~MyNPC(void);

	//���npc�г��۵Ĺ��ܾʹ��ݸ�ָ��
	ShopPackage* shoppack;

	//npc����
	NPCProperty npc_prop;

	//��npc�̵������ļ�
	void ReadNPCShopConfig();

	void PushItemInShopPack(SItem item );

private:



	//����
	Socket::MySOCKServer* netser;



};
