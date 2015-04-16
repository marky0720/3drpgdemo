#pragma once


#include "mycharactermgr.h"
#include "my3rdcamera.h"
#include "myitem.h" 
#include "MyMonstersMgr.h"
#include "myitemmgr.h"
#include "MyGUIManager.h" 
#include "mysceneloader.h"

#include "MyNPCMgr.h"
/************************************************************************/
// 
//		 场景管理类.统一管理场景的所有事情. .
//		
//
/************************************************************************/


class MyCharacterMgr;
class MyMonstersMgr;
class MyGUIManager;
class MyNPCMgr;
class MyItemMgr;

class MySceneManager
{
public:
	MySceneManager( Socket::CNetClient* client);
	~MySceneManager(void);
	
	//卸载当前世界场景
	void DropCurrScene();

	//加载指定id的世界场景
	void LoadScene(int id);

	//初始化摄象机
	void InitCamera();

	//初始化场景角色管理器
	void InitCharacterMgr();

	//初始化场景怪物管理器
	void InitMonsterMgr();

	//初始化场景npc管理器
	void InitNpcMgr();

	//初始化场景物品管理器
	void InitItemMgr();
	
	//初始化GUI界面管理器
	void InitGUIMgr();



	//接收事件,并分发给子管理器
	void OnEvent (const irr::SEvent& e);

	//更新
	void update();

	//3d渲染 
	void Render();

	//2d渲染 比如绘制2d图片
	void UIRender();

	//输出字体
	void TextRender();

	//切换世界
	void ChangeWorld(int worldMap);


	//屏幕显示字体
	void Show(char* s);

public:
		//引擎core
	            irr::IrrlichtDevice *dev; 
			    irr::video::IVideoDriver *drv; 
                irr::scene::ISceneManager *smgr; 


				
				
				//3rd摄象机
			    My3rdCamera* myCamera;

				//gui管理器
				MyGUIManager* myGUI_Mgr;

				//怪物管理器
				MyMonstersMgr* monster_mgr;

				//角色管理器
				MyCharacterMgr* char_mgr; 

				//NPC管理器
				MyNPCMgr* npc_mgr;

				//物品管理器
				MyItemMgr* item_mgr; 

				//场景加载卸载管理器
				MySceneLoader* myscene_loader;

				//网络引擎
				Socket::CNetClient* client;

				
				
				// mouse registry 
				bool mouseDownL,mouseDownR; 

				void beginchange();

				bool change;

				//是否切换世界? true时切换
				bool changeworld;
				//当前世界id
				int worldMap;


				int muoseclick;
				int lastmouseclick;

				

};
