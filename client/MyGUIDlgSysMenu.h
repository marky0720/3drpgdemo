#pragma once



/************************************************************************/
// 
//		 系统菜单类 按ESC弹出. 模式对话框
//		
//
/************************************************************************/


#include "stdafx.h"
#include "MyGUIDlgConfig.h"  //配置gui属性窗口



class MyGUIDlgSysMenu
{
public:
	MyGUIDlgSysMenu( );
	~MyGUIDlgSysMenu(void);


void OnEvent (const irr::SEvent& e);
void update();//更新
void drawAll();
bool LoadGUI();

bool mouseInGui;
private:
	irr::IrrlichtDevice *dev;
	irr::video::IVideoDriver *drv; 
	irr::scene::ISceneManager *smgr; 
	irr::gui::IGUIEnvironment* ui;


	MyGUIDlgConfig* config_dlg;

	gui::IGUIWindow* win ;

};
