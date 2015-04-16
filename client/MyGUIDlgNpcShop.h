#pragma once
#include "stdafx.h"

/************************************************************************/
// 
//		 npc商店面版 显示npc出售的物品.点击物品就购买了
//		
//
/************************************************************************/

#include "MySceneManager.h"

class MySceneManager;
class MyGUIDlgNpcShop
{
public:
	MyGUIDlgNpcShop(	MySceneManager* mgr);
	~MyGUIDlgNpcShop(void);


	void OnEvent (const irr::SEvent& e);
	void update();//更新
	void drawAll();

	void ShowDlg(bool fl);

	char* getItemNameById(int itemID);

	bool LoadGUI();
	//鼠标在面版内
	bool mouseInGui;

	void setShopPackage(ShopPackage* pk)
	{
		memset((char*)&shop,0,sizeof(ShopPackage));
		memcpy((char*)&shop,(char*)pk,sizeof(ShopPackage));
	}

		gui::IGUIWindow* win ;

private:

		void draw2dImage(int row,int col,rect<s32> re);

		void DrawAllItem();

		void DrawItem(rect<s32> re,int itemID);
		//鼠标拾取物品栏
		void SelectItem();

	irr::IrrlichtDevice *dev; 
	irr::video::IVideoDriver *drv; 
	irr::scene::ISceneManager *smgr; 
	irr::gui::IGUIEnvironment* ui;

	//主场景管理器
	MySceneManager* mgr;


	ITexture* item_detail;	// 用于显示物品详细信息的窗口
	video::ITexture* items_bg,*item_pic,*item_pic2; //纹理图片 用于包裹显示的物品图片

	ITexture* items_pic;
	ITexture* xbj_pic; //玄冰剑


	rect<s32> r;

	u32 alpha;

	vector2d<s32> mouseOnItem;//如果鼠标指向了物品.就给他的下标 用于显示物品详细信息 mouseOnItem x是行 y是列..


	//鼠标坐标
	vector2d<s32> cursor;

	//商店结构
	ShopPackage shop;
};
