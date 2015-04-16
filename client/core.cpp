#include "stdafx.h" 
#include "core.h" 



GameMain* lpgamemain=0;

int recvcount=0; //记录接收数据包次数
/*
消息回调函数
*/
void WINAPI  RecvData(const char * pData,const unsigned int DataLength)
{
	//有数据
	if(pData!=NULL)
	{	
		recvcount++;
		
		char test[255];
		sprintf(test,"count:%d  cmd:%d  size:%d",recvcount,(unsigned char)pData[0],DataLength);
		WriteToLog(test);

		printf("-count:%d-------RecvData data:%d DataLength:%d\n",recvcount,(unsigned char)pData[0],DataLength);

		//实际的数据
		unsigned char cmd=pData[0];

		
		//用户管理器接收消息
		if( cmd <=CMD_C2S_Request_SellPackageItem && cmd>=CMD_C2S_UserRequest_LOGIN)
		{

			//主角色登陆
			if(cmd == CMD_S2C_User_LOGIN)
			{	
				printf("recvdata CMD_S2C_User_LOGIN CMD_USERLOGIN_PROC\n");
				lpgamemain->myScene_Mgr->char_mgr->CMD_USERLOGIN_PROC(pData,DataLength);
				//标志登陆成功
				SetEvent(lpgamemain->m_h_LoginEvent);
			}
			else{

				printf("添加到char_mgr\n");
				lpgamemain->myScene_Mgr->char_mgr->insertCharData(pData,DataLength);
			}
		}else if(cmd>=MonsterCMD_LOGIN && cmd<=MonsterCMD_MonsterAll)
		{
			//怪物管理器接收消息

			//怪物的消息
			printf("添加到monster_mgr\n");
			 lpgamemain->myScene_Mgr->monster_mgr->insertMonsterData(pData,DataLength);
			
		}else if(cmd>=SceneCMD_ITEMDROP && cmd<=SceneCMD_ITEMDROPRemove)
		{
			//场景物品管理器接收消息
			printf("添加到item_mgr\n");
			
			lpgamemain->myScene_Mgr->item_mgr->insertItemData(pData,DataLength);
		}else if(cmd>=CMD_C2S_UserRequest_NPC && cmd<=CMD_C2S_UserBuyShop )
		{
			//gui管理器接收消息
			printf("添加到GUI_Mgr\n");
			
			lpgamemain->myScene_Mgr->myGUI_Mgr->insertGUIData(pData,DataLength);
		}

	}else{
		MessageBox(0,"连接断开!","",0);
	}


}




GameMain::GameMain (SLogin* lo) {


	slogin=lo;


	DeleteFile("client_log.txt");

	lpgamemain=this;

	device=getIrrDevice();
	ui=getIrrUI();
	smgr=getIrrSmgr();
	drv=getIrrDriver();

		myScene_Mgr=0;

     

		getIrrSmgr()->clear();//清理场景
		getIrrUI()->clear();  //清理ui
		
		getIrrDevice()->setEventReceiver(this);//设置事件接收器

     //   dev->getCursorControl ()->setVisible(false); //鼠标显示?

        // Without setting the following flags all works good as well 
        // It's just to be 100% sure 
        drv->setTextureCreationFlag (ETCF_ALWAYS_32_BIT, true); 
        drv->setTextureCreationFlag (ETCF_ALWAYS_16_BIT, false); 
        drv->setTextureCreationFlag (ETCF_CREATE_MIP_MAPS, false); 
        drv->setTextureCreationFlag (ETCF_OPTIMIZED_FOR_QUALITY, true); 




//--初始化字体----------------------------------------------------------------------------------------------------

	char tmp[512];
	GetWindowsDirectory(tmp,511);
	strcat(tmp,"\\fonts\\SIMSUN.TTC");  //SIMSUN.TTC宋体,新宋体         SIMKAI.TTF楷体     \STXIHEI.TTF   Fixedsys

	gui::IGUIFont *font;
	gui::IGUISkin* skin;
  //SIMSUN.TTC宋体,新宋体         SIMKAI.TTF楷体     \STXIHEI.TTF   Fixedsys
	font = ui->getFont(tmp,14, true,false);
	skin = ui->getSkin();
	skin->setFont(font);

//-加载资源前先绘制图或字.避免黑屏等待----------------------------------------------------------------------------------------------------


	ITexture* h=drv->getTexture("textures/loading.jpg");

   drv->beginScene (true, true, SColor (150, 0, 0, 0)); 

   ui->drawAll();

   drv->draw2DImage(h, core::position2d<s32>(0,0)	//指定绘制到屏幕的位置			
);		//指定要显示的图象在图片中截取的矩形区域

	//TextLeft( "正在加载中!",vector2d<s32>(400,400),SColor(1,0,255,0),25,0);
   drv->endScene (); 


//------------------------------------------------------------------------------------------------------
//总场景管理器
myScene_Mgr=new MySceneManager(&client);


//初始化camera
myScene_Mgr->InitCamera();


//初始化角色管理器
myScene_Mgr->InitCharacterMgr();
//初始化怪物管理器
myScene_Mgr->InitMonsterMgr();
//初始化npc管理器 
myScene_Mgr->InitNpcMgr();
//初始化场景物品管理器
myScene_Mgr->InitItemMgr();
//初始化gui
myScene_Mgr->InitGUIMgr();

//初始化网络
if(InitNetWork()==false){
	MessageBox(0,"初始化网络/连接服务器失败!","",0);
	device->closeDevice();
	exit(1);
}


//Sleep(1000);

//登陆
Login();


// set ambient light 环境光,灰色
smgr->setAmbientLight(video::SColor(0,60,60,60)); // a r g b


device->run();
myScene_Mgr->update();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  

uesBlur=false;

HideOther=false;

 
 



  ////create light
 ISceneNode* node = smgr->addLightSceneNode(0, core::vector3df(0,50,0),
	 video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 400.0f);
 	scene::ISceneNodeAnimator* anim = 0;
 	anim = smgr->createFlyCircleAnimator (core::vector3df(0,150,0),250.0f);
 	node->addAnimator(anim);
 	anim->drop();


////  attach billboard to light
// ISceneNode* node;
node = smgr->addBillboardSceneNode( node , core::dimension2d<f32>(50, 50));
node->setPosition(vector3df(0,50,0));
 node->setMaterialFlag(video::EMF_LIGHTING, false);
 node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
 node->setMaterialTexture(0, drv->getTexture("textures//fire.bmp"));
 node->setMaterialFlag(EMF_ZWRITE_ENABLE ,false); //关闭背面剔除




} 

