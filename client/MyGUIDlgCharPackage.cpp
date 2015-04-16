#include ".\myguidlgcharpackage.h"


//主角色指针 用宏来缩写避免长引用
#define 	main_char mgr->char_mgr->GetMainCharacter()

MyGUIDlgCharPackage::MyGUIDlgCharPackage(  MySceneManager* mgr)
{
	this->dev=getIrrDevice();
	this->mgr=mgr;

	//鼠标在gui界面里
	mouseInGui=false;

	ui=dev->getGUIEnvironment();
	smgr=dev->getSceneManager();
	drv=dev->getVideoDriver();

	//没有装备被选取.点起来
	selectedItemBox=false;

	//鼠标不在包裹栏里   
	mouseOnItem.X=-1;
	mouseOnItem.Y=-1;




}

MyGUIDlgCharPackage::~MyGUIDlgCharPackage(void)
{
}

bool MyGUIDlgCharPackage::LoadGUI()
{
	//装备框背景
	items_bg=drv->getTexture("ui/items.tbj");
	//武器图片
	item_pic=drv->getTexture("ui/exclusive_knight.tbj");
	item_pic2=drv->getTexture("ui/Exclusive_Infinity.tbj"); 

	//玄冰剑
	xbj_pic=drv->getTexture("ui/XBJ2.tbj");

	//使用品 hp mp等
	items_pic=drv->getTexture("ui/item.tbj");

	item_detail=drv->getTexture("ui/black.tbj");

	win = ui->addWindow(core::rect<s32>(0,0,260,360), false, L"");
	win->setRelativePosition(position2di(470,200));
	win->setVisible(false);

	win->getCloseButton()->setVisible(false);
	win->getCloseButton()->setToolTipText(L"");
	//win->getCloseButton()->setPosition(vector2d<s32>(0,-2)));


	//窗口的x 23编号 用来关闭
	IGUIButton* btn=ui->addButton(core::rect<s32>(0,0,15,14), win, 23, L"X" );
	btn->setRelativePosition(position2d<s32>(245,0));

	//显示 金钱:  的text
	IGUIStaticText* s=ui->addStaticText(L"金钱:",rect<s32>(0,0,70,20),false,true,win);
	s->setRelativePosition(position2d<s32>(5,330));

	
	//显示金钱数字
	money_text=ui->addStaticText(L"0",rect<s32>(0,0,50,20),false,true,win);
	money_text->setRelativePosition(position2d<s32>(80,330));
	money_text->setOverrideColor(SColor(255,0,0,0));



	return true;
}

//存储被选起的物品a,b行,列下表,的值到临时变量
void MyGUIDlgCharPackage::StorePackItemValue(int a,int b)
{
	selectedItemId=main_char->getPackage()->itembox[a][b];
	selectedItemparam1=main_char->getPackage()->itemParam1[a][b];
	selectedItemparam2=main_char->getPackage()->itemParam2[a][b];
	selectedItemparam3=main_char->getPackage()->itemParam3[a][b];
	selectedItemparam4=main_char->getPackage()->itemParam4[a][b];
	selectedItemparam5=main_char->getPackage()->itemParam5[a][b];
	selectedItemparam6=main_char->getPackage()->itemParam6[a][b];
}


//将选起的物品的临时变量设置包a,b行列的包裹内
void MyGUIDlgCharPackage::UseStorePackItemValue(int a,int b)
{
	//将物品放入到指定包裹栏
	main_char->getPackage()->itembox[a][b]=selectedItemId;

	main_char->getPackage()->itemParam1[a][b]=selectedItemparam1;
	main_char->getPackage()->itemParam2[a][b]=selectedItemparam2;
	main_char->getPackage()->itemParam3[a][b]=selectedItemparam3;
	main_char->getPackage()->itemParam4[a][b]=selectedItemparam4;
	main_char->getPackage()->itemParam5[a][b]=selectedItemparam5;
	main_char->getPackage()->itemParam6[a][b]=selectedItemparam6;
}


