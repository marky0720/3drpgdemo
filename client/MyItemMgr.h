#pragma once

#include "stdafx.h"
#include "myitem.h"
#include "NetClient.h"

#include "MySceneManager.h"

/************************************************************************/
// 
//		  场景物品管理器 负责和服务器通讯场景的物品.
//		
//
/************************************************************************/

class MySceneManager;

class MyItemMgr
{
public:
	MyItemMgr( MySceneManager* mysmgr, Socket::CNetClient* net);
	~MyItemMgr(void);


	//是否所有场景地上物品是否可见
	void setAllItemVisiable(bool v);

	void insertItemData(const char* pData,const unsigned int DataLength);
	void SceneCMD_ITEMDROP_PROC(char* pData,unsigned long length);
	//物品消失
	void MyItemMgr::SceneCMD_ITEMDROPRemoveProc(char* pData,unsigned long length);

	void Quit(){m_bquit=true;};

	//更新场景物品.比如加载,卸载
	void update();

	////地图的掉落物品清空
	void RemoveAllItem();

	//得到所有场景的掉落物品
	void getSceneCMD_ITEMDROPAll();

	//显示物品名称
	void UIRender();


private:
	IrrlichtDevice *dev;


	//处理服务器发来的物品消息
	void  DoListThread( );







	std::list<MyItem*>  SceneItem_list;  //list存地面物品

	irr::core::list<ClientMsg*> msglist;    //存server传来的消息链,更新场景物品信息的
	CRITICAL_SECTION g_cs_msglist,g_cs_SceneItem_list; //临界

	MySceneManager* m_pMySceneMgr;


	//网络
	Socket::CNetClient* net;

	bool m_bquit; //退出
};