GameMain::~GameMain () { 

} 

void GameMain::Login() // 登入模块 登陆失败则退出程序
{

	//发送登陆请求
	SendLoginRequest();
	//等待请求
	bool ret=WaitForServerLoginResponse();
	if(ret==false){
		MessageBox(0,"服务器请求登陆失败!","",0);
		device->closeDevice();
		exit(1);
	}

	//if( 成立 )
	//		   　　进入游戏;
	//	   　else
	//			　　提示用户登入错误并重新接受用户登入;
}

void GameMain::SendLoginRequest()
{
	//发送登陆请求,得到角色信息---------------------------------------------------
	RoleProperty temp;
	memset((char*)&temp,'\0',sizeof(RoleProperty));
	temp.cmd=CMD_C2S_UserRequest_LOGIN;
	strcpy(temp.roleName,slogin->rolename);
	temp.charSex= slogin->charSex;

	bool ret=client.SendMsg((char*)&temp,sizeof(RoleProperty));
	if(ret==false){
		printf("--发送登陆请求1失败\n");
		
	}else	printf("--发送登陆请求1成功\n"); 

	//创建一个事件对象,等待服务器响应
	m_h_LoginEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);//参数2是否手动重置,参数3初始化时有/无信号
	//创建一个自动重置,初始化时无信号的事件对象

}

bool GameMain::WaitForServerLoginResponse()
{
	//5秒等待.超过5秒就返回
	DWORD ret=WaitForSingleObject(m_h_LoginEvent,50000);
	if(ret==WAIT_TIMEOUT){
		return false;
	}else if(ret==WAIT_OBJECT_0){
		return true; //对象有信号
	}
}


bool GameMain::InitNetWork()
{


	char FilePath[15]=".\\server.ini";      //注意路径问题
	char inBuf[255]="127.0.0.1";     //用于保存读取的字符串

	IniGetString(FilePath,"ip","ip",inBuf);

	int port=12;     //端口
	port=IniGetInt(FilePath,"ip","port");
	

	//初始化网络引擎
	if(client.ConnectServer(inBuf,port,RecvData)){
		printf("InitNetWork连接ok\n");

	}else{
		printf("InitNetWork连接失败\n");
		return false;
	}



	return true;
}

bool GameMain::OnEvent (const SEvent& e) { 

	if(myScene_Mgr) myScene_Mgr->OnEvent(e);


			 // 键盘输出事件
			if (e.EventType == EET_KEY_INPUT_EVENT) {
					//if (e.KeyInput.Key == KEY_ESCAPE) { 
					//	device->closeDevice();   //	device->closeDevice();
					//} 
					if(e.KeyInput.Key==KEY_F8 && e.KeyInput.PressedDown==false) {
					//	debug_info=!debug_info;
						uesBlur=!uesBlur;

					//	myScene_Mgr->char_mgr->setAllRoleClothVisible(true);
					//	myScene_Mgr->myscene_loader->setVisiable(true);
					//	myScene_Mgr->monster_mgr->setAllMonsterVisiable(true);
					//	myScene_Mgr->item_mgr->setAllItemVisiable(true);
					//	myScene_Mgr->char_mgr->setAllRoleWeaponVisible(true);
					//	myScene_Mgr->npc_mgr->setVisiable(true);

					}


				if (e.KeyInput.Key == KEY_DELETE && e.KeyInput.PressedDown==false) { 
					HideOther=!HideOther;

					if(HideOther==false){
						myScene_Mgr->char_mgr->setAllRoleClothVisible(true);
						myScene_Mgr->char_mgr->setAllRoleWeaponVisible(true);
					}
				} 


					
				
					return false; 
			}

        return false; 
} 



void GameMain::Run () { 

		static vector3df target_pos;

        int last_fps = -1; 



        while (device->run() ) { 
			
	
			//限fps
			if(last_fps>80){
				Sleep(1);
			 	
			}
 
						myScene_Mgr->update();
						
                        drv->beginScene (true, true, SColor (150, 0, 0, 0)); 
					
 
						if(uesBlur){
							effect->update();
						}
						else{
							
							smgr->drawAll();
						}
			 
				
				ui->drawAll();
				
			 
					myScene_Mgr->UIRender();	


					


                    drv->endScene (); 

	
                        int fps = drv->getFPS (); 
						
                        if (last_fps != fps) { 
                                core::stringw str = L"孤独之旅   FPS: "; 
                                str += fps; 
                                device->setWindowCaption (str.c_str ()); 
                                last_fps = fps; 
                        } 
            //    } 
        } 

		printf("网络卸载 清理场景 清理ui 清除内存中的所有纹理图片\n");

		myScene_Mgr->char_mgr->Quit();
		myScene_Mgr->monster_mgr->Quit();
 
		smgr->clear(); 
		ui->clear();   

		drv->removeAllHardwareBuffers();  
		drv->removeAllTextures(); // 
 
} 
