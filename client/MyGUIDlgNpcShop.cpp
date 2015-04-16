#include ".\myguidlgnpcshop.h"



MyGUIDlgNpcShop::MyGUIDlgNpcShop( MySceneManager* mgr)
{
	this->dev=getIrrDevice();
	this->mgr=mgr;

	//鼠标在gui界面里
	mouseInGui=false;

	ui=dev->getGUIEnvironment();
	smgr=dev->getSceneManager();
	drv=dev->getVideoDriver();



	//鼠标不在包裹栏里   
	mouseOnItem.X=-1;
	mouseOnItem.Y=-1;

}

MyGUIDlgNpcShop::~MyGUIDlgNpcShop(void)
{
}

void MyGUIDlgNpcShop::ShowDlg(bool fl)
{
	win->setVisible(fl);
	//最前端
	ui->setFocus(win);
}


bool MyGUIDlgNpcShop::LoadGUI()
{
	//物品框背景
	items_bg=drv->getTexture("ui/items.tbj");
	item_detail=drv->getTexture("ui/black.tbj");
	
	//玄冰剑
	xbj_pic=drv->getTexture("ui/XBJ2.tbj");

	//武器图片
	item_pic=drv->getTexture("ui/exclusive_knight.tbj");
	item_pic2=drv->getTexture("ui/Exclusive_Infinity.tbj"); 

	//使用品 hp mp等
	items_pic=drv->getTexture("ui/item.tbj");

	win = ui->addWindow(core::rect<s32>(0,0,240,200), false, L"");
	win->setRelativePosition(position2di(200,200));

	win->setVisible(false);

	win->getCloseButton()->setVisible(false);
	win->getCloseButton()->setToolTipText(L"");
	//win->getCloseButton()->setPosition(vector2d<s32>(0,-2)));
	
	//窗口的x 35编号 用来关闭
	IGUIButton* btn=ui->addButton(core::rect<s32>(0,0,15,14), win, 35, L"X" );
	btn->setRelativePosition(position2d<s32>(225,0));
	btn->setToolTipText(L"");
	
	return true;
}


//鼠标拾取物品栏
void MyGUIDlgNpcShop::SelectItem()
{
	//如果鼠标指向了某物品,鼠标点起物品,放下.即移动物品的位置..

	//从窗口的区域得到物品框区域
	int x=r.UpperLeftCorner.X+10;
	int y=20+r.UpperLeftCorner.Y;

	//行
	for(int a=0;a<4;a++)
	{
		//列
		for(int b=0;b<6;b++)
		{
			//构造绝对区域
			rect<s32> rr(x+b*35,y+a*35,x+b*35+35,y+a*35+35);
			//判断鼠标是否指向了物品,表示第一次点了下某物品栏 

			//物品包裹栏 拾取物品.点起
			if(rr.isPointInside(cursor) )
			{
				//拾取的物品栏物品是空的,返回
				if( shop.itembox[a][b]==0) break;
				//否则记录下点的物品id
				
				//如果角色包裹有物品被拾起.鼠标却又指向了npc商店里点击.表示用户想出售物品.而不是买npc商店物品.
				//if(mgr->myGUI_Mgr->getCharPackage()->selectedItemId!=0){
				//		MessageBox(0,"角色包裹点起","",0);				
				//	return ;//返回
				//}
				mouseOnItem.X=a;
				mouseOnItem.Y=b;

				//请求购买商店物品.传送物品所在的x,y
			
				shop.cmd=CMD_C2S_UserBuyShop;
				shop.row1=b;
				shop.col1=a;
				

				mgr->client->SendMsg((char*)&shop,6);
			}
		}
	}


}


