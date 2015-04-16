#include ".\myguidlgcommon.h"

MyGUIDlgNpcTalk::MyGUIDlgNpcTalk(   NPCProperty* npcprop ,Socket::CNetClient* client)
{
	this->dev=getIrrDevice();
	
	this->npcprop=npcprop;

	this->client=client;

	//鼠标在gui界面里
	mouseInGui=false;

	ui=dev->getGUIEnvironment();
	smgr=dev->getSceneManager();
	drv=dev->getVideoDriver();
}

MyGUIDlgNpcTalk::~MyGUIDlgNpcTalk(void)
{

}

void MyGUIDlgNpcTalk::ShowDlg(bool v)
{
   win->setVisible(v);
   //最前端
   ui->setFocus(win);
}

void MyGUIDlgNpcTalk::OnEvent (const irr::SEvent& e)
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
				//窗体右上角退出X按钮
				if(id==33){
					win->setVisible(false);
					mouseInGui=false;
					break;
				}

				//npc的按钮
				if(id==34){
					//本窗口隐藏
					win->setVisible(false);
					mouseInGui=false;

					//发送到服务器.请求此次npc的命令
				//	if(npcprop->cmd==CMD_C2S_UserRequest_NPC)
				//		npcprop->cmd=CMD_C2S_UserRequest_NPC;
					client->SendMsg((char*)npcprop, (6) ); //只需要发送前6位.包含npcID worldMap
					
					break;
				}


			}
		}
	}
}

void MyGUIDlgNpcTalk::update() //更新
{

}

void MyGUIDlgNpcTalk::drop()
{
  win->remove();
}


void MyGUIDlgNpcTalk::drawAll()
{
	rect<s32> win_rect=win->getAbsolutePosition();
	int x=win_rect.UpperLeftCorner.X;
	int y=win_rect.UpperLeftCorner.Y;
	if(win->isVisible()){
		SColor col = ui->getSkin()->getColor((EGUI_DEFAULT_COLOR)1);
		float	alpha=col.getAlpha();

		//显示npc说的话
		TextLeft(npcprop->strNpcTalk,vector2d<s32>(x+20,y+25), SColor(alpha,255,255,255),13,0,180); //每行180宽度 超过了自动换行
	}

}

bool MyGUIDlgNpcTalk::LoadGUI()
{
	win = ui->addWindow(core::rect<s32>(0,0,200,220), false, L"");
	win->setRelativePosition(position2di(350,100));
	win->setVisible(true);

	win->getCloseButton()->setVisible(false);
//	win->getCloseButton()->setToolTipText(L"");
//	win->getCloseButton()->setRelativePosition(vector2d<s32>(0,-2));


	//窗口的x 33编号 用来关闭
	btn=ui->addButton(core::rect<s32>(0,0,15,14), win, 33, L"X" );
	btn->setRelativePosition(position2d<s32>(185,0));


	rect<s32> win_rect=win->getAbsolutePosition();
	int x=win_rect.UpperLeftCorner.X;
	int y=win_rect.UpperLeftCorner.Y;


	//按钮

	//不等于-1 表示有按钮要激发事件
	if(npcprop->cmd!=-1){
	
		
		WCHAR * pwStr = ConvertLPCSTRToLPWSTR((char*)npcprop->strBtnText); 

		IGUIButton* buy_btn=ui->addButton(core::rect<s32>(0,0,15*4,16), win, 34 , pwStr  );
		//这里只需要用相对坐标了,因为他指定了父窗口
		position2d<s32> p1(50,25*3);
		buy_btn->setRelativePosition(p1);

		delete pwStr;
	}
	


	return true;
}