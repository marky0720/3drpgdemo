#include ".\MyGUIDlgSysMenu.h"



MyGUIDlgSysMenu::MyGUIDlgSysMenu()
{
	this->dev=getIrrDevice();

	//鼠标在gui界面里
	mouseInGui=false;

	ui=dev->getGUIEnvironment();
	smgr=dev->getSceneManager();
	drv=dev->getVideoDriver();

	config_dlg=0;
}

MyGUIDlgSysMenu::~MyGUIDlgSysMenu(void)
{
}

void MyGUIDlgSysMenu::update()
{
  if(config_dlg) config_dlg->update();
}

void MyGUIDlgSysMenu::drawAll()
{
  if(config_dlg) config_dlg->drawAll();
}

bool MyGUIDlgSysMenu::LoadGUI()
{
	win = ui->addWindow(core::rect<s32>(0,0,200,220), false, L"");
	win->setRelativePosition(position2di(300,180));
	win->setVisible(false);

	win->getCloseButton()->setVisible(false);
	//win->getCloseButton()->setToolTipText(L"");
	//win->getCloseButton()->setPosition(vector2d<s32>(0,-2)));

	//不可拖动
	win->setDraggable(false);

	//三个增加属性点按钮
	IGUIButton* exit_btn=ui->addButton(core::rect<s32>(0,0,15*6,15), win, 30, L"界面配置" );
	//这里只需要用相对坐标了,因为他指定了父窗口
	position2d<s32> p1(50,25*2);
	exit_btn->setRelativePosition(p1);

	exit_btn=ui->addButton(core::rect<s32>(0,0,15*6,15), win, 31, L"退出" );
	//这里只需要用相对坐标了,因为他指定了父窗口
	position2d<s32> p2(50,25*3);
	exit_btn->setRelativePosition(p2);

	
	exit_btn=ui->addButton(core::rect<s32>(0,0,15*6,15), win, 32, L"继续游戏" );
	//这里只需要用相对坐标了,因为他指定了父窗口
	position2d<s32> p3(50,25*4);
	exit_btn->setRelativePosition(p3);


	config_dlg=new MyGUIDlgConfig();
	config_dlg->LoadGUI();


	return true;
}


void MyGUIDlgSysMenu::OnEvent (const SEvent& e) 
{

	if(config_dlg) config_dlg->OnEvent(e);
	if(config_dlg&& config_dlg->mouseInGui){
		mouseInGui=true;
		return ;
	}

	// 键盘输出事件
	if (e.EventType == EET_KEY_INPUT_EVENT) {

		//ESC事件
		if(e.KeyInput.Key==KEY_ESCAPE && e.KeyInput.PressedDown==false) {
			if(win->isVisible()){
				mouseInGui=false;
				win->setVisible(false);
			}else
			{
				mouseInGui=true;
				win->setVisible(true);
				//最前端
				ui->setFocus(win);
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
				//退出
				if(id==31){
					dev->closeDevice();

					break;
				}

				//gui配置
				if(id==30){
					//显示gui配置窗口
					config_dlg->ShowDlg(true);

					break;
				}

				//继续游戏
				if(id==32){
					win->setVisible(false);
					mouseInGui=false;

					break;
				}
			}
		}
	}


}