//鼠标拾取物品栏
void MyGUIDlgCharPackage::SelectItem()
{
	//如果鼠标指向了某物品,鼠标点起物品,放下.即移动物品的位置..

	//从窗口的区域得到物品框区域
	int x=r.UpperLeftCorner.X+10;
	int y=143+r.UpperLeftCorner.Y;



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
			if(rr.isPointInside(cursor) && selectedItemBox==false)
			{
				//拾取的物品栏物品空的,返回
				if(main_char->getPackage()->itembox[a][b]==0) break;
				//记录下点的物品id
				selectedItemBox=true;
				
				//存储被选起的物品值到临时变量
				StorePackItemValue(a,b);
				
				main_char->getPackage()->itembox[a][b]=0;
				sel_x=a;
				sel_y=b;

				drop_x=a;
				drop_y=b;

				return;
			}else if(rr.isPointInside(cursor) && selectedItemBox && main_char->getPackage()->itembox[a][b]==0){
				//表示第二次点了某框.而且框里没物品.就把刚才点下了的物品装过去

				//表示先点起了物品,然后又点到原来的位置. 同一个栏里.不做其他动作.物品恢复到原来的地方,拾取状态false
				if(sel_x==a && sel_y==b){
					selectedItemBox=false;

					//存储被选起的物品值到临时变量
					UseStorePackItemValue(a,b);
					
					return ;
				}

				//武器卸下到包裹栏 sel_x是上次在武器栏点起武器时标志的-1
				if(sel_x==-1 && sel_y==-1){
					selectedItemBox=false;
					
					//武器栏空了
					main_char->getPackage()->currWeapon=0;
					//角色武器模型空显示
					main_char->setCurWeapon(0,0);
					
					//被拾取的物品(这里是武器)放入包裹
					UseStorePackItemValue (a,b);

					//将武器卸下到指定包裹栏
					//main_char->getPackage()->itembox[a][b]=selectedItemId;
					//将卸下武器的参数保存到对应包裹栏
					//main_char->getPackage()->itemParam1[a][b]=selectedItemparam1;

					char tem[222];
					sprintf(tem,"卸下武器! 行%d 列%d",a,b);

					//这是卸下武器存放的地方
					memset((char*)&pack,'\0',sizeof(Package));
					pack.cmd=CMD_C2S_Request_ChangeWeapon; //请求客户包裹信息
					pack.col1=b;
					pack.row1=a;
					pack.currWeapon=main_char->getPackage()->currWeapon;
					//发送服务器,角色卸下武器
					mgr->client->SendMsg((char*)&pack,sizeof(Package));
					return;
				}

				//被拾取的物品放入包裹
				UseStorePackItemValue(a,b);

				//物品包裹栏 交换物品.点起,放下.位置变了..且放下的栏必须是空的
				//main_char->getPackage()->itembox[a][b]=selectedItemId;
				//拾起物品的参数保存
				//main_char->getPackage()->itemParam1[a][b]=selectedItemparam1;

				//包裹物品交换了.需要向服务器提交更新
				
				memset((char*)&pack,'\0',sizeof(Package));
				pack.cmd=CMD_C2S_Request_ChangePackageItem; //请求客户包裹信息
				pack.col1=sel_y;
				pack.row1=sel_x;
				pack.row2=a;
				pack.col2=b;
				mgr->client->SendMsg((char*)&pack,sizeof(Package));

				selectedItemBox=false;
				return;
			}
		}
	}

	//
	//物品被点起了.然后向场景里丢弃.,表示用户想丢该物品.
	if(selectedItemBox //标志有物品已被点起 
		 &&  selectedItemId!=0  //表示被选择起来的物品id不为空,即选取了物品 因为0定义为空物品
		&&	win->isPointInside(cursor)==false  //而且鼠标不在角色包裹窗口.内 
		&& sel_y!=-1 &&  sel_x!=-1  // 这两个变量等于-1表示拾取的物品不在包裹里. 不等于-1表示拾取的物品存在包裹里 .加这个判断防止丢弃拾取了武器栏的武器
		 //当前角色没打开npc商店.
		&&  mgr->myGUI_Mgr->getNpcShop()->win->isVisible() ==false
		){
			
			//状态恢复 
			selectedItemBox=false;
			selectedItemId=0;

			sel_y=-1;
			sel_x=-1;

			//丢该物品.
			pack.cmd=CMD_C2S_Request_DropPackageItem;
			//物品在包裹里的下标
			pack.row1=drop_x;
			pack.col1=drop_y;
			
			printf("角色丢物品! 行%d 列%d\n",sel_x,sel_y); 
			//MessageBox(0,tem,"",0);
			//发送服务器丢此物品
			mgr->client->SendMsg((char*)&pack,22); //不需要完整的数据包数据.只需要到22.发送row col的数据而已


	}


		//物品被点起了.然后向npc商店包裹窗口里点,表示用户想卖该物品给npc.
		if(selectedItemBox &&  //标志有物品已被点起 
			selectedItemId!=0  //表示被选择起来的物品id不为空,即选取了物品 因为0定义为空物品
			&&	win->isPointInside(cursor)==false  //而且鼠标不在角色包裹窗口.内 
			&& sel_y!=-1 &&  sel_x!=-1  // 这两个变量等于-1表示拾取的物品不在包裹里. 不等于-1表示拾取的物品存在包裹里 .加这个判断防止丢弃拾取了武器栏的武器
			&& mgr->myGUI_Mgr->getNpcShop()->win->isPointInside(cursor)==true  //而且鼠标在npc商店内 
			&&  mgr->myGUI_Mgr->getNpcShop()->win->isVisible() //npc商店窗口可见,说明打开了商店
			){

				//状态恢复 
				selectedItemBox=false;
				sel_y=-1;
				sel_x=-1;


				//卖该物品.
				pack.cmd=CMD_C2S_Request_SellPackageItem;
				//物品在包裹里的下标
				pack.row1=drop_x;
				pack.col1=drop_y;

				printf("角色卖物品! 行%d 列%d\n",sel_x,sel_y); 
				//MessageBox(0,tem,"",0);
				//发送服务器卖此物品
				mgr->client->SendMsg((char*)&pack,22); //不需要完整的数据包数据.只需要到22.发送row col的数据而已


		}



	//装备被点起了.然后向装备武器栏点.即想装上武器
	if(selectedItemBox && 
		selectedItemId!=0  //表示被选择起来的物品id不为空,即选取了物品 因为0定义为空物品
		){
		//武器区域被鼠标点了.表示用户想把武器装进去,而且当前角色curWeaponID=0 表示没武器呢

		//装武器到空的武器栏
		if(CurWeaponRect.isPointInside(cursor) && main_char->getPackage()->currWeapon==0 
			&& selectedItemId<1000 //1000以内为武器才能装到那里
			){
			//主角色的手上的武器编号更新
			main_char->getPackage()->currWeapon=selectedItemId;
			main_char->getPackage()->weaponparam1=selectedItemparam1;
			main_char->getPackage()->weaponparam2=selectedItemparam2;
			main_char->getPackage()->weaponparam3=selectedItemparam3;
			main_char->getPackage()->weaponparam4=selectedItemparam4;
			main_char->getPackage()->weaponparam5=selectedItemparam5;
			main_char->getPackage()->weaponparam6=selectedItemparam6;

			selectedItemBox=false;
			selectedItemId=0;
			selectedItemparam1=0;
			selectedItemparam2=0;
			selectedItemparam3=0;
			selectedItemparam4=0;
			selectedItemparam5=0;
			selectedItemparam6=0;

			//本地更新显示.  找到主角色,装上他的武器吧.只更新武器模型
			main_char->setCurWeapon(main_char->getPackage()->currWeapon,
				main_char->getPackage()->weaponparam1);

			//如果上次点起武器的下标-1表示点的位置不在包裹栏,而在武器栏.即这次装上武器动作为 先点起武器栏的武器,又点会武器栏
			if(sel_x==-1 && sel_y==-1){
				//武器信息没有变化.只本地更新显示武器而已.
				return ;
			}

			//这里就说明确实是先在物品栏里点起武器,再点到武器栏里.所以要向服务器更新装武器的动作了
			pack.cmd=CMD_C2S_Request_ChangeWeapon;
			//只需要给武器id行了.因为他给服务器发送更新武器信息.
			pack.currWeapon=main_char->getPackage()->currWeapon;
			pack.row1=sel_x;
			pack.col1=sel_y;
			char tem[222];
			sprintf(tem,"装上武器! 行%d 列%d",sel_x,sel_y); // 有时出现 -1 -1 表示先点了武器栏,又点回了武器栏.不在包裹里.所以下标是-1-1
		//	//MessageBox(0,tem,"",0);
			//发送服务器装上武器
			mgr->client->SendMsg((char*)&pack,sizeof(Package));


			return;
		}
	}



	//判断鼠标是否点了武器栏, 即用户是否想卸下武器
	if(CurWeaponRect.isPointInside(cursor) && selectedItemBox==false)
	{
		//武器栏没武器.不做什么
		if(main_char->getPackage()->currWeapon==0){

		}else if(main_char->getPackage()->currWeapon!=0){
			//武器栏有武器,把他拾起,准备放入物品包裹栏
			//标志,有物品被拾起
			selectedItemBox=true;
			//拾起武器的id保存到临时变量
			selectedItemId=main_char->getPackage()->currWeapon;
			//拾起武器的参数保存
			selectedItemparam1=main_char->getPackage()->weaponparam1;
			selectedItemparam2=main_char->getPackage()->weaponparam2;
			selectedItemparam3=main_char->getPackage()->weaponparam3;
			selectedItemparam4=main_char->getPackage()->weaponparam4;
			selectedItemparam5=main_char->getPackage()->weaponparam5;
			selectedItemparam6=main_char->getPackage()->weaponparam6;

			//当前武器栏id清空
			main_char->getPackage()->currWeapon=0;
			main_char->getPackage()->weaponparam1=0;
			main_char->getPackage()->weaponparam2=0;
			main_char->getPackage()->weaponparam3=0;
			main_char->getPackage()->weaponparam4=0;
			main_char->getPackage()->weaponparam5=0;
			main_char->getPackage()->weaponparam6=0;

			//表示不是点起了包裹里的物品.而是武器的
			sel_y=-1;
			sel_x=-1;
			//找到主角色,卸下他的武器吧.只本地更新显示武器模型
			main_char->setCurWeapon(0,0);
			

		}
	}



}




