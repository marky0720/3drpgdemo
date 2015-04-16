#pragma once

#include "stdafx.h"



/************************************************************************/
// 
//		 角色技能面版 按K弹出
//		
//
/************************************************************************/

#include "MySceneManager.h"


class MyGUIDlgCharSkill
{
public:
	MyGUIDlgCharSkill( MySceneManager* mgr);
	~MyGUIDlgCharSkill(void);
	void OnEvent (const irr::SEvent& e);
	void update();//更新
	void drawAll();

	bool LoadGUI();
	//鼠标在面版内
	bool mouseInGui;
private:
	//主场景管理器
	MySceneManager* mgr;

	irr::IrrlichtDevice *dev; 
	irr::video::IVideoDriver *drv; 
	irr::scene::ISceneManager *smgr; 
	irr::gui::IGUIEnvironment* ui;


	//鼠标坐标
	vector2d<s32> cursor;



	gui::IGUIWindow* win ;


	rect<s32> win_rect;

	u32 alpha; //gui的透明度 

	ITimer* m_timer;
	u32 lasttime;




};
