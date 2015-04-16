#pragma once



/************************************************************************/
// 
//		角色属性面版类  按C弹出
//		
//
/************************************************************************/

#include "stdafx.h"
#include "MySceneManager.h"





class MyGUIDlgCharProp
{
public:
	MyGUIDlgCharProp( MySceneManager* mgr);
	~MyGUIDlgCharProp(void);

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
	IGUIButton* addpoint_1;
	IGUIButton* addpoint_2;
	IGUIButton* addpoint_3;
	
	rect<s32> win_rect;

	u32 alpha; //gui的透明度 

	ITimer* m_timer;
	u32 lasttime;
};

