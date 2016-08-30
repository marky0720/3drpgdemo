// muloginscene.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "../stdafx.h"

#include "../realisticwater.h"

#include "loginscene.h"



#include "../guicpp/CGUIProgressBar.h"
#include "../guicpp/CImageGUISkin.h"
#include "../guicpp/SkinLoader.h"


using namespace Login;

#include <windows.h>
#include <vfw.h>
#include <stdio.h>
#pragma comment(lib,"vfw32.lib")

bool playBGM( char* s)
{	 

	HWND hWnd;
	hWnd = MCIWndCreate(0,NULL,0,TEXT( s )); //mp3 wav wma

	ShowWindow(hWnd,SW_HIDE);

	MCIWndPlay(hWnd);

	return 1;

}


LoginCore::LoginCore () {
	
	bquit=false;

   slogin=new SLogin;
   slogin->charSex=1; //Ĭ��ѡ�� ��


	SIrrlichtCreationParameters deviceParam;

	deviceParam.Doublebuffer=true;
	deviceParam.DriverType=irr::video::EDT_DIRECT3D9;

	deviceParam.WindowSize.Width = 800;
	deviceParam.WindowSize.Height = 600;
	deviceParam.Fullscreen = false;
	deviceParam.Bits = 32;
	deviceParam.ZBufferBits = 16;
	deviceParam.Vsync = false;
	deviceParam.AntiAlias=true;
	deviceParam.Stencilbuffer=false; //�������shadow��־
	deviceParam.EventReceiver=this; //������Ϣ��

	device = createDeviceEx(deviceParam);
	//device = createDevice(EDT_DIRECT3D9, dimension2d<u32> (800,600), 32, false, 	  shadows, false,this); 
	

	if(device==NULL){
		MessageBox(0,"����d3d9ʧ��!","",0);
		exit(1);
	}

	//��������������
//	engine = getSoundEngine();
	//engine->play2D("music/��Ѫȼ��.mp3",true);//ֱ�Ӳ���
	playBGM("music/��Ѫȼ��.mp3");

	//ISound* music =engine->play3D("music/rain.wav",vector3df(100,10,290),true,false,true);
	//if (music)
	//	music->setMinDistance(45.0f);
	//music->setMaxDistance(400.f);
//	music->setVolume(0.30f);

	ui=device->getGUIEnvironment();
	smgr=device->getSceneManager();
	drv=device->getVideoDriver();


	//device->setEventReceiver(this);//�����¼�������




		

		//-������Դǰ�Ȼ���ͼ����.��������ȴ�----------------------------------------------------------------------------------------------------


		//ITexture* h=drv->getTexture("textures/loading.jpg");

		//drv->beginScene (true, true, SColor (150, 0, 0, 0)); 

		//ui->drawAll();

		//drv->draw2DImage(h, core::position2d<s32>(0,0)	//ָ�����Ƶ���Ļ��λ��			
		//	);		//ָ��Ҫ��ʾ��ͼ����ͼƬ�н�ȡ�ľ�������

		////TextLeft( "���ڼ�����!",vector2d<s32>(400,400),SColor(1,0,255,0),25,0);
		//drv->endScene (); 


//------------------------------------------------------------------------------------------------------


		io::IFileSystem* filesys = device->getFileSystem();

		// Load GUI
		gui::SImageGUISkinConfig guicfg = LoadGUISkinFromFile(filesys, drv, "ui/guiskin.cfg");
		gui::CImageGUISkin* skin = new gui::CImageGUISkin(drv, ui->getSkin());
		skin->loadConfig(guicfg);
		ui->setSkin(skin);

		//--��ʼ������----------------------------------------------------------------------------------------------------

		char tmp[512];
		GetWindowsDirectory(tmp,511);
		strcat(tmp,"\\fonts\\SIMSUN.TTC");  //SIMSUN.TTC����,������         SIMKAI.TTF����     \STXIHEI.TTF   Fixedsys

		gui::IGUIFont *font;
		gui::IGUISkin* skin2;
		//SIMSUN.TTC����,������         SIMKAI.TTF����     \STXIHEI.TTF   Fixedsys
		font = ui->getFont(tmp,13, true,false);
		skin2 = ui->getSkin();
		skin2->setFont(font);

		


		win = ui->addWindow(core::rect<s32>(0,0,200,100), false, L"");
		win->setRelativePosition(position2di(300,210));
		//win->setVisible(false);

		win->getCloseButton()->setVisible(false);
		//win->getCloseButton()->setToolTipText(L"");
		//win->getCloseButton()->setPosition(vector2d<s32>(0,-2)));

		//�����϶�
		//win->setDraggable(false);


		irr::gui::IGUIStaticText* te=ui->addStaticText(L"��������:",rect<s32>(330,210,400,230),false,false,win);
		te->setRelativePosition(position2di(10,20));
		

		//��������
		editbox= ui->addEditBox(L"", rect<s32>(400, 210, 500, 230),true,win);
		editbox->setRelativePosition(position2di(80,20));
		editbox->setMax(18);//����18���ַ�����

		//��checkbox
		mancheck=ui->addCheckBox(true, core::rect<s32>(0,0,20,20), win,3 );
		mancheck->setRelativePosition(position2di(20,40));

		//��ʾ "��"
		irr::gui::IGUIStaticText* t=ui->addStaticText(L"��", core::rect<s32>(0, 0, 150, 25), false, false, win);
		t->setRelativePosition(position2d<s32>(40,40));


		//Ůcheckbox
		girlcheck=ui->addCheckBox(false, core::rect<s32>(0,0,40,20), win ,4 );
		girlcheck->setRelativePosition(position2di(60,40));

		//��ʾ "Ů"
		t=ui->addStaticText(L"Ů", core::rect<s32>(100, 30, 150, 65), false, false, win);
		t->setRelativePosition(position2d<s32>(80,40));


		//�����������Ե㰴ť
		btn_login=ui->addButton(core::rect<s32>(0,0,15*4,20), win, 1, L"��¼" );
		//����ֻ��Ҫ�����������,��Ϊ��ָ���˸�����
		position2d<s32> p1(30,70);
		btn_login->setRelativePosition(p1);

		btn_quit=ui->addButton(core::rect<s32>(0,0,15*4,20), win, 2, L"�˳�" );
		position2d<s32> p2(110,70);
		btn_quit->setRelativePosition(p2);

//-----------------------------------------------------------------------------------------------------
//��½�Ľ�ɫս��

mgr=new MyLoginCharClothMgr(device,1,1);
mgr->LoadBody(3);
mgr->setPosition(vector3df(vector3df(120,0.50,175)));
mgr->setRotation(vector3df(0,230,0)); //220
mgr->Animate(4,21,28,true);

 backweapon=new Login::MyWeapon2(device);


scene::ISceneNode* pJointNode = mgr->getRootNode()->getMS3DJointNode("joint23");  //   joint23 �Ǳ�
if(pJointNode)
{

 
	//�������� ��� 
	backweapon->ChangeWeapon(randomNum(1,9));

	backweapon->getNode()->setID(101);
	//backweapon->setUseHLSL(true);
	backweapon->getNode()->setScale(vector3df(6,6,6));
	backweapon->getNode()->setRotation(vector3df(180,0,110));
	backweapon->setPosition(vector3df(0,1,0));

	//�󶨵�����
	pJointNode->addChild( backweapon->getNode() );

 
	 //���  ����
	 backweapon->setUseHLSL(true,false,randomNum(1,6));
}

//-----------------------------------------------------------------------------------------------------
//�˷ɽ���



//�ɽ�
 flyweapon=new Login::MyWeapon2(device);
flyweapon->ChangeWeapon(9);
flyweapon->getNode()->setID(101);
flyweapon->getNode()->setScale(vector3df(13,13,13));
flyweapon->getNode()->setRotation(vector3df(0,40+270,0));
flyweapon->setPosition(vector3df(1800 ,172, 1860));
//��ʾ����
flyweapon->setUseHLSL(true,false,6);


//�ɽ�Ʈ��������

scene::IParticleSystemSceneNode* ps ;//����ϵͳ

 ps = device->getSceneManager()->addParticleSystemSceneNode(false); //����һ������ϵͳ,��ʹ���Զ���ķ�����
//ps->setPosition(c->getPosition()+vector3df(0,50,0));//λ��
//ps->setScale(core::vector3df(0.5f,0.5f,0.5f));//����
//ps->setParticleSize(core::dimension2d<f32>(0.250f, 0.250f));//�������Ӵ�С
ps->setParent(flyweapon->getNode());
ps->setParticlesAreGlobal(true);

flyem=ps->createPointEmitter(vector3df(-0.03,0,0),200,200,
						video::SColor(0,0,0,0), video::SColor(0,40,40,45),//��ʼ��ɫ�ͽ�����ɫ
						  2000,2000   //���ʱ��,800-2000����
						  );
flyem->setMaxStartSize(core::dimension2d<f32>(40.0f, 40.0f));
flyem->setMinStartSize(core::dimension2d<f32>(40.0f, 40.0f));

////�������ӷ�����
//em = ps->createBoxEmitter(
//	core::aabbox3d<f32>(-4,-4,-4,4,4,4),//���ú��ӵ�6������
//	core::vector3df(0.0f,0.0f,0.0f),//���÷��䷽����ٶ�
//	500,700,//ÿ����С������80,���100
//	video::SColor(0,0,0,0), video::SColor(0,255,255,255),//��ʼ��ɫ�ͽ�����ɫ
//	500,1000);//���ʱ��,800-2000����
ps->setEmitter(flyem);//����ϵͳ���÷�����
flyem->drop();

scene::IParticleAffector* paf =ps->createFadeOutParticleAffector();
ps->addAffector(paf);
paf->drop();

ps->setMaterialFlag(video::EMF_LIGHTING, false);
ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
ps->setMaterialTexture(0, drv->getTexture("textures/flare.bmp"));
ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);



//�ɽ�ֱ�߷�
scene::ISceneNodeAnimator* anim2;
anim2 = smgr->createFlyStraightAnimator(vector3df(5800,1572,1500),vector3df(0,1572,2500),10000,false);  //��Y����ת �ٶ�0.3
flyweapon->getNode()->addAnimator(anim2);
anim2->drop();


//�ɽ��ϵ���
man=new MyLoginCharClothMgr(device,1,1);
man->LoadBody(3);
man->getRootNode()->setParent(flyweapon->getNode());
man->setPosition(vector3df(vector3df(0,0,3)));
man->setRotation(vector3df(0,180,0)); //220
man->Animate(4,41,49,true);

//��ɫ������


if(man->getRootNode()->getMS3DJointNode("joint23"))
{
	man->getRootNode()->getMS3DJointNode("joint23")->removeAll();

	Login::MyWeapon2* weapon=new Login::MyWeapon2(device);
	//��������
	weapon->ChangeWeapon(9);
	weapon->getNode()->setID(101);
	//backweapon->setUseHLSL(true);
	weapon->getNode()->setScale(vector3df(6,6,6));
	weapon->getNode()->setRotation(vector3df(180,0,110));
	weapon->setPosition(vector3df(0,1,0));

	//�󶨵�����
	man->getRootNode()->getMS3DJointNode("joint23")->addChild( weapon->getNode() );

	//��ʾ����
	weapon->setUseHLSL(true,false,5);
}

