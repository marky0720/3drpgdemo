#include ".\myguidlgcharprop.h"




MyGUIDlgCharProp::MyGUIDlgCharProp(  MySceneManager* mgr)
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

MyGUIDlgCharProp::~MyGUIDlgCharProp(void)
{
}


bool MyGUIDlgCharProp::LoadGUI(void)
{
	win = ui->addWindow(core::rect<s32>(0,0,260,360), false, L"");
	win->setRelativePosition(position2di(100,300));
	win->setVisible(false);

	win->getCloseButton()->setVisible(false);
	win->getCloseButton()->setToolTipText(L"");

	//窗口的x 24编号 用来关闭
	IGUIButton* btn=ui->addButton(core::rect<s32>(0,0,15,14), win, 24, L"X" );
	btn->setRelativePosition(position2d<s32>(245,0));



	//三个增加属性点按钮
	addpoint_1=ui->addButton(core::rect<s32>(0,0,15*4,14), win, 25, L"+" );
	//这里只需要用相对坐标了,因为他指定了父窗口
	position2d<s32> p1(150,25*3);
	addpoint_1->setRelativePosition(p1);

	addpoint_2=ui->addButton(core::rect<s32>(0,0,15*4,14), win, 26, L"+" );
	position2d<s32> p2(150,25*5);
	addpoint_2->setRelativePosition(p2);

	addpoint_3=ui->addButton(core::rect<s32>(0,0,15*4,14), win, 27, L"+" );
	position2d<s32> p3(150,25*7);
	addpoint_3->setRelativePosition(p3);

	addpoint_1->setVisible(false);
	addpoint_2->setVisible(false);
	addpoint_3->setVisible(false);

	return true;
}


//
void MyGUIDlgCharProp::OnEvent (const SEvent& e) 
{

	// 键盘输出事件
	if (e.EventType == EET_KEY_INPUT_EVENT) {

		//打开/关闭窗口
		if(e.KeyInput.Key==KEY_KEY_C && e.KeyInput.PressedDown==false ) {
			win->setVisible(!win->isVisible());
			if(win->isVisible()==false){
				mouseInGui=false;
			}else{
				//最前端显示
				ui->setFocus(win);

				//表示打开了属性面版
				RoleProperty srp;
				memset((char*)&srp,'\0',sizeof(RoleProperty));
				srp.cmd=CMD_C2S_Request_Prop; //请求客户属性面版信息
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
				if(id==24){
					win->setVisible(!win->isVisible());
					if(win->isVisible()==false){
						mouseInGui=false;
					}
					break;
				}
				
				//加力量的按钮
				if(id==25){
				    u32	uestime=m_timer->getRealTime()-lasttime;
					//避免连按
					if(uestime>100){
						mgr->char_mgr->GetMainCharacter()->AddPoint(1);
						lasttime=m_timer->getRealTime();
					}

					
					break;
				}

				//加敏捷的按钮
				if(id==26){
					u32	uestime=m_timer->getRealTime()-lasttime;
					//避免连按
					if(uestime>100){
						mgr->char_mgr->GetMainCharacter()->AddPoint(2);
						lasttime=m_timer->getRealTime();
					}
					break;
				}
				
				//加体力的按钮
				if(id==27){
					u32	uestime=m_timer->getRealTime()-lasttime;
					//避免连按
					if(uestime>100){
						mgr->char_mgr->GetMainCharacter()->AddPoint(3);
						lasttime=m_timer->getRealTime();
					}
					break;
				}
				


			}

		}
	}


}




void MyGUIDlgCharProp::drawAll()
{
	win_rect=win->getAbsolutePosition();
	if(win->isVisible()){
		SColor col = ui->getSkin()->getColor((EGUI_DEFAULT_COLOR)1);
		alpha=col.getAlpha();



	int x=win_rect.UpperLeftCorner.X;
	int y=win_rect.UpperLeftCorner.Y;

	int level=mgr->char_mgr->GetMainCharacter()->getRoleproperty().Level;
	char temp[255]="";
	sprintf(temp,"等级: %d",level);
	TextCenter(temp,vector2d<s32>(x+50,y+25), SColor(alpha,255,255,255),15,700);
	
	sprintf(temp,"经验值: %d / %d",mgr->char_mgr->GetMainCharacter()->getRoleproperty().Exp, mgr->char_mgr->GetMainCharacter()->getRoleproperty().NextLevelExp);
	TextCenter(temp,vector2d<s32>(x+50+20,y+25*2), SColor(alpha,255,255,255),13,0);

	sprintf(temp,"力量: %d",mgr->char_mgr->GetMainCharacter()->getRoleproperty().Power_Point);
	TextCenter( temp,vector2d<s32>(x+50,y+25*3), SColor(alpha,220,191,113),13,700);

	sprintf(temp,"攻击力: %d",mgr->char_mgr->GetMainCharacter()->getRoleproperty().Attack+mgr->char_mgr->GetMainCharacter()->getPackage()->weaponparam2);
	TextCenter( temp,vector2d<s32>(x+50+20,y+25*4), SColor(alpha,255,255,255),13,0);

	sprintf(temp,"敏捷: %d",mgr->char_mgr->GetMainCharacter()->getRoleproperty().Minjie_Point);
	TextCenter( temp,vector2d<s32>(x+50,y+25*5), SColor(alpha,220,191,113),13,700);

	sprintf(temp,"防御力: %d",mgr->char_mgr->GetMainCharacter()->getRoleproperty().Defense );
	TextCenter( temp,vector2d<s32>(x+50+20,y+25*6), SColor(alpha,255,255,255),13,0);

	sprintf(temp,"体力: %d",mgr->char_mgr->GetMainCharacter()->getRoleproperty().Tili_Point);
	TextCenter( temp,vector2d<s32>(x+50,y+25*7), SColor(alpha,220,191,113),13,700);

	sprintf(temp,"生命值(HP) : %d / %d",mgr->char_mgr->GetMainCharacter()->getRoleproperty().HP,mgr->char_mgr->GetMainCharacter()->getRoleproperty().MAX_HP );
	TextLeft( temp,vector2d<s32>(x+50+20-37,y+25*8), SColor(alpha,255,255,255),13,0);



		//如果有可用属性点,就显示增加属性点按钮
	  if(mgr->char_mgr->GetMainCharacter()->getRoleproperty().usePoint>0)
	  {
			addpoint_1->setVisible(true);
			addpoint_2->setVisible(true);
			addpoint_3->setVisible(true);

			sprintf(temp,"可用属性点: %d",mgr->char_mgr->GetMainCharacter()->getRoleproperty().usePoint);
			TextCenter( temp,vector2d<s32>(x+150,y+25), SColor(alpha,255,0,255),15,600);
	  }else{
		  addpoint_1->setVisible(false);
		  addpoint_2->setVisible(false);
		  addpoint_3->setVisible(false);
	  }
	
	}
}




void MyGUIDlgCharProp::update()
{
	if(win->isVisible()){
		stringw t="";
		t+=mgr->char_mgr->GetMainCharacter()->getRoleproperty().usePoint;
		

	}

}