void MyGUIDlgNpcShop::OnEvent (const SEvent& e) 
{

	if(e.EventType ==irr::EET_MOUSE_INPUT_EVENT)//鼠标事件
	{
		switch (e.MouseInput.Event)//判断鼠标事件
		{
			//左键按下
		case EMIE_LMOUSE_PRESSED_DOWN :
			{
				cursor.X = e.MouseInput.X; 
				cursor.Y = e.MouseInput.Y; 
				if(win->isVisible()){
					SelectItem();
					if( win->isPointInside(cursor)){
						mouseInGui=true;
					}else{
						mouseInGui=false;
					}
				}

				break;
			}





		}
	}




	//gui事件
	if (e.EventType == EET_GUI_EVENT)
	{
		s32 id = e.GUIEvent.Caller->getID();//元素对象的id
		//e.GUIEvent.Caller就是发生事件的gui元素对象.
		switch(e.GUIEvent.EventType)
		{


			
		case EGET_BUTTON_CLICKED:
			{

				//点了窗口右上角的X
				if(id==35){

					//商店窗口关闭的同时,角色包裹也自动关闭.也可以和角色包裹的x按钮共享一个id.只要按了id按钮就关闭(隐藏)两个窗口
					if(win->isVisible()){
						mgr->myGUI_Mgr->getCharPackage()->ShowDlg(false);
					}
					win->setVisible(!win->isVisible());
					if(win->isVisible()==false){
						mouseInGui=false;
					}
				}
				break;
			}

		}
	}


}

void MyGUIDlgNpcShop::DrawAllItem()
{
	irr::core::rect<s32> r=win->getAbsolutePosition();
	int x=r.UpperLeftCorner.X+10;
	int y=20+r.UpperLeftCorner.Y;

	//35.75 
	if(win->isVisible()){
		//行
		for(int a=0;a<4;a++){
			//列
			for(int b=0;b<6;b++){
				rect<s32> rr(x+b*35,y+a*35,x+b*35+35,y+a*35+35);

				//0编号表示栏里无物品.则不绘制
				if((int)shop.itembox[a][b]==0){
					//printf("itembox[%d][%d]无物品\n",a,b);
				}else{
					DrawItem(rr,shop.itembox[a][b]);
					//drv->draw2DRectangleOutline(rr,SColor(255,250,0,250));
				}
			}
		}
	}


}


//传装备在图片的行和列,以及要绘制的开始区域.左上角
void MyGUIDlgNpcShop::draw2dImage(int row,int col,rect<s32> re)
{
	drv->draw2DImage(item_pic,
		vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
		rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
		SColor(alpha,255,255,255),true);
}


