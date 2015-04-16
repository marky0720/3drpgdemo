#include "StdAfx.h"
#include ".\myscenemanager.h"

MySceneManager* sm;




char filestr[255]="vector.txt";
	ITimer* timer;
	u32 lasttime;

MySceneManager::MySceneManager(  Socket::CNetClient* client)
{
sm=this;

this->client=client;

myCamera=NULL;
monster_mgr=NULL;
char_mgr=NULL;
npc_mgr=0;
item_mgr=0;
myGUI_Mgr=0;

dev=getIrrDevice();
smgr=getIrrSmgr();
drv=getIrrDriver();

mouseDownL=false;
mouseDownR=false;

muoseclick=0;
lastmouseclick=0;

timer=dev->getTimer();
lasttime=timer->getRealTime();

//初始化场景加载器,负责切换场景资源
myscene_loader=0;
myscene_loader=new MySceneLoader();

changeworld=false;

}

MySceneManager::~MySceneManager(void)
{
}

void MySceneManager::Render()
{
   		//必须提供一个矩阵--------------------------------------------------------------------------------------------------------------------------
		core::matrix4 mat;
		video::SMaterial material;
		material.TextureLayer[0].Texture= 0;
		material.Lighting = false;
		material.Thickness=2; //设置粗细
		dev->getVideoDriver()->setTransform(video::ETS_WORLD, mat);
		dev->getVideoDriver()->setMaterial(material);

		//-----------------------------------------------------------------------------------------------------------------------------------------	
		//循环输出所有选取的三角形
		core::list<core::triangle3df*>::Iterator  list_oper;
		for(list_oper=myscene_loader->mylist.begin();list_oper!=myscene_loader->mylist.end();++list_oper)
		{
				core::triangle3df* temp_triangle;
				temp_triangle= *list_oper;
				drv->draw3DTriangle(*temp_triangle,video::SColor(255, 255, 0, 0));
		}
}


//卸载当前场景
void MySceneManager::DropCurrScene( )
{
   myscene_loader->DropCurrScene();
}

//加载场景
void MySceneManager::LoadScene(int id)
{
	//调用场景加载器加载
	myscene_loader->LoadScene(id,myCamera->getNode());

	//加载完毕.重新设置角色需要的地图指针.用于求高度.
	if(char_mgr->GetMainCharacter()){
		char_mgr->GetMainCharacter()->sceneloader=myscene_loader;
	}

}

//初始化npc管理器 
void MySceneManager::InitNpcMgr()
{
	npc_mgr=new MyNPCMgr(this);
}

//初始化摄象机
void MySceneManager::InitCamera()
{
	myCamera=new My3rdCamera(0);
}

//初始化角色管理器
void MySceneManager::InitCharacterMgr()
{
	char_mgr=new MyCharacterMgr(this);	
}


/*
添加怪物管理器
*/
void MySceneManager::InitMonsterMgr()
{
	//需要知道device和角色信息
	monster_mgr=new MyMonstersMgr(this,client);
}

void MySceneManager::InitItemMgr()
{
	//初始化物品管理
	item_mgr=new MyItemMgr(this,client);
}

void MySceneManager::InitGUIMgr()
{
  myGUI_Mgr=new MyGUIManager(this);
  myGUI_Mgr->LoadGUI();

}

