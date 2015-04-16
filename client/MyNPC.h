#pragma once

#include "stdafx.h"

#include "MyGUIDlgCommon.h"

#include "netclient.h"


/************************************************************************/
// 
//		npc类
//		
//
/************************************************************************/



class MyNPC
{
public:
	MyNPC( NPCProperty prop,Socket::CNetClient* client);

	~MyNPC(void);

	IAnimatedMeshSceneNode* getNode(){return node;};

	//接收事件
	void OnEvent(const SEvent& e);

	//更新
	void update();
	
	//ui绘制
	void UIRender();

	//
	void ShowTalkDlg();

	//卸载
	void drop();

	//得到npc属性
	NPCProperty getProp(){return npc_prop;};

	//鼠标指向了npc?
	bool mouseCurNPC;

	//鼠标指向了npc对话框
	bool mouseCurNPC_Dlg;


	core::line3d<f32> line;//碰撞点

private:
	IrrlichtDevice *dev;
	ISceneManager* smgr;					//场景管理
	IAnimatedMeshSceneNode* node;			//场景节点

	//显示npc名称的text节点
	ITextSceneNode*	char_textnode;

	Socket::CNetClient* client;

	//npc的属性
	NPCProperty npc_prop;

	//npc的对话框
	MyGUIDlgNpcTalk* dlg;


};
