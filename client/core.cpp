#include "stdafx.h" 
#include "core.h" 



GameMain* lpgamemain=0;

int recvcount=0; //��¼�������ݰ�����
/*
��Ϣ�ص�����
*/
void WINAPI  RecvData(  char * pData,unsigned long  DataLength)
{
	//������
	if(pData!=NULL)
	{	
		recvcount++;
		
		char test[255];
		sprintf(test,"count:%d  cmd:%d  size:%d",recvcount,(unsigned char)pData[0],DataLength);
		WriteToLog(test);

		printf("-count:%d-------RecvData data:%d DataLength:%d\n",recvcount,(unsigned char)pData[0],DataLength);

		//ʵ�ʵ�����
		unsigned char cmd=pData[0];

		
		//�û�������������Ϣ
		if( cmd <=CMD_C2S_Request_SellPackageItem && cmd>=CMD_C2S_UserRequest_LOGIN)
		{

			//����ɫ��½
			if(cmd == CMD_S2C_User_LOGIN)
			{	
				printf("recvdata CMD_S2C_User_LOGIN CMD_USERLOGIN_PROC\n");
				lpgamemain->myScene_Mgr->char_mgr->CMD_USERLOGIN_PROC(pData,DataLength);
				//��־��½�ɹ�
				SetEvent(lpgamemain->m_h_LoginEvent);
			}
			else{

				printf("��ӵ�char_mgr\n");
				lpgamemain->myScene_Mgr->char_mgr->insertCharData(pData,DataLength);
			}
		}else if(cmd>=MonsterCMD_LOGIN && cmd<=MonsterCMD_MonsterAll)
		{
			//���������������Ϣ

			//�������Ϣ
			printf("��ӵ�monster_mgr\n");
			 lpgamemain->myScene_Mgr->monster_mgr->insertMonsterData(pData,DataLength);
			
		}else if(cmd>=SceneCMD_ITEMDROP && cmd<=SceneCMD_ITEMDROPRemove)
		{
			//������Ʒ������������Ϣ
			printf("��ӵ�item_mgr\n");
			
			lpgamemain->myScene_Mgr->item_mgr->insertItemData(pData,DataLength);
		}else if(cmd>=CMD_C2S_UserRequest_NPC && cmd<=CMD_C2S_UserBuyShop )
		{
			//gui������������Ϣ
			printf("��ӵ�GUI_Mgr\n");
			
			lpgamemain->myScene_Mgr->myGUI_Mgr->insertGUIData(pData,DataLength);
		}

	}else{
		MessageBox(0,"���ӶϿ�!","",0);
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

     

		getIrrSmgr()->clear();//������
		getIrrUI()->clear();  //����ui
		
		getIrrDevice()->setEventReceiver(this);//�����¼�������

     //   dev->getCursorControl ()->setVisible(false); //�����ʾ?

        // Without setting the following flags all works good as well 
        // It's just to be 100% sure 
        drv->setTextureCreationFlag (ETCF_ALWAYS_32_BIT, true); 
        drv->setTextureCreationFlag (ETCF_ALWAYS_16_BIT, false); 
        drv->setTextureCreationFlag (ETCF_CREATE_MIP_MAPS, false); 
        drv->setTextureCreationFlag (ETCF_OPTIMIZED_FOR_QUALITY, true); 




//--��ʼ������----------------------------------------------------------------------------------------------------

	char tmp[512];
	GetWindowsDirectory(tmp,511);
	strcat(tmp,"\\fonts\\SIMSUN.TTC");  //SIMSUN.TTC����,������         SIMKAI.TTF����     \STXIHEI.TTF   Fixedsys

	gui::IGUIFont *font;
	gui::IGUISkin* skin;
  //SIMSUN.TTC����,������         SIMKAI.TTF����     \STXIHEI.TTF   Fixedsys
	font = ui->getFont(tmp,14, true,false);
	skin = ui->getSkin();
	skin->setFont(font);

//-������Դǰ�Ȼ���ͼ����.��������ȴ�----------------------------------------------------------------------------------------------------


	ITexture* h=drv->getTexture("textures/loading.jpg");

   drv->beginScene (true, true, SColor (150, 0, 0, 0)); 

   ui->drawAll();

   drv->draw2DImage(h, core::position2d<s32>(0,0)	//ָ�����Ƶ���Ļ��λ��			
);		//ָ��Ҫ��ʾ��ͼ����ͼƬ�н�ȡ�ľ�������

	//TextLeft( "���ڼ�����!",vector2d<s32>(400,400),SColor(1,0,255,0),25,0);
   drv->endScene (); 


//------------------------------------------------------------------------------------------------------
//�ܳ���������
myScene_Mgr=new MySceneManager(&client);


//��ʼ��camera
myScene_Mgr->InitCamera();


//��ʼ����ɫ������
myScene_Mgr->InitCharacterMgr();
//��ʼ�����������
myScene_Mgr->InitMonsterMgr();
//��ʼ��npc������ 
myScene_Mgr->InitNpcMgr();
//��ʼ��������Ʒ������
myScene_Mgr->InitItemMgr();
//��ʼ��gui
myScene_Mgr->InitGUIMgr();

//��ʼ������
if(InitNetWork()==false){
	MessageBox(0,"��ʼ������/���ӷ�����ʧ��!","",0);
	device->closeDevice();
	exit(1);
}


//Sleep(1000);

//��½
Login();


// set ambient light ������,��ɫ
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
 node->setMaterialFlag(EMF_ZWRITE_ENABLE ,false); //�رձ����޳�




} 

GameMain::~GameMain () { 

} 

void GameMain::Login() // ����ģ�� ��½ʧ�����˳�����
{

	//���͵�½����
	SendLoginRequest();
	//�ȴ�����
	bool ret=WaitForServerLoginResponse();
	if(ret==false){
		MessageBox(0,"�����������½ʧ��!","",0);
		device->closeDevice();
		exit(1);
	}

	//if( ���� )
	//		   ����������Ϸ;
	//	   ��else
	//			������ʾ�û�����������½����û�����;
}

void GameMain::SendLoginRequest()
{
	//���͵�½����,�õ���ɫ��Ϣ---------------------------------------------------
	RoleProperty temp;
	memset((char*)&temp,'\0',sizeof(RoleProperty));
	temp.cmd=CMD_C2S_UserRequest_LOGIN;
	strcpy(temp.roleName,slogin->rolename);
	temp.charSex= slogin->charSex;

	bool ret=client.SendMsg((char*)&temp,sizeof(RoleProperty));
	if(ret==false){
		printf("--���͵�½����1ʧ��\n");
		
	}else	printf("--���͵�½����1�ɹ�\n"); 

	//����һ���¼�����,�ȴ���������Ӧ
	m_h_LoginEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);//����2�Ƿ��ֶ�����,����3��ʼ��ʱ��/���ź�
	//����һ���Զ�����,��ʼ��ʱ���źŵ��¼�����

}