//传来武器编号和绝对绘制rect.
void MyGUIDlgNpcShop::DrawItem(rect<s32> re,int itemID)
{

	switch(itemID)
	{
		//双刃剑
	case SRJ_Weapon:
		{

			int col=8; //列   控制宽度x
			int row=4; //行 下标0开始的 控制高度y
			draw2dImage(row,col,re);

			break;
		}

		//天无剑
	case TWJ_Weapon:
		{
			int col=11; //列   控制宽度x 横着的
			int row=3; //行 下标0开始的 控制高度y 竖着的
			draw2dImage(row,col,re);
			break;
		}

		//楚王剑
	case CWJ_Weapon:
		{
			int col=11; //列   控制宽度x 横着的
			int row=2; //行 下标0开始的 控制高度y 竖着的
			draw2dImage(row,col,re);
			break;
		}

		//机械阔剑
	case JXKJ_Weapon:
		{
			int col=9; //列   控制宽度x 横着的
			int row=4; //行 下标0开始的 控制高度y 竖着的
			draw2dImage(row,col,re);
			break;
		}


		//大马格氏剑
	case DMGSJ_Weapon:
		{
			int col=6; //列   控制宽度x 横着的
			int row=5; //行 下标0开始的 控制高度y 竖着的
			draw2dImage(row,col,re);
			break;
		}


		//玄冰剑
	case XBJ_Weapon:
		{
			drv->draw2DImage(xbj_pic,
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(0,0,34,34),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}


		//白蓮刀
	case BLD_Weapon:
		{
			int col=12; //列   控制宽度x 横着的
			int row=2; //行 下标0开始的 控制高度y 竖着的
			draw2dImage(row,col,re);
			break;
		}

		//熟铁刀
	case STD_Weapon:
		{
			int col=12; //列   控制宽度x 横着的
			int row=3; //行 下标0开始的 控制高度y 竖着的
			draw2dImage(row,col,re);
			break;
		}

		//7彩剑
	case Cai7_Weapon:
		{
			int col=0; //列   控制宽度x 横着的
			int row=4; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(item_pic2,
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}

		//巨齿剑
	case JC_Weapon:
		{
			int col=2; //列   控制宽度x 横着的
			int row=4; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(item_pic2,
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}


		//薄双刃剑
	case BSRJ_Weapon:
		{
			int col=1; //列   控制宽度x 横着的
			int row=4; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(drv->getTexture("ui/Exclusive_Infinity.tbj"),
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}


		//弯刃剑
	case WRJ_Weapon:
		{
			int col=1; //列   控制宽度x 横着的
			int row=1; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(drv->getTexture("ui/funny_weapon.tbj"),
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}

		//蓝弯刃剑
	case LWRJ_Weapon:
		{
			int col=2; //列   控制宽度x 横着的
			int row=1; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(drv->getTexture("ui/funny_weapon.tbj"),
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}


		//莲花剑
	case LHJ_Weapon:
		{
			int col=6; //列   控制宽度x 横着的
			int row=1; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(drv->getTexture("ui/funny_weapon.tbj"),
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}


		//白龙剑
	case BLJ_Weapon:
		{
			int col=12; //列   控制宽度x 横着的
			int row=4; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(item_pic,
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}


		//剑皇
	case 17:
		{
			int col=7; //列   控制宽度x 横着的
			int row=0; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(drv->getTexture("ui/funny_weapon.tbj"),
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}


		//超级金疮药
	case 1000:
		{
			int col=6; //列   控制宽度x 横着的
			int row=0; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(items_pic,
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}

		//超级魔法药
	case 1001:
		{
			int col=8; //列   控制宽度x 横着的
			int row=0; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(items_pic,
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}

		//男布衣
	case 2000:
		{
			int col=3; //列   控制宽度x 横着的
			int row=2; //行 下标0开始的 控制高度y 竖着的
			drv->draw2DImage(drv->getTexture("ui/itemui.tbj"),
				vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
				rect<s32>(36*col,36*row,36*(col+1),36*(row+1)),0, //这个是在源图象截取图片要显示的区域
				SColor(alpha,255,255,255),true);
			break;
		}


	}
}


void MyGUIDlgNpcShop::drawAll()
{
	//获取窗体绝对区域
	r=win->getAbsolutePosition();
	if(win->isVisible()){
		//获取gui皮肤透明度
		SColor col = ui->getSkin()->getColor((EGUI_DEFAULT_COLOR)1);
		alpha=col.getAlpha();

		//绘制包裹格子背景图片
		drv->draw2DImage(items_bg, core::position2d<s32>(r.UpperLeftCorner.X+10 ,20+r.UpperLeftCorner.Y ),
			rect<s32>(0,0,212,143),0,SColor(alpha,255,255,255),true);
	}

	//绘制被鼠标移入到装备框,选择哪个框就绘制个矩形 紫色显示
	position2d<s32> pos=dev->getCursorControl()->getPosition();
	int x=r.UpperLeftCorner.X+10;
	int y=20+r.UpperLeftCorner.Y;
	//35.75 
	if(win->isVisible())
	{
		bool isin=false;
		//行
		for(int a=0;a<4;a++)
		{
			//列
			for(int b=0;b<6;b++)
			{
				rect<s32> rr(x+b*35,y+a*35,x+b*35+35,y+a*35+35);
				if(rr.isPointInside(pos))
				{
					//	drv->draw2DRectangleOutline(rr,SColor(alpha,0,0,205),SColor(alpha,0,0,205),SColor(alpha,0,0,205),SColor(alpha,0,0,205));
					drv->draw2DRectangleOutline(rr,SColor(alpha,255,255,0 ));
					//只绘制一个..然后返回.得到包裹的下标
					mouseOnItem.X=a; //行
					mouseOnItem.Y=b; //列

					isin=true;
					break;
				}else{
					mouseOnItem.X=-1; //行
					mouseOnItem.Y=-1; //列
				}
			}
			if(isin) break;
		}
	}

	DrawAllItem();


	//显示物品详细信息,如果鼠标指向了物品栏的话
	if(win->isVisible() && mouseOnItem.X!=-1 && mouseOnItem.Y!=-1){

		//得到当前被鼠标指的物品id
		int itemid=shop.itembox[mouseOnItem.X][mouseOnItem.Y];

		//物品栏空的
		if(itemid==0){
			return ;
		}

		//物品的参数1
		int itemparam1=shop.itemParam1[mouseOnItem.X][mouseOnItem.Y];
		int itemparam2=shop.itemParam2[mouseOnItem.X][mouseOnItem.Y];
		int itemparam3=shop.itemParam3[mouseOnItem.X][mouseOnItem.Y];
		int itemparam4=shop.itemParam4[mouseOnItem.X][mouseOnItem.Y];
		int itemparam5=shop.itemParam5[mouseOnItem.X][mouseOnItem.Y];
		int itemparam6=shop.itemParam6[mouseOnItem.X][mouseOnItem.Y];

		drv->draw2DImage(item_detail,
			vector2d<s32>(position2di(pos.X-160,pos.Y-110)), //这个参数是指定绘制的坐标 即定位左上角位置绘制图片
			rect<s32>(0,0,200,250),0, //这个是在源图象截取图片要显示的区域 即截出图片的大小
			SColor(alpha,255,255,255),true);
		//物品名称的坐标.居中
		vector2d<s32> textPos(position2di((pos.X-150+75 //减去图片宽度,再加上一半宽度就是实际居中的x坐标
			),pos.Y-100));
		char* itemName=getItemNameById(itemid);
		char temp[255];

		//普通武器
		if(itemparam1==0 && itemid<1000 )
		{ 
			char strtemp[255];
			sprintf(strtemp,"%s 价格:%d",itemName,shop.itemPrice[mouseOnItem.X][mouseOnItem.Y]);
			TextCenter( strtemp ,textPos,SColor(alpha,255,255,255),15,700);
		}else if(itemparam1>0 && itemid<1000){
				//1000以内 光武
			char strtemp[255];
			sprintf(strtemp,"[卓越]%s 价格:%d",itemName,shop.itemPrice[mouseOnItem.X][mouseOnItem.Y]);

			TextCenter( strtemp ,textPos,SColor(alpha,0,255,0),15,700);
		}else if( itemid>=1000 && itemid<2000){
			char strtemp[255];
			sprintf(strtemp,"%s 价格:%d",itemName,shop.itemPrice[mouseOnItem.X][mouseOnItem.Y]);
			TextCenter( strtemp ,textPos,SColor(alpha,255,255,255),15,700);
		}else if( itemid>=2000 && itemid<3000){
			char strtemp[255];
			sprintf(strtemp,"%s 价格:%d",itemName,shop.itemPrice[mouseOnItem.X][mouseOnItem.Y]);
			TextCenter( strtemp ,textPos,SColor(alpha,255,255,255),15,700);
		}


		//1000以内的物品id是武器
		if(itemid<1000){
			//得到当前被鼠标指的装备的攻击力
			int attack=itemparam2; //就是实际的攻击力
			

			sprintf(temp,"攻击力:%d",attack);
			textPos.Y+=15;
			TextCenter( temp ,textPos,SColor(alpha,255,255,255),14,500);
		}

		
		textPos.Y+=15;
		if(itemid<1000){
			sprintf(temp,"武器属性1:%d",itemparam1);
			if(itemparam1==1)
				//紫色
				sprintf(temp,"属性:紫岚");
			else if(itemparam1==2)
				//乱里的火
				sprintf(temp,"属性:火",itemName);

			else if(itemparam1==3)
				//乱里的冰
				sprintf(temp,"属性:冰",itemName);

			else if(itemparam1==4)
				//乱里的毒
				sprintf(temp,"属性:毒",itemName);
			else if(itemparam1==5)
				//7彩
				sprintf(temp,"属性:七彩",itemName);
		}else if(itemid==1000){
			sprintf(temp,"HP立即恢复:%d",itemparam1);
		}else if(itemid==1001){
			sprintf(temp,"MP立即恢复:%d",itemparam1);	
		}else if(itemid>=2000){
			sprintf(temp,"param1:%d",itemparam1);	
		}

		TextCenter( temp ,textPos,SColor(alpha,255,255,255),15,100);

		//if(itemparam1==1){
		//	//紫色
		//	sprintf(temp,"属性:紫岚");
		//	TextCenter(ui, temp ,textPos,SColor(alpha,255,255,255),15,700);
		//}else if(itemparam1==2){
		//	//乱里的火
		//	sprintf(temp,"属性:火",itemName);
		//	TextCenter(ui, temp ,textPos,SColor(alpha,255,255,255),15,700);
		//}else if(itemparam1==3){
		//	//乱里的冰
		//	sprintf(temp,"属性:冰",itemName);
		//	TextCenter(ui, temp ,textPos,SColor(alpha,255,255,255),15,700);
		//}else if(itemparam1==4){
		//	//乱里的毒
		//	sprintf(temp,"属性:毒",itemName);
		//	TextCenter(ui, temp ,textPos,SColor(alpha,255,255,2550),15,700);
		//}else if(itemparam1==5){
		//	//7彩
		//	sprintf(temp,"属性:七彩",itemName);
		//	TextCenter(ui, temp ,textPos,SColor(alpha,255,255,2550),15,700);
		//}

		textPos.Y+=15;
		if(itemid<1000){
			sprintf(temp,"武器属性2:%d",itemparam2);
		}else if(itemid>=1000 && itemid<2000){
			sprintf(temp,"个数:%d",itemparam2);
		}else if(itemid>=2000 ){
			sprintf(temp,"param2:%d",itemparam2);
		}

		TextCenter( temp ,textPos,SColor(alpha,255,255,255),15,100);

		textPos.Y+=15;
		sprintf(temp,"param3:%d",itemparam3);
		TextCenter( temp ,textPos,SColor(alpha,255,255,255),15,100);

		textPos.Y+=15;
		sprintf(temp,"param4:%d",itemparam4);
		TextCenter( temp ,textPos,SColor(alpha,255,255,255),15,100);

		textPos.Y+=15;
		sprintf(temp,"param5:%d",itemparam5);
		TextCenter( temp ,textPos,SColor(alpha,255,255,255),15,100);

		textPos.Y+=15;
		sprintf(temp,"param6:%d",itemparam6);
		TextCenter( temp ,textPos,SColor(alpha,255,255,255),15,100);


		delete itemName;

	}


}

char* MyGUIDlgNpcShop::getItemNameById(int itemID)
{

	//物品配置信息
	char FilePath[255]="scene\\item.ini";
	char *inBuf=new char[80];     //用于保存读取的字符串
	char temp[25]="";
	sprintf(temp,"ItemID:%d",itemID); //id

	if(IniGetString(FilePath,temp,"Name",inBuf)){
		return inBuf;
	}else{
		return 0;
	}


}


void MyGUIDlgNpcShop::update()
{
	
	if(win->isVisible()){

	}

}