void MyGUIDlgCharPackage::OnEvent (const SEvent& e) 
{

	// 键盘输出事件
	if (e.EventType == EET_KEY_INPUT_EVENT) {

		//回车事件
		if(e.KeyInput.Key==KEY_RETURN && e.KeyInput.PressedDown==false) {

		}


		//打开关闭窗口
		if(e.KeyInput.Key==KEY_KEY_I && e.KeyInput.PressedDown==false ) {
			win->setVisible(!win->isVisible());
			if(win->isVisible()==false){
				mouseInGui=false;
			}else{
				//最前端
				ui->setFocus(win);

				//表示打开了包裹面版
				Package pack;
				memset((char*)&pack,'\0',sizeof(Package));
				pack.cmd=CMD_C2S_Request_Package; //请求客户包裹信息
				mgr->client->SendMsg((char*)&pack,sizeof(Package));
			}
		}


	}


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
				if(id==23){
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

//包裹界面绘制
void MyGUIDlgCharPackage::drawAll()
{
	//获取窗体绝对区域
	 r=win->getAbsolutePosition();

	 //包裹窗口可见
	if(win->isVisible()){
		//获取gui皮肤透明度
		SColor col = ui->getSkin()->getColor((EGUI_DEFAULT_COLOR)1);
		 alpha=col.getAlpha();

	    //绘制包裹格子背景图片
		drv->draw2DImage(items_bg, core::position2d<s32>(r.UpperLeftCorner.X+10 ,143+r.UpperLeftCorner.Y ),
		rect<s32>(0,0,212,143),0,SColor(alpha,255,255,255),true);
	}


	//绘制被鼠标移入到装备框,选择哪个框就绘制个矩形 紫色显示
	position2d<s32> pos=dev->getCursorControl()->getPosition();
	int x=r.UpperLeftCorner.X+10;
	int y=143+r.UpperLeftCorner.Y;
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
					drv->draw2DRectangleOutline(rr,SColor(alpha,255,255,0));
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

	//绘制所有装备框
	DrawAllItem();

	//绘制角色武器栏的.当前武器

	//包裹窗口打开了?
	if(win->isVisible())
	{
		//当前角色武器栏 没办法,窗口区域在变的话,他也要更新
		CurWeaponRect=rect<s32>(r.UpperLeftCorner.X+60,r.UpperLeftCorner.Y+100,r.UpperLeftCorner.X+95,r.UpperLeftCorner.Y+135);

		TextLeft( "武器:",vector2d<s32>(r.UpperLeftCorner.X+10,r.UpperLeftCorner.Y+110),SColor(alpha,255,0,255),18,0);
		
		//武器栏背景
		drv->draw2DImage(item_pic,
			vector2d<s32>(CurWeaponRect.UpperLeftCorner.X,CurWeaponRect.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
			rect<s32>(35*7,0,35*8,35),0, //这个是在源图象截取图片要显示的区域
			SColor(alpha,255,255,255),true);

		DrawItem(CurWeaponRect,main_char->getPackage()->currWeapon);
	//---------------------------------------------------------------------------------------------------------------------------

		//当前角色武器栏 没办法,窗口区域在变的话,他也要更新
		CurClothRect=rect<s32>(r.UpperLeftCorner.X+60+35,r.UpperLeftCorner.Y+60,r.UpperLeftCorner.X+95+35,r.UpperLeftCorner.Y+95);

		TextLeft( "服装:",vector2d<s32>(r.UpperLeftCorner.X+10+35,r.UpperLeftCorner.Y+70),SColor(alpha,255,0,255),18,0);
		//武器栏背景
		drv->draw2DImage(item_pic,
			vector2d<s32>(CurClothRect.UpperLeftCorner.X,CurClothRect.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
			rect<s32>(35*7,0,35*8,35),0, //这个是在源图象截取图片要显示的区域
			SColor(alpha,255,255,255),true);


	}

	//绘制被鼠标点起来了的装备
	if(selectedItemBox ){

		rect<s32> a(pos.X-10,pos.Y-10,pos.X,pos.Y);
		DrawItem(a,selectedItemId);
	}

	
	//显示物品详细信息,如果鼠标指向了物品栏的话
	if(win->isVisible() && mouseOnItem.X!=-1 && mouseOnItem.Y!=-1){

		//得到当前被鼠标指的物品id
		int itemid=main_char->getPackage()->itembox[mouseOnItem.X][mouseOnItem.Y];

		//物品栏空的
		if(itemid==0){
			return ;
		}

		//物品的参数1
		int itemparam1=main_char->getPackage()->itemParam1[mouseOnItem.X][mouseOnItem.Y];
		int itemparam2=main_char->getPackage()->itemParam2[mouseOnItem.X][mouseOnItem.Y];
		int itemparam3=main_char->getPackage()->itemParam3[mouseOnItem.X][mouseOnItem.Y];
		int itemparam4=main_char->getPackage()->itemParam4[mouseOnItem.X][mouseOnItem.Y];
		int itemparam5=main_char->getPackage()->itemParam5[mouseOnItem.X][mouseOnItem.Y];
		int itemparam6=main_char->getPackage()->itemParam6[mouseOnItem.X][mouseOnItem.Y];

		//绘制的黑色图片做背景
		drv->draw2DImage(item_detail,
			vector2d<s32>(position2di(pos.X-140,pos.Y-110)), //这个参数是指定绘制的坐标 即定位左上角位置绘制图片
			rect<s32>(0,0,150,200),0, //这个是在源图象截取图片要显示的区域 即截出图片的大小
			SColor(alpha,255,255,255),true);

		//物品名称的坐标.居中
		vector2d<s32> textPos(position2di((pos.X-150+75 //减去图片宽度,再加上一半宽度就是实际居中的x坐标
			),pos.Y-100));

		char* itemName=getItemNameById(itemid);
		char temp[255];
		
		//普通武器 
		if(itemparam1==0 && itemid<1000)
		{ 
			TextCenter(   itemName ,textPos,SColor(alpha,255,255,255),15,700);
		}else if(itemparam1>0 && itemid<1000){
			sprintf(temp,"[卓越]%s",itemName);
			TextCenter(   temp ,textPos,SColor(alpha,0,255,0),15,700);
		}else if(itemid>=1000 ){
			TextCenter(   itemName ,textPos,SColor(alpha,255,255,255),15,700);
		}
		
		
	
		//1000以内的物品id是武器
		if(itemid<1000){
			//得到当前被鼠标指的装备的攻击力
			int attack = itemparam2  ; //就是实际攻击力 都在服务端计算了的


			sprintf(temp,"攻击力:%d",attack);
			textPos.Y+=15;
			TextCenter(  temp ,textPos,SColor(alpha,255,255,255),14,500);
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
		
		TextCenter(  temp ,textPos,SColor(alpha,255,255,255),15,100);
		
		textPos.Y+=15;
		if(itemid<1000){
			sprintf(temp,"武器属性2:%d",itemparam2);
		}else if(itemid>=1000 && itemid<2000){
			sprintf(temp,"个数:%d",itemparam2);
		}else if(itemid>=2000 ){
			sprintf(temp,"param2:%d",itemparam2);
		}

		TextCenter(  temp ,textPos,SColor(alpha,255,255,255),15,100);

		textPos.Y+=15;
		sprintf(temp,"param3:%d",itemparam3);
		TextCenter(  temp ,textPos,SColor(alpha,255,255,255),15,100);

		textPos.Y+=15;
		sprintf(temp,"param4:%d",itemparam4);
		TextCenter(  temp ,textPos,SColor(alpha,255,255,255),15,100);

		textPos.Y+=15;
		sprintf(temp,"param5:%d",itemparam5);
		TextCenter(  temp ,textPos,SColor(alpha,255,255,255),15,100);

		textPos.Y+=15;
		sprintf(temp,"param6:%d",itemparam6);
		TextCenter(  temp ,textPos,SColor(alpha,255,255,255),15,100);

		delete itemName;
	}
}

char* MyGUIDlgCharPackage::getItemNameById(int itemID)
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

//传装备在图片的行和列,以及要绘制的开始区域.左上角
void MyGUIDlgCharPackage::draw2dImage(int row,int col,rect<s32> re)
{
	drv->draw2DImage(item_pic,
		vector2d<s32>(re.UpperLeftCorner.X,re.UpperLeftCorner.Y), //这个参数是指定绘制的坐标
		rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
		SColor(alpha,255,255,255),true);
}

//传来武器编号和绝对绘制rect.
void MyGUIDlgCharPackage::DrawItem(rect<s32> re,int itemID)
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

void MyGUIDlgCharPackage::DrawAllItem()
{
	irr::core::rect<s32> r=win->getAbsolutePosition();
	int x=r.UpperLeftCorner.X+10;
	int y=143+r.UpperLeftCorner.Y;

	//35.75 
	if(win->isVisible()){
		//行
		for(int a=0;a<4;a++){
			//列
			for(int b=0;b<6;b++){
				rect<s32> rr(x+b*35,y+a*35,x+b*35+35,y+a*35+35);

				//0编号表示空栏
				if((int)main_char->getPackage()->itembox[a][b]==0){
					//printf("itembox[%d][%d]无物品\n",a,b);
				}else{
				    DrawItem(rr,main_char->getPackage()->itembox[a][b]);
					//drv->draw2DRectangleOutline(rr,SColor(255,250,0,250));
				}
			}
		}
	}


}

void MyGUIDlgCharPackage::update()
{


	if(win->isVisible()){
		stringw t="";
		t+=main_char->getMoney();
		money_text->setText(t.c_str());

	}
	
}