//-----------------------------------------------------------------------------------------------------



	//1.6
	drv->setFog(video::SColor(0,100,0,0),EFT_FOG_LINEAR, 250, 1150, 0.015, true);




	// attach billboard to light
	//node = smgr->addBillboardSceneNode(node, core::dimension2d<f32>(50, 50));
	//node->setMaterialFlag(video::EMF_LIGHTING, false);
	//node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	//node->setMaterialTexture(0, drv->getTexture("particlewhite.bmp"));

	 cam=smgr->addCameraSceneNodeFPS(0,100,0.125);
	//cam->setPosition(vector3df(13.4,9.1,-25.7));
	cam->setAutomaticCulling(EAC_FRUSTUM_BOX );
	cam->setPosition(vector3df(95.6, 10.9, 166.9));
	cam->setTarget(vector3df(208.6 ,18.5 ,322.4));
	cam->setFarValue(20000);
	cam->setInputReceiverEnabled(false);



	smgr->loadScene("scene/login.irr");



	RealisticWaterSceneNode* water=new RealisticWaterSceneNode(smgr, 180, 180, 
	//drv->getTexture("waterpool/sn218.JPG"), d->getTimer());
	drv->getTexture("water/waterbump.png"), device->getTimer());
	water->setPosition(vector3df(100,-1.80,290));
//	water->setWaveHeight(1);
	water->setRotation(vector3df(0,180,0));
	water->setWaterColor(SColor(0,100,0,0));





	drv->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	scene::ISceneNode* skydome2=smgr->addSkyDomeSceneNode(drv->getTexture("textures/Skydome3.tbj"),16,8,0.95f,2.0f);
	skydome2->setMaterialFlag(video::EMF_FOG_ENABLE, true);//��
	skydome2->setMaterialFlag(video::EMF_LIGHTING, false);
	//skydome2->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);// ���������͸����..
	//	//��ת������
	scene::ISceneNodeAnimator* anim = 0;
	anim = smgr->createRotationAnimator(core::vector3df(0,0.01f,0));
	skydome2->addAnimator(anim);
	anim->drop();
	drv->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);




	//��������Ч��
	MyUVAnimationEffect* e=new MyUVAnimationEffect(device) ;

	waterfall = smgr->addAnimatedMeshSceneNode(smgr->getMesh("model/waterfall/water.ms3d"));
//	waterfall->setMaterialTexture(1,drv->getTexture("waterfall/d6e1ed0.tga"));
//	waterfall->setMaterialTexture(1,drv->getTexture("_wa_water1.bmp"));
	waterfall->setMaterialFlag(video::EMF_LIGHTING, true);
	waterfall->setMaterialFlag(video::EMF_FOG_ENABLE, true);//��
	waterfall->setMaterialFlag(EMF_BACK_FACE_CULLING ,false); //�رձ����޳�
	
	waterfall->setPosition(vector3df(100,-27,290));
	waterfall->setAutomaticCulling(EAC_FRUSTUM_BOX );

	waterfall->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);// ���������͸����. EMT_TRANSPARENT_ALPHA_CHANNEL. EMT_TRANSPARENT_ADD_COLOR
	waterfall->setRotation(vector3df(0,-80,0));
	waterfall->setScale(vector3df(1,3.5,1));


	waterfall->setMaterialType((irr::video::E_MATERIAL_TYPE)e->getMaterialType());

	smgr->setAmbientLight(video::SColor(255,30,0,0));


//---------------------------------------------------------------------------------------------------------------
	// ���� ����ϵͳ	 ������ٲ�����������ϵͳ

	//������
	scene::IParticleEmitter* em;
//		scene::IParticleSystemSceneNode* ps ;//����ϵͳ
	video::SColor startcolor,endcolor;

		ps = device->getSceneManager()->addParticleSystemSceneNode(false); //����һ������ϵͳ,��ʹ���Զ���ķ�����
		//ps->setPosition(c->getPosition()+vector3df(0,50,0));//λ��
		//ps->setScale(core::vector3df(0.5f,0.5f,0.5f));//����
		ps->setParticleSize(core::dimension2d<f32>(2.50f, 2.50f));//�������Ӵ�С


		////�������ӷ�����
		em = ps->createBoxEmitter(
			core::aabbox3d<f32>(-8,-8,-8,8,8,8),//���ú��ӵ�6������
			core::vector3df(0.0f,-0.09f,0.0f),//���÷��䷽����ٶ�
			600,700,//ÿ����С������80,���100
			video::SColor(0,0,0,5), video::SColor(0,30,30,30),//��ʼ��ɫ�ͽ�����ɫ
			2000,3000);//���ʱ��,800-2000����
		ps->setEmitter(em);//����ϵͳ���÷�����
		em->drop();

//		scene::IParticleAffector* paf =ps->createFadeOutParticleAffector();
		ps->addAffector(paf);
		paf->drop();

		ps->setPosition(vector3df(100,110,290));
		ps->setMaterialFlag(video::EMF_LIGHTING, false);
		ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
		ps->setMaterialFlag(video::EMF_FOG_ENABLE, false);//��
		ps->setMaterialTexture(0, drv->getTexture("textures/flare.bmp"));
		ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);




