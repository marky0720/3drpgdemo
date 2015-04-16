#pragma once


#include "MySceneManager.h"

#include "guicpp/CGUIProgressBar.h"
#include "guicpp/CImageGUISkin.h"
#include "guicpp/SkinLoader.h"

#include "myguidlgsysmenu.h"	//系统菜单面版

#include "myguidlgcharpackage.h"	//包裹面版

#include "myguidlgcharprop.h" //角色属性面版

#include "MyGUIDlgCharSkill.h" //角色技能面版

#include "MyGUIDlgNpcShop.h"  //npc商店面板

/************************************************************************/
// 
//		 gui总管理类
//		
//
/************************************************************************/


class MyGUIDlgCharPackage;
class MyGUIDlgCharProp;
class MyGUIDlgSysMenu;
class MyGUIDlgNpcShop;
class MyGUIDlgCharSkill;

class MyGUIManager
{
public:
	MyGUIManager (MySceneManager* m);
	 void OnEvent (const irr::SEvent& e);
	 void update();//更新
	 void drawAll();
	 bool LoadGUI();

	 void insertGUIData(const char* pData,const unsigned int DataLength);


	 //用户喊话的方法,把msg字符串显示到listbox上
	 void PostMsg(const wchar_t* msg);

	~MyGUIManager(void);

	bool GetEditIsInput(); //鼠标点击了ui,或回车,开启输入模式?

	//得到角色包裹gui指针
	MyGUIDlgCharPackage* getCharPackage(){return m_pGUIDlg_charPackage;};

	//得到npc商店gui指针
	MyGUIDlgNpcShop* getNpcShop(){return m_pGUIDlg_npcShop;};


private:
	irr::IrrlichtDevice *dev; 
	irr::video::IVideoDriver *drv; 
	 irr::scene::ISceneManager *smgr; 
	irr::gui::IGUIEnvironment* ui;

	MySceneManager* mgr;
//-键盘鼠标变量-----------------------------------------------------------
	// keyboard registry 
	bool keys[irr::KEY_KEY_CODES_COUNT]; 

	// mouse registry 
	bool mouseDownL; 
	bool mouseDownM; 
	bool mouseDownR; 
	f32 lastWheelMovement; 
	position2d<s32> cursor; 
	position2d<s32> cursorOld; 

//---------------------------------------------------------
	//ui元素变量
	IGUIListBox * msglist;
	IGUIEditBox* editbox;



	u32 last_time;
	bool guieditInput; //鼠标点击了ui,或回车,开启输入模式


	MyGUIDlgCharPackage* m_pGUIDlg_charPackage;
	MyGUIDlgCharProp* m_pGUIDlg_charProp;
	MyGUIDlgSysMenu* m_pGUIDlg_sysMenu;
	MyGUIDlgNpcShop* m_pGUIDlg_npcShop;
	MyGUIDlgCharSkill* m_pGUIDlg_skill;
//---------------------------------------------------------
public:

video::ITexture*  tex_HP,*tex_HPBG;
	float blood_per;  //血的百分比


video::ITexture* bottom_bar; //下面的快捷条




};