void MySceneManager::OnEvent (const irr::SEvent& e)
{
	
	//gui管理器接收事件
	if(myGUI_Mgr ){
		myGUI_Mgr->OnEvent(e);
		//是否鼠标在gui内
		if( myGUI_Mgr->GetEditIsInput()==true){
			return;
		}
	}
	//npc管理器
	if(npc_mgr){
		npc_mgr->OnEvent(e);

		if(npc_mgr->MouseInNPC_GUI){
			return;
		}
	}

	//摄象机接收事件
	if(myCamera)    myCamera->OnEvent(e);

	//怪物管理器接收事件
	if(monster_mgr)    monster_mgr->OnEvent(e);

	//角色管理器接收事件
	if(char_mgr)    char_mgr->OnEvent(e);

	
	//键盘输出事件
	   if (e.EventType == EET_KEY_INPUT_EVENT) {
			if (e.KeyInput.Key == KEY_DELETE && e.KeyInput.PressedDown==false) { 
				beginchange(); //change=!change;
		    }
			if (e.KeyInput.Key == KEY_END && e.KeyInput.PressedDown==false) { 
				printf("removeAllTriangleSelectors\n");
				//meta->removeAllTriangleSelectors();
				//CalculMeta();
		    }

			//前进
			if (e.KeyInput.Key == KEY_KEY_W  ) { 
				float speed=char_mgr->GetMainCharacter()->getMoveSpeed();
				vector3df target_pos;
				vector3df campos=myCamera->getNode()->getPosition();
				vector3df rolpos= char_mgr->GetMainCharacter()->getPosition();
				vector3df dir=rolpos-campos;
				dir.normalize();
				target_pos=rolpos+dir*speed*15;
				//char_mgr->GetMainCharacter()->faceTarget(rolpos-dir*10);
				target_pos.Y=myscene_loader->getHeight(target_pos.X,target_pos.Z);//调整高度
				char_mgr->GetMainCharacter()->Beginmove(target_pos);		
			}

			//后退
			if (e.KeyInput.Key == KEY_KEY_S ) { 
				float speed=char_mgr->GetMainCharacter()->getMoveSpeed();
				vector3df target_pos;
				vector3df campos=myCamera->getNode()->getPosition();
				vector3df rolpos=char_mgr->GetMainCharacter()->getPosition();
				vector3df dir=rolpos-campos;
				dir.normalize();
				target_pos=rolpos-dir*speed*15;
				//char_mgr->GetMainCharacter()->faceTarget(rolpos-dir*10);
				target_pos.Y=myscene_loader->getHeight(target_pos.X,target_pos.Z);//调整高度
				char_mgr->GetMainCharacter()->Beginmove(target_pos);		
			}


			/*
			if (e.KeyInput.Key == KEY_KEY_1 && e.KeyInput.PressedDown==false) { 
				
				this->myCamera->setTargetNode(	char_mgr->GetMainCharacter()->m_charClothMgr->getRootNode() );		

			}

			if (e.KeyInput.Key == KEY_KEY_2 && e.KeyInput.PressedDown==false) { 
				this->myCamera->setTargetNode(	char_mgr->GetMainCharacter()->m_charClothMgr->getRootNode() );		
			}

			if (e.KeyInput.Key == KEY_KEY_3 && e.KeyInput.PressedDown==false) { 
				this->myCamera->setTargetNode(	char_mgr->GetMainCharacter()->m_charClothMgr->getRootNode() );		
			}

			if (e.KeyInput.Key == KEY_KEY_4 && e.KeyInput.PressedDown==false) { 
				this->myCamera->setTargetNode(	char_mgr->GetMainCharacter()->m_charClothMgr->getRootNode() );		
			}
			*/
	   }

	   if (e.EventType == EET_MOUSE_INPUT_EVENT) 
	   {
		   if(e.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)    mouseDownL = true; muoseclick++;
		   if(e.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)    mouseDownR = true; 
		   if(e.MouseInput.Event == EMIE_RMOUSE_LEFT_UP)	     mouseDownR = false; 
		   if(e.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)         mouseDownL = false; 
	   }

}

void MySceneManager::beginchange()
{			
	//static int count=1;
	//		if(character){
	//			printf("删除char count:%d\n",count);
	//			char_mgr->GetMainCharacter()->drop();
	//			char_mgr->SetMainCharacter(0);
	//			// char_mgr->GetMainCharacter()=0;	
	//				this->myCamera->setTargetNode(0);//	monster_mgr->GetFirstMonster()->GetNode());
	//				//monster_mgr->setAllMonsterNoTarget();
	//				monster_mgr->SetCharacter(0);
	//				count++;
	//		}else{
	//			printf("添加char count次数:%d\n",count);
	//			   	character=new MyCharacter(dev,"yunhe//yunhe.ms3d");
	//				character->setPosition(vector3df(110,0.5,110));
	//				this->myCamera->setTargetNode(	character->getNode());		
	//				//读取地图可移动区域文件
	//				//给角色类设置可移动区域三角形list
	//				char_mgr->GetMainCharacter()->SetTrigleList(&this->mylist);
	//		}

}

void MySceneManager::ChangeWorld(int worldMap)
{
	//标志 要切换世界了 不能直接在这写卸载场景加载的代码.因为在另一个线程调用的话,会导致不同步.这边删除了,那边在调用.就设置一个参数,让他run渲染线程里切换场景吧
	changeworld=true;
	
	this->worldMap=worldMap;
	
}


void MySceneManager::Show(char* s)
{
	drv->beginScene (true, true, SColor (150, 150, 150, 150)); 

	
	smgr->drawAll();

	dev->getGUIEnvironment()->drawAll();

	TextLeft( s,vector2d<s32>(400,400),SColor(1,0,255,0),25,0);


	drv->endScene (); 
}