//---------------------------------------------------------------------------------------------------------------
//������ٲ���ˮ̶������ϵͳ.ģ��Ư������

		ps = device->getSceneManager()->addParticleSystemSceneNode(false); //����һ������ϵͳ,��ʹ���Զ���ķ�����
		//ps->setPosition(c->getPosition()+vector3df(0,50,0));//λ��
		//ps->setScale(core::vector3df(0.5f,0.5f,0.5f));//����
		ps->setParticleSize(core::dimension2d<f32>(0.50f, 0.50f));//�������Ӵ�С


		////�������ӷ�����
		em = ps->createBoxEmitter(
			core::aabbox3d<f32>(-40,-40,-40,40,40,40),//���ú��ӵ�6������
			core::vector3df(0.0f,0.007f,0.0f),//���÷��䷽����ٶ�
			300,600,//ÿ����С������80,���100
			video::SColor(0,10,10,10), video::SColor(0,10,10,10),//��ʼ��ɫ�ͽ�����ɫ
			900,1000);//���ʱ��,800-2000����
	//	em->setMinParticlesPerSecond(300);
	//	em->setMaxParticlesPerSecond(500);

		ps->setEmitter(em);//����ϵͳ���÷�����

		em->drop();
		

		paf =ps->createFadeOutParticleAffector();
		ps->addAffector(paf);
		paf->drop();

		ps->setPosition(vector3df(110.0, 1.3, 260.0));
		ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
		ps->setMaterialFlag(video::EMF_LIGHTING, false);
		ps->setMaterialTexture(0, drv->getTexture("textures/flare.bmp"));
		ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);


     bquit = false; 
}


LoginCore::~LoginCore () { 
	//devicec->loseDevice();
     //   devicedrop (); 
} 

bool LoginCore::OnEvent (const SEvent& e) { 


        // ��������¼�
	    if (e.EventType == EET_KEY_INPUT_EVENT) {
                if (e.KeyInput.Key == KEY_ESCAPE && e.KeyInput.PressedDown==false) { 
                        //quit = true; 
                } 
				if(e.KeyInput.Key==KEY_SPACE && e.KeyInput.PressedDown==false) {
					//	debug_info=!debug_info;
					/*vector3df pos,t;
					pos=cam->getPosition();
					t=cam->getTarget();
					char str[255];
					sprintf(str,"%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f",pos.X,pos.Y,pos.Z,t.X,t.Y,t.Z);
					WriteToLog(str);*/

				}
 
			
				 return false; 
		}


		//gui�¼�
		if (e.EventType == EET_GUI_EVENT)
		{
			s32 id = e.GUIEvent.Caller->getID();//Ԫ�ض����id
			//e.GUIEvent.Caller���Ƿ����¼���guiԪ�ض���.
			switch(e.GUIEvent.EventType)
			{
			case EGET_CHECKBOX_CHANGED: //checkbox�ı�״̬
				{
					//������
					if(id==3){
						//MessageBox(0,"3","",0);
						irr::gui::IGUICheckBox* check=(irr::gui::IGUICheckBox*)e.GUIEvent.Caller;
                        if (check->isChecked())
                        {
                            slogin->charSex=1; //��
							girlcheck->setChecked(false);
							
                        }
						break;
					}
				

					//����Ů
					if(id==4){
						//MessageBox(0,"4","",0);
						irr::gui::IGUICheckBox* check=(irr::gui::IGUICheckBox*)e.GUIEvent.Caller;
						if (check->isChecked())
						{
							slogin->charSex=2; //Ů
							mancheck->setChecked(false);
						}
						break;
					}
				
				}


			case EGET_BUTTON_CLICKED:
				{
					if(id==1)
					{
						
						ret=1; //��ǰ�ť����

						//�õ���ɫ����
						
						//�ַ�ת��
						 stringw s=L"";
						 s.append(editbox->getText());
						char* te=WCharToMByte2(s.c_str());
						slogin->rolename=te;//���浽�ṹ
						
						

							drv->removeAllHardwareBuffers(); //��Ч
						//�˳���,����ڴ��е���������ͼƬ
							drv->removeAllTextures(); //��Ч
						device->closeDevice();
						// device->drop();
						break;
					}

					if(id==2)
					{
						
						ret=0; //����˳���ť����
						device->closeDevice();
						device->drop();
						break;
					}

				}
			}
		}

        // Otherwise send events to CEGUI 
		//�����¼�������,������cegui.����Ҫ��ȡ�¼�
        return false; 
} 



