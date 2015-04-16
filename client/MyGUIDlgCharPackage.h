#pragma once





/************************************************************************/
// 
//		 角色包裹面版 按I弹出
//		
//
/************************************************************************/



#include "stdafx.h"
#include "MySceneManager.h"

class MySceneManager;


class MyGUIDlgCharPackage
{
public:
	MyGUIDlgCharPackage(	MySceneManager* mgr);
	~MyGUIDlgCharPackage(void);
	void OnEvent (const irr::SEvent& e);
	void update();//更新
	void drawAll();

	bool LoadGUI();

	//显示窗口
	void ShowDlg(bool  v)
	{
	  win->setVisible(v);
	  //最前端
	  ui->setFocus(win);
	}


	void DrawItem(rect<s32> re,int itemID);
	void DrawAllItem();

	//鼠标在面版内
	bool mouseInGui;
private:

	//存储被选起的物品a,b行,列下表,的值到临时变量
	void MyGUIDlgCharPackage::StorePackItemValue(int a,int b);
	//将选起的物品的临时变量设置包a,b行列的包裹内
	void MyGUIDlgCharPackage::UseStorePackItemValue(int a,int b);


	 
	irr::IrrlichtDevice *dev; 
	irr::video::IVideoDriver *drv; 
	irr::scene::ISceneManager *smgr; 
	irr::gui::IGUIEnvironment* ui;

//主场景管理器
	MySceneManager* mgr;


	gui::IGUIWindow* win ;
	IGUIStaticText* money_text;

	//角色武器栏
	rect<s32> CurWeaponRect;

	//角色服装栏
	rect<s32> CurClothRect;


	//鼠标坐标
	vector2d<s32> cursor;


	//记录下最后点起的物品下标
	int drop_x,drop_y;

	//消耗品的图片
	ITexture* items_pic;

	video::ITexture* items_bg; //纹理图片 用于包裹显示的物品图片
	video::ITexture* item_pic,*item_pic2;
	ITexture* xbj_pic; //玄冰剑

public:
	int selectedItemId; //保存被选择了的物品id
private:
	
	bool selectedItemBox; //是否鼠标选择了某装备框.即点了一下他.把他拿起来的
	int selectedItemparam1,selectedItemparam2,selectedItemparam3,selectedItemparam4,selectedItemparam5,selectedItemparam6; //保存被选择了的物品param1
	int sel_x,sel_y; //保存上次被选择起的行列
	irr::core::rect<s32> r; //保存装备窗口的绝对rect

	u32 alpha; //gui的透明度 

	//包结构
	Package pack;

	vector2d<s32> mouseOnItem;//如果鼠标指向了物品.就给他的下标 用于显示物品详细信息 mouseOnItem x是行 y是列..
	ITexture* item_detail;	// 用于显示物品详细信息的窗口

	char* MyGUIDlgCharPackage::getItemNameById(int itemid);

protected:
	void draw2dImage(int row,int col,rect<s32> re);
	void SelectItem();
};
