#include ".\myguidlgcharskill.h"

MyGUIDlgCharSkill::MyGUIDlgCharSkill(MySceneManager* mgr)
{
	this->dev=getIrrDevice();
	smgr=dev->getSceneManager();
	ui=dev->getGUIEnvironment();
	drv=dev->getVideoDriver();

	this->mgr=mgr;

	mouseInGui=false;

	m_timer=dev->getTimer();
	lasttime=m_timer->getRealTime();
}

MyGUIDlgCharSkill::~MyGUIDlgCharSkill(void)
{
}
void MyGUIDlgCharSkill::OnEvent (const irr::SEvent& e)
{
	// 键盘输出事件
	if (e.EventType == EET_KEY_INPUT_EVENT) {

		//打开/关闭窗口
		if(e.KeyInput.Key==KEY_KEY_K && e.KeyInput.PressedDown==false ) {
			win->setVisible(!win->isVisible());
			if(win->isVisible()==false){
				mouseInGui=false;
			}else{
				//最前端显示
				ui->setFocus(win);

				//表示打开了属性面版
				RoleProperty srp;
				memset((char*)&srp,'\0',sizeof(RoleProperty));
				srp.cmd=CMD_C2S_Request_RoleSkillTable; //请求技能面版信息
				mgr->client->SendMsg((char*)&srp,sizeof(RoleProperty));
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
				if(id==38){
					win->setVisible(!win->isVisible());
					if(win->isVisible()==false){
						mouseInGui=false;
					}
					break;
				}


			}

		}
	}






}


void MyGUIDlgCharSkill::update()
{

}


void MyGUIDlgCharSkill::drawAll()
{
	SColor color = ui->getSkin()->getColor((EGUI_DEFAULT_COLOR)1);
	alpha=color.getAlpha();

	//放技能框的背景
	drv->draw2DImage(drv->getTexture("ui/Interface_main.tbj"),
		vector2d<s32>(5,60), //这个参数是指定绘制的坐标 即定位左上角位置绘制图片
		rect<s32>(0,164,40,220),0, //这个是在源图象截取图片要显示的区域 即截出图片的大小
		SColor(alpha,255,255,255),true);



	//获得本窗体的绝对坐标
	win_rect=win->getAbsolutePosition();

	//如果窗体可见
	if(win->isVisible()){
		

		int x=win_rect.UpperLeftCorner.X+10;
		int y=20+win_rect.UpperLeftCorner.Y;


		int col=9;
		int row=2;

		//黑色背景
		drv->draw2DImage(drv->getTexture("ui/black.tbj"),
			vector2d<s32>(position2di(x-3,y-3)), //这个参数是指定绘制的坐标 即定位左上角位置绘制图片
			rect<s32>(0,0,225,35+5),0, //这个是在源图象截取图片要显示的区域 即截出图片的大小
			SColor(alpha,255,255,255),true);

		//绘制技能图片
		drv->draw2DImage(drv->getTexture("ui/skill_2.tbj"),
			vector2d<s32>(x,y), //这个参数是指定绘制的坐标
			rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
			SColor(alpha,255,255,255),true);


		//物品名称的坐标.居中
		vector2d<s32> textPos(position2di(x+40,y+10));

		TextLeft( "技能名称:" ,textPos,SColor(alpha,255,255,255),12,0);
		textPos.X+=12*5;
		TextLeft( "[极光护体]" ,textPos,SColor(alpha,255,255,0),12,0);


//--------------------------------------------------------------------------------------------------------
		//黑色背景
		drv->draw2DImage(drv->getTexture("ui/black.tbj"),
			vector2d<s32>(position2di(x-3,y+40+3)), //这个参数是指定绘制的坐标 即定位左上角位置绘制图片
			rect<s32>(0,0,225,35+5),0, //这个是在源图象截取图片要显示的区域 即截出图片的大小
			SColor(alpha,255,255,255),true);

		 col=5;
		 row=2;
		//绘制技能图片
		drv->draw2DImage(drv->getTexture("ui/skill_magic3.tbj"),
			vector2d<s32>(x,y+40+3+3), //这个参数是指定绘制的坐标
			rect<s32>(35*col,35*row,35*(col+1),35*(row+1)),0, //这个是在源图象截取图片要显示的区域
			SColor(alpha,255,255,255),true);

		//物品名称的坐标.居中
		textPos.X=x+40;
		textPos.Y=y+40+10+10;

		TextLeft( "技能名称:" ,textPos,SColor(alpha,255,255,255),12,0);
		textPos.X+=12*5;
		TextLeft( "[时空防御]" ,textPos,SColor(alpha,255,255,0),12,0);





	}

}

bool MyGUIDlgCharSkill::LoadGUI()
{
	win = ui->addWindow(core::rect<s32>(0,0,240,300), false, L"");
	win->setRelativePosition(position2di(500,200));
	//win->setText(L"Skill Tool");


	win->setVisible(false);

	win->getCloseButton()->setVisible(false);
	win->getCloseButton()->setToolTipText(L"");

	//窗口的x 24编号 用来关闭
	IGUIButton* btn=ui->addButton(core::rect<s32>(0,0,15,14), win, 38, L"X" );
	btn->setRelativePosition(position2d<s32>(225,0));


	return true;
}