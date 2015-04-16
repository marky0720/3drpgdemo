#include "StdAfx.h"
#include ".\myguimanager.h"



MyGUIManager::MyGUIManager (MySceneManager* m)
{
	dev=getIrrDevice();
	mgr=m;
	ui=dev->getGUIEnvironment();
	drv=dev->getVideoDriver();
	smgr=dev->getSceneManager();

	 for(int x=0; x<irr::KEY_KEY_CODES_COUNT; x++) keys[x] = false; 
	 mouseDownL=false; 
	 mouseDownM=false; 
	 mouseDownR=false; 
	cursorOld=dev->getCursorControl()->getPosition();
	cursor=cursorOld;

	 last_time = dev->getTimer ()->getRealTime (); 
	 guieditInput=false;

	 editbox=0;

	 m_pGUIDlg_charPackage=0;
	 m_pGUIDlg_charProp=0;
	 m_pGUIDlg_sysMenu=0;
	 m_pGUIDlg_npcShop=0;
	 m_pGUIDlg_skill=0;

}

MyGUIManager::~MyGUIManager(void)
{
}

//插入gui消息,来自服务器
void MyGUIManager::insertGUIData(const char* pData,const unsigned int DataLength)
{
	printf(" MyGUIManager::insertGUIData\n");
	
	unsigned char cmd=pData[0];
	if(cmd==CMD_S2C_NPC_Shop){
		ShopPackage sp;
		memcpy((char*)&sp,pData,sizeof(ShopPackage));

		//npc商店数据设置
		m_pGUIDlg_npcShop->setShopPackage(&sp);
		
		//npc商店窗口开启
		m_pGUIDlg_npcShop->ShowDlg(true);
	
		//角色包裹窗口开启
		m_pGUIDlg_charPackage->ShowDlg(true);
	}




}

//用户喊话的方法,把msg字符串显示到listbox上
void MyGUIManager::PostMsg(const wchar_t* msg)
{
	msglist->addItem(msg);

	msglist->setSelected(msglist->getItemCount());
}

bool MyGUIManager::LoadGUI()
{



blood_per=1;
	tex_HP= drv->getTexture("ui/ui2.tbj");
	tex_HPBG= drv->getTexture("ui/ui.tbj");
drv->makeColorKeyTexture(tex_HPBG, core::position2d<s32>(1,1)); //指定关键色的argb
drv->makeColorKeyTexture(tex_HP, core::position2d<s32>(1,1)); //指定关键色的argb
	//msglist = ui->addListBox(rect<s32>(0, 540, 400, 645),0,1);
	//msglist->addItem(L"welcome to game world!");

bottom_bar=drv->getTexture("ui/bar.tbj");

io::IFileSystem* filesys = dev->getFileSystem();

// Load GUI
gui::SImageGUISkinConfig guicfg = LoadGUISkinFromFile(filesys, drv, "ui/guiskin.cfg");
gui::CImageGUISkin* skin = new gui::CImageGUISkin(drv, ui->getSkin());
skin->loadConfig(guicfg);


char tmp[512];
GetWindowsDirectory(tmp,511);
strcat(tmp,"\\fonts\\SIMSUN.TTC");  //SIMSUN.TTC宋体,新宋体         SIMKAI.TTF楷体     \STXIHEI.TTF   Fixedsys

gui::IGUIFont *font;
//	gui::IGUISkin* skin;
//SIMSUN.TTC宋体,新宋体         SIMKAI.TTF楷体     \STXIHEI.TTF   Fixedsys
font = ui->getFont(tmp,14, true,false);




ui->setSkin(skin);
skin->drop();



stringw t=L"当前在线人数:";
t+=mgr->char_mgr->getOnlineCharacterNum();
t+=L" 怪物数:";
t+=mgr->monster_mgr->getAllMonsterNum();


msglist=ui->addListBox(rect<s32>(0,450,300,580),0,-1,true);


msglist->addItem(L"-----------------------------------------------------------");
msglist->addItem(L"欢迎来到game world!");
//msglist->addItem(t.c_str());
msglist->addItem(L"-----------------------------------------------------------");
msglist->setSelected(1);
//msglist->setItemOverrideColor(1,SColor(255,0,255,5));


	editbox= ui->addEditBox(L"", rect<s32>(0, 580, 400, 600),true,0,2);
	editbox->setVisible(false);

	 //SIMSUN.TTC宋体,新宋体         SIMKAI.TTF楷体        Fixedsys
//	irr::gui::IGUIFont*	font = ui->getDXFont("宋体",true,16,0,900);


//	msg=smgr->addTextSceneNode (font,L"123",SColor(255,255,255,0),mgr->char_mgr->GetMainCharacter()->getNode(),vector3df(0,90,0));
//	msg->setVisible(true);

	//初始化包裹面版
	m_pGUIDlg_charPackage=new MyGUIDlgCharPackage(mgr);
	m_pGUIDlg_charPackage->LoadGUI();

	//初始化角色属性面版
	m_pGUIDlg_charProp=new MyGUIDlgCharProp(mgr);
	m_pGUIDlg_charProp->LoadGUI();

	//初始化角色技能面版
	m_pGUIDlg_skill=new MyGUIDlgCharSkill(mgr);
	m_pGUIDlg_skill->LoadGUI();

	//初始化系统菜单
	m_pGUIDlg_sysMenu=new MyGUIDlgSysMenu();
	m_pGUIDlg_sysMenu->LoadGUI();

	//初始化商店窗口
	m_pGUIDlg_npcShop=new MyGUIDlgNpcShop(mgr);
	m_pGUIDlg_npcShop->LoadGUI();



	return true;
}
//editbox正在输入中
bool MyGUIManager::GetEditIsInput()
{
	return	guieditInput;
}
void MyGUIManager::OnEvent (const SEvent& e) 
{
	if(m_pGUIDlg_sysMenu) m_pGUIDlg_sysMenu->OnEvent(e);
	//系统菜单打开,其他的就不接收事件了
	if(m_pGUIDlg_sysMenu && m_pGUIDlg_sysMenu->mouseInGui){
		guieditInput=true;
		return;
	}

	if(m_pGUIDlg_charPackage) m_pGUIDlg_charPackage->OnEvent(e);
	if(m_pGUIDlg_charProp) m_pGUIDlg_charProp->OnEvent(e);

	if(m_pGUIDlg_charPackage && m_pGUIDlg_charPackage->mouseInGui){
     guieditInput=true;
	 return;
	}else if(m_pGUIDlg_charProp && m_pGUIDlg_charProp->mouseInGui){
		guieditInput=true;
		return;
	}

	if(m_pGUIDlg_npcShop) m_pGUIDlg_npcShop->OnEvent(e);
	if(m_pGUIDlg_npcShop && m_pGUIDlg_npcShop->mouseInGui){
		guieditInput=true;
		return;
	}

	if(m_pGUIDlg_skill) m_pGUIDlg_skill->OnEvent(e);
	if(m_pGUIDlg_skill && m_pGUIDlg_skill->mouseInGui){
		guieditInput=true;
		return;
	}
	


	// 键盘输出事件
	if (e.EventType == EET_KEY_INPUT_EVENT) {
		
		//回车事件
		if(e.KeyInput.Key==KEY_RETURN && e.KeyInput.PressedDown==false) {

			//
			if(editbox->isVisible())
			{ 
				//输入结束
				editbox->setVisible(false);
				guieditInput=false;
			}
			else{
				//开始输入
				guieditInput=true;
				editbox->setEnabled(true);
				editbox->setVisible(true);
				editbox->setText(L"");
			}
			
		}
		if(editbox->isVisible())
		{
			editbox->OnEvent(e);
		}



	//	if(e.KeyInput.Key==KEY_RETURN && e.KeyInput.PressedDown==false) {
			//	debug_info=!debug_info;
	//	}

		 keys[e.KeyInput.Key] = e.KeyInput.PressedDown; 
	
	}


	if(e.EventType ==irr::EET_MOUSE_INPUT_EVENT)//鼠标事件
	{
		switch (e.MouseInput.Event)//判断鼠标事件
		{
		case EMIE_LMOUSE_PRESSED_DOWN :

			cursor.X = e.MouseInput.X; 
			cursor.Y = e.MouseInput.Y; 

			if(msglist->isPointInside(cursor) || editbox->isPointInside(cursor)){
					guieditInput=true;
			}else{
					guieditInput=false;
			}
			
		break;
		case EMIE_LMOUSE_LEFT_UP:
			mouseDownL=false;
			//device->setWindowCaption(L"左键mouse弹起");
			break;
		case EMIE_RMOUSE_PRESSED_DOWN :
			//device->setWindowCaption(L"右键mouse按下");
				mouseDownR=true;
			break;
		case EMIE_RMOUSE_LEFT_UP :
				mouseDownR=false;
			//device->setWindowCaption(L"右键mouse弹起");
			break;
		case EMIE_MMOUSE_PRESSED_DOWN  :
			//device->setWindowCaption(L"中键mouse按下");
			mouseDownM=true;
			break;
		case EMIE_MMOUSE_LEFT_UP   :
			//device->setWindowCaption(L"中键mouse弹起");
			mouseDownM=false;
			break;
			// mouse move check 
		case EMIE_MOUSE_MOVED   :
			{ 
					cursor.X = e.MouseInput.X; 
					cursor.Y = e.MouseInput.Y; 
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
		case EGET_EDITBOX_ENTER:
			if (id == 2) //处理editbox的回车事件 
			{
				if(editbox->getText()==L""){
					return;
				}
				
				WCHAR * pwStr = ConvertLPCSTRToLPWSTR(mgr->char_mgr->GetMainCharacter()->getRoleproperty().roleName); 
				stringw w="";
				w+=pwStr;
				w+=L":";
				w+=editbox->getText();

				msglist->addItem(w.c_str());
	
				msglist->setSelected(msglist->getItemCount());


				//msg->setVisible(true);
				stringw s=L"";
				s.append(editbox->getText());
				//本地客户端显示msg
				if(mgr->char_mgr->GetMainCharacter())  mgr->char_mgr->GetMainCharacter()->PostMsg(s.c_str());
				//编辑框字符归零
				editbox->setText(L"");

				//构造发送msg数据
				ServerRoleMSG msg;
				memset((char*)&msg,'/0',sizeof(ServerRoleMSG));
				msg.cmd=CMD_USERMSG;
				msg.sock=123;
				//字符转换
				char* te=WCharToMByte(s.c_str());
				strcpy(msg.msg,te);

				//发送给服务器
				mgr->client->SendMsg((char*)&msg,sizeof(ServerRoleMSG));

				break;
			}

		
		}
	}


}


void MyGUIManager::drawAll()
{
    drv->draw2DImage(tex_HPBG, core::position2d<s32>(0,0),
				core::rect<s32>(0,0,164,59), 0, 
				video::SColor(255, 255,255,255), true);

	drv->draw2DImage(tex_HP, core::position2d<s32>(0+30,0+2),
				core::rect<s32>(0,0,(int)134*blood_per,12), 0, 
				video::SColor(255, 255,255,255), true);

	drv->draw2DImage(bottom_bar, core::position2d<s32>(420,570));

	if(m_pGUIDlg_charPackage) m_pGUIDlg_charPackage->drawAll();

	if(m_pGUIDlg_charProp) m_pGUIDlg_charProp->drawAll();

	if(m_pGUIDlg_sysMenu) m_pGUIDlg_sysMenu->drawAll();

	if(m_pGUIDlg_npcShop) m_pGUIDlg_npcShop->drawAll();

	if(m_pGUIDlg_skill) m_pGUIDlg_skill->drawAll();

}

void MyGUIManager::update()
{

	if(mgr->char_mgr->GetMainCharacter()) blood_per=mgr->char_mgr->GetMainCharacter()->getHPper();

	//角色包裹面版
	if(m_pGUIDlg_charPackage) m_pGUIDlg_charPackage->update();

	//角色属性面版
	if(m_pGUIDlg_charProp) m_pGUIDlg_charProp->update();

	//角色技能面版
	if(m_pGUIDlg_skill) m_pGUIDlg_skill->update();

	//系统菜单
	if(m_pGUIDlg_sysMenu) m_pGUIDlg_sysMenu->update();

	//商店窗口更新
	if(m_pGUIDlg_npcShop) m_pGUIDlg_npcShop->update();

}
