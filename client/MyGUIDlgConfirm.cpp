#include ".\myguidlgconfirm.h"

MyGUIDlgConfirm::MyGUIDlgConfirm()
{
	this->dev=getIrrDevice();
	//鼠标在gui界面里
	mouseInGui=false;

	ui=dev->getGUIEnvironment();
	smgr=dev->getSceneManager();
	drv=dev->getVideoDriver();
}

MyGUIDlgConfirm::~MyGUIDlgConfirm(void)
{
}

void MyGUIDlgConfirm::ShowDlg(bool v)
{
	win->setVisible(v);
	//最前端
	ui->setFocus(win);
}


bool MyGUIDlgConfirm::LoadGUI()
{
	win = ui->addWindow(core::rect<s32>(0,0,200,220), false, L"");
	win->setRelativePosition(position2di(350,100));
	win->setVisible(true);

	win->getCloseButton()->setVisible(false);
	//	win->getCloseButton()->setToolTipText(L"");
	//	win->getCloseButton()->setRelativePosition(vector2d<s32>(0,-2));


	//窗口的x 33编号 用来关闭
	IGUIButton* btn=ui->addButton(core::rect<s32>(0,0,15,14), win, 35, L"X" );
	btn->setRelativePosition(position2d<s32>(185,0));

	//按钮

	IGUIButton* btn_yes=ui->addButton(core::rect<s32>(0,0,15*4,16), win, 36 , L"确定"  );
	//这里只需要用相对坐标了,因为他指定了父窗口
	position2d<s32> p1(50,25*3);
	btn_yes->setRelativePosition(p1);

	IGUIButton* btn_no=ui->addButton(core::rect<s32>(0,0,15*4,16), win, 37 , L"取消"  );
	//这里只需要用相对坐标了,因为他指定了父窗口
	position2d<s32> p2(120,25*3);
	btn_no->setRelativePosition(p2);


	return true;
}

void MyGUIDlgConfirm::OnEvent (const irr::SEvent& e)
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
				//x按钮
				if(id==35){
					win->setVisible(false);
					mouseInGui=false;
					break;
				}

				//yes的按钮
				if(id==36){
					//本窗口隐藏
					win->setVisible(false);
					mouseInGui=false;

					break;
				}
				
				//no的按钮
				if(id==36){
					//本窗口隐藏
					win->setVisible(false);
					mouseInGui=false;

					break;
				}


			}
		}
	}
}