#pragma once
#include "stdafx.h"

#include "NetClient.h"


/************************************************************************/
// 
//		npc对话框类,属于npc类的成员. 游戏里的角色点击npc后弹出该对话框.显示对应npc说的话.如果是商店npc.就有购买按钮.点按钮就向服务器发送请求购买的命令
//		
//
/************************************************************************/



class MyGUIDlgNpcTalk
{
public:
	MyGUIDlgNpcTalk(NPCProperty* npcprop , Socket::CNetClient* client);
	~MyGUIDlgNpcTalk(void);

	void OnEvent (const irr::SEvent& e);
	void update();//更新
	void drawAll();
	bool LoadGUI();
	
	void ShowDlg(bool v); 
	//销毁
	void drop();

	bool mouseInGui;

private:
	irr::IrrlichtDevice *dev;
	irr::video::IVideoDriver *drv; 
	irr::scene::ISceneManager *smgr; 
	irr::gui::IGUIEnvironment* ui;

	Socket::CNetClient* client;

	NPCProperty* npcprop;

	//鼠标坐标
	vector2d<s32> cursor;


	gui::IGUIButton* btn;
	gui::IGUIWindow* win ;
};
