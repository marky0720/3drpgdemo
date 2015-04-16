#pragma once

#include "stdafx.h"


/************************************************************************/
// 
//		对话框类.选择对话框 yes,no的那种
//		
//
/************************************************************************/


class MyGUIDlgConfirm
{
public:
	MyGUIDlgConfirm();
	~MyGUIDlgConfirm(void);

	void OnEvent (const irr::SEvent& e);
	void update();//更新
	void drawAll();
	bool LoadGUI();

	void ShowDlg(bool v); 

	//鼠标在gui界面里
	bool mouseInGui;
private:
   irr::IrrlichtDevice *dev;
   irr::video::IVideoDriver *drv; 
   irr::scene::ISceneManager *smgr; 
   irr::gui::IGUIEnvironment* ui;

   gui::IGUIButton* btn_yes,*btn_no;
   gui::IGUIWindow* win ;

   //鼠标坐标
   vector2d<s32> cursor;

};