bool GameMain::WaitForServerLoginResponse()
{
	//5��ȴ�.����5��ͷ���
	DWORD ret=WaitForSingleObject(m_h_LoginEvent,50000);
	if(ret==WAIT_TIMEOUT){
		return false;
	}else if(ret==WAIT_OBJECT_0){
		return true; //�������ź�
	}
}


bool GameMain::InitNetWork()
{


	char FilePath[15]=".\\server.ini";      //ע��·������
	char inBuf[255]="127.0.0.1";     //���ڱ����ȡ���ַ���

	IniGetString(FilePath,"ip","ip",inBuf);

	int port=12;     //�˿�
	port=IniGetInt(FilePath,"ip","port");
	

	//��ʼ����������
	if(client.Init(RecvData,inBuf,port)){
		printf("InitNetWork����ok\n");

	}else{
		printf("InitNetWork����ʧ��\n");
		return false;
	}



	return true;
}

bool GameMain::OnEvent (const SEvent& e) { 

	if(myScene_Mgr) myScene_Mgr->OnEvent(e);


			 // ��������¼�
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
			
	
			//��fps
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
                                core::stringw str = L"�¶�֮��   FPS: "; 
                                str += fps; 
                                device->setWindowCaption (str.c_str ()); 
                                last_fps = fps; 
                        } 
            //    } 
        } 

		printf("����ж�� ������ ����ui ����ڴ��е���������ͼƬ\n");

		myScene_Mgr->char_mgr->Quit();
		myScene_Mgr->monster_mgr->Quit();
 
		smgr->clear(); 
		ui->clear();   

		drv->removeAllHardwareBuffers();  
		drv->removeAllTextures(); // 
 
} 
