#pragma once



/************************************************************************/
// 
//		 gui配置窗口   按ESC弹出系统菜单,有此项 "gui配置"
//		
//
/************************************************************************/

#include "stdafx.h"



class MyGUIDlgConfig
{
public:
	MyGUIDlgConfig();
	~MyGUIDlgConfig(void);


	void OnEvent (const irr::SEvent& e);
	void update();//更新
	void drawAll();
	bool LoadGUI();

	//显示窗口
	void ShowDlg(bool fl);

	bool mouseInGui;
private:
	irr::IrrlichtDevice *dev;
	irr::video::IVideoDriver *drv; 
	irr::scene::ISceneManager *smgr; 
	irr::gui::IGUIEnvironment* ui;



	gui::IGUIWindow* win ;
};