SLogin* LoginCore::Run () { 

char temp[255]="";

        int last_fps = -1; 
DWORD lasttime=GetTickCount();
        while (device->run () && !bquit) { 
                if (device->isWindowActive ()) { 

					//����6���һ��
					if(GetTickCount()-lasttime>16000){
						lasttime=GetTickCount();


						//ÿ���л���ͬ����
						if(man->getRootNode()->getMS3DJointNode("joint23"))
						{
							man->getRootNode()->getMS3DJointNode("joint23")->removeAll();

							MyWeapon2* weapon=new MyWeapon2(device);
							//����л�����
							weapon->ChangeWeapon(randomNum(1,9));
						
							//backweapon->setUseHLSL(true);
							weapon->getNode()->setScale(vector3df(6,6,6));
							weapon->getNode()->setRotation(vector3df(180,0,110));
							weapon->setPosition(vector3df(0,1,0));

							//�󶨵�����
							man->getRootNode()->getMS3DJointNode("joint23")->addChild( weapon->getNode() );

							//�������
							weapon->setUseHLSL(true,false,randomNum(1,6));
						

							//�����ɫ
							SColor co=SColor(25,randomNum(100,255),randomNum(0,255),randomNum(0,255));
							flyem->setMaxStartColor(co);
							flyem->setMinStartColor(co);

						}

						//�ɽ�ֱ�߷�
						scene::ISceneNodeAnimator* anim2;
						
						//����߶�
						float height=randomNum(300,1572);
						float height2=randomNum(300,1572);

						int re=randomNum(1,100);
						if(re%2==0){
							//���������
anim2 = smgr->createFlyStraightAnimator(vector3df(6000,height,2500),vector3df(0,height2,2500),randomNum(5000,10000),false); 

//�����ɽ�����
flyweapon->getNode()->setRotation(vector3df(0,40+270,0));

						}else{
anim2 = smgr->createFlyStraightAnimator(vector3df(0,height2,2500),vector3df(6000,height,2500),randomNum(5000,10000),false); 
//�������ҷ�

//�����ɽ�����
flyweapon->getNode()->setRotation(vector3df(0,40+90,0));


						}
				
						flyweapon->getNode()->addAnimator(anim2);
						anim2->drop();


					}

				backweapon->update();

					//engine->setListenerPosition(cam->getPosition(),cam->getTarget());

					smgr->setAmbientLight(video::SColor(0,60,60,60));

					//n->setScale(core::vector3df(5, 256.0f,250.0f-num));
						
                        drv->beginScene (true, true, SColor (150, 0,0, 0)); 
		
						smgr->drawAll();
							

						sprintf(temp,"fps:%d",last_fps);
						myTextLeft(temp,vector2d<s32>(0,2),SColor(1,255,0,0),18,0);
			
						
						ui->drawAll();

                        drv->endScene (); 

	
                        int fps = drv->getFPS (); 
					
                        if (last_fps != fps) { 
                                core::stringw str = L"    FPS: "; 
                                str += fps; 
                                device->setWindowCaption (str.c_str ()); 
                                last_fps = fps; 
                        } 
                } 
        } 



	 	drv->removeAllHardwareBuffers(); //��Ч
		//�˳���,����ڴ��е���������ͼƬ
	 	drv->removeAllTextures(); //��Ч





if(ret==1) return slogin;
else return NULL;

} 


char* LoginCore::WCharToMByte2(LPCWSTR lpcwszStr)
{
	//����Ķ��ַ�����ռ�ĸ���
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP,0,lpcwszStr,-1,NULL,0,NULL,FALSE)+4;
	// printf("����:%d\n",dwNum);
	char* temp=new char[dwNum];
	WChar2MByte(lpcwszStr,temp,dwNum);

	return temp;
}



//�����
void LoginCore::myTextLeft( const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight)
{

	
	if(ui==0) return;

	WCHAR * pwStr = ConvertLPCSTRToLPWSTR((char*)str); 
	gui::IGUIFont *font;

	//STSONG.TTF ��������   SIMFANG.TTF  STFANGSO.TTF ���ķ���   SIMSUN.TTC����,������          SIMKAI.TTF���Ŀ���        Fixedsys
	font = ui->getDXFont("����",false,fontsize,0,weight);

	int textsize=strlen(str);
	core::rect<s32> AbsoluteRect( pos, pos+ core::dimension2d<s32>(textsize*fontsize,fontsize)); 
	font->draw(pwStr,AbsoluteRect,color);
	delete []pwStr;
	//	delete []temp;
}