void MySceneManager::update()
{

	//怪物管理器更新,并测试是否鼠标选择了怪物
	if(monster_mgr ) monster_mgr->Update();

	//角色管理器更新
	if(char_mgr)    char_mgr->update();

	//npc管理器更新
	if(npc_mgr) npc_mgr->update();


	//camera更新
	if(myCamera)  myCamera->update();

	//场景物品管理器更新
	if(item_mgr) item_mgr->update();

	//gui管理器更新
	if(myGUI_Mgr) myGUI_Mgr->update();

	//场景加载器更新
	if(myscene_loader) myscene_loader->update();
	 
	
	//反复更新,就反复发送信息吧
	
	//指针存在则表示这个主角色已建立了
	if(char_mgr->GetMainCharacter())
	{

				u32 usetime=timer->getRealTime()-lasttime;
					
					//每秒向服务器更新一次角色信息
					if( usetime > 1000)
					{
						lasttime=timer->getRealTime();

						char_mgr->GetMainCharacter()->ReoprtToServer();
					}
					
				
	}
	


	//是否要切换世界
	if(changeworld){
		
		Show("加载场景");

		changeworld=false;

		//角色移动到新场景的坐标
		char_mgr->GetMainCharacter()->setPosition(char_mgr->GetMainCharacter()->getPosition());

		//设置已到达.
		char_mgr->GetMainCharacter()->idle(false);

		//Show("卸载当前场景");

		//卸载当前场景
		DropCurrScene();

		//地图在线其他角色清空
		char_mgr->RemoveAllCharacter();

		//地图在线怪物清空
		monster_mgr->RemoveAllMonsters();

		//地图的掉落物品清空
		item_mgr->RemoveAllItem();

		//场景npc清空
		npc_mgr->RemoveAllNPC();

		//加载新场景
		LoadScene(worldMap);

		//场景npc加载
		npc_mgr->LoadSceneNPC(worldMap);

		//Show("加载新场景");
		
		//请求世界所有在线角色
		char_mgr->getOnlineAllCharacter();

		//请求场景所有怪物
		monster_mgr->getMonsterCMD_MonsterAll();

		//Sleep(200);

		//请求场景所有物品
		item_mgr->getSceneCMD_ITEMDROPAll();


	}



	
  //鼠标左键 移动或物理攻击怪物
  if(mouseDownL && muoseclick!=lastmouseclick)
  {
	  lastmouseclick=muoseclick;

	  //是否鼠标指向了怪物,如果是就表示想攻击怪物
	   if( monster_mgr->GetNodeIsselected()){
		  printf("scene GetNodeIsselected\n");
		     char_mgr->GetMainCharacter()->curAttackType = MyCharacter::eAttacktype::PowerAttack;  //攻击类型
		  if(monster_mgr->GetselectedMonster()->IsDead()==false) char_mgr->GetMainCharacter()->SetWantToAttackTarget( monster_mgr->GetselectedMonster());
		   //char_mgr->GetMainCharacter()->FollowAndAttackTarget();

	   }

	   //是否选择了npc
	   if(npc_mgr->IsselectedNPC()){
			//选择了就显示对话框
            npc_mgr->GetselectedNPC()->ShowTalkDlg();
	   }
	 
  }

  //右键技能攻击
  if(mouseDownR ){
	  //是否鼠标指向了怪物,如果是就表示想攻击怪物
	  if( monster_mgr->GetNodeIsselected()){
		  printf("scene GetNodeIsselected\n");
		   char_mgr->GetMainCharacter()->curAttackType = MyCharacter::eAttacktype::MagicAttack;  //攻击类型
		  if(monster_mgr->GetselectedMonster()->IsDead()==false) char_mgr->GetMainCharacter()->SetWantToAttackTarget( monster_mgr->GetselectedMonster());
		 // char_mgr->GetMainCharacter()->FollowAndAttackTarget();

	  }
  }


//  printf("%3.1f %3.1f %3.1f\n", b.MaxEdge.X,b.MaxEdge.Y,b.MaxEdge.Z);

}



//输出字体
void MySceneManager::TextRender()
{


}


//2d图片绘制
void MySceneManager::UIRender()
{
	
	//gui管理类显示
	if(myGUI_Mgr) myGUI_Mgr->drawAll();

	//怪物管理器 ui显示 比如text 血条
	if(monster_mgr) monster_mgr->UIRender();

	//角色管理器显示
	if(char_mgr) char_mgr->UIRender();

	//物品管理器显示
	if(item_mgr) item_mgr->UIRender();

	//npc管理器显示
	if(npc_mgr) npc_mgr->UIRender();

	//主角色还未加载完毕,不进行后面操作.
	if(char_mgr->GetMainCharacter()==NULL) return;
	
	if(monster_mgr->GetNodeIsselected() ||	char_mgr->GetMainCharacter()->isfollow || char_mgr->GetMainCharacter()->getState() == cAttack)
	{
			//core::position2d<s32> m = dev->getCursorControl()->getPosition();
			//drv->draw2DImage(cur2, core::position2d<s32>(m.X,m.Y),
			//core::rect<s32>(0,0,32,32), 0, 
			//video::SColor(255, 255,255,255), true);
			//
			long normal= (long)LoadCursor( ::GetModuleHandle(0), MAKEINTRESOURCE(IDC_CURSOR2) );
	
			SExposedVideoData exposedData=	drv->getExposedVideoData();
			HWND h = reinterpret_cast<HWND>(exposedData.D3D9.HWnd);
			::SetClassLong(h,GCL_HCURSOR,normal);


	}else{
			long normal= (long)LoadCursor( ::GetModuleHandle(0), MAKEINTRESOURCE(IDC_CURSOR3) );
	
			SExposedVideoData exposedData=	drv->getExposedVideoData();
			HWND h = reinterpret_cast<HWND>(exposedData.D3D9.HWnd);
			::SetClassLong(h,GCL_HCURSOR,normal);
			//core::position2d<s32> m = dev->getCursorControl()->getPosition();
		 // drv->draw2DImage(cur1, core::position2d<s32>(m.X,m.Y),
			//core::rect<s32>(0,0,32,32), 0, 
			//video::SColor(255, 255,255,255), true);
	}
}
