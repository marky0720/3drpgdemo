#include ".\mynpcmgr.h"

MyNPCMgr::MyNPCMgr( MySceneManager* mm)
{
	this->dev=getIrrDevice();

	this->m_pMySceneMgr=mm;

	smgr=getIrrSmgr();


	::InitializeCriticalSection(&g_cs_npclist);//初始化临界区对象

	::InitializeCriticalSection(&g_cs_msglist);//初始化临界区对象

	NPCselected=false;
	selectedNPCnode=0;

	MouseInNPC_GUI=false;
}

MyNPCMgr::~MyNPCMgr(void)
{
}

//插入npc消息到链表,来自服务器
void MyNPCMgr::insertNPCData(char* pData,int DataLength)
{
	printf(" MyNPCMgr::insertNPCData\n");
	ClientMsg* m=new ClientMsg();
	char* tmp=new char[DataLength];
	memcpy(tmp,pData,DataLength);
	m->pData=tmp;
	m->size=DataLength;


	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_msglist);
	msglist.push_back(m); //添加处理进链表
	::LeaveCriticalSection(&g_cs_msglist);
	//////---------------------------------------------------------------------------------------------///////////////	


}

void MyNPCMgr::setVisiable(bool v)
{
	::EnterCriticalSection(&g_cs_npclist);
	std::vector<MyNPC*>::iterator list_oper;
	for(list_oper=npc_list.begin();list_oper!=npc_list.end();++list_oper)
	{

		MyNPC *p=*list_oper;

		if(p) 
		{
			p->getNode()->setVisible(v);

		}
	}


	::LeaveCriticalSection(&g_cs_npclist);
}


//接收事件
void MyNPCMgr::OnEvent (const irr::SEvent & e)
{
	bool r=false;
	::EnterCriticalSection(&g_cs_npclist);
	std::vector<MyNPC*>::iterator list_oper;
	for(list_oper=npc_list.begin();list_oper!=npc_list.end();++list_oper)
	{

		MyNPC *p=*list_oper;

		if(p) 
		{
			p->OnEvent(e);
			if(r==false){

				//鼠标指向npc或npc的对话框
				if(p->mouseCurNPC_Dlg)
				{
					r=true;
				}
			}
			
		}
	}


	::LeaveCriticalSection(&g_cs_npclist);
	//////---------------------------------------------------------------------------------------------///////////////	

	MouseInNPC_GUI=r;
}

//移出场景所有NPC.用于切换场景时的.
void MyNPCMgr::RemoveAllNPC()
{

	::EnterCriticalSection(&g_cs_npclist);
	std::vector<MyNPC*>::iterator list_oper;
	for(list_oper=npc_list.begin();list_oper!=npc_list.end();++list_oper)
	{

		MyNPC *p=*list_oper;

		if(p) 
		{
			p->drop();
			delete p;
			p=0;
		}
	}

	npc_list.clear();
	
	::LeaveCriticalSection(&g_cs_npclist);
	//////---------------------------------------------------------------------------------------------///////////////	

}

/*

添加一个npc到场景中

*/
void MyNPCMgr::AddNPC(NPCProperty prop)
{

	::EnterCriticalSection(&g_cs_npclist);
	MyNPC* n=new MyNPC(prop,m_pMySceneMgr->client);
	npc_list.push_back(n); //添加进链表 加载模型.因为npc加载是在主线程里全部加载完的.
	//beast->SetNetServer(this->net);

	::LeaveCriticalSection(&g_cs_npclist);

	//	beast->GetNode()->setDebugDataVisible(scene::EDS_BBOX ); //设置显示包围盒

	printf("npc%d完全信息添加完毕!\n",prop.ID);

}

//场景npc加载
void MyNPCMgr::LoadSceneNPC(int worldMap)
{
	//设置当前场景地图id
  this->currWorldMap=worldMap;
	
  //场景配置文件
  char FilePath[255]="scene//scene.ini";

  char inBuf[255];     //用于保存读取的字符串
  char temp[255]="";
  sprintf(temp,"scene%d",currWorldMap); 

  //此场景npc数量
  int sceneNpcNum=0;
  sceneNpcNum=IniGetInt(FilePath,temp,"npc_num");
	
  //循环添加当前场景地图的所有npc
  for (int i=1;i<=sceneNpcNum;i++)
  {	
	  NPCProperty p;
	  p.cmd=0;
	  p.ID=i;

	  char str[255];
	  sprintf(str,"npc%d_POS",i); //读取第i个npc的坐标
	  if( IniGetString(FilePath,temp,str,inBuf) ){
		
		  sscanf(inBuf, "%f,%f,%f",   &p.pos.X,&p.pos.Y,&p.pos.Z  ); 
		 
	  }
	
	  sprintf(str,"npc%d_name",i); //读取第i个npc的name
	  if( IniGetString(FilePath,temp,str,inBuf) ){
		
		 strcpy(p.strNPCName,inBuf);
	  }

	  sprintf(str,"npc%d_Rot",i); //读取第i个npc的旋转
	  if( IniGetString(FilePath,temp,str,inBuf) ){

		  sscanf(inBuf, "%f,%f,%f",   &p.rot.X,&p.rot.Y,&p.rot.Z); 

	  }

	  sprintf(str,"npc%d_talk",i); //读取第i个npc的对玩家说的话
	  if( IniGetString(FilePath,temp,str,p.strNpcTalk) ){

	  }

	  sprintf(str,"npc%d_btnText",i); //读取第i个npc的对话框按钮的文字
	  if( IniGetString(FilePath,temp,str,p.strBtnText) ){
			//MessageBox(0,p.strBtnText,"",0);
	  }else{
		  //读取失败,说明该npc没有进一步的事件.
		strcpy(p.strBtnText,"");
	  }



	  sprintf(str,"npc%d_btnClickcmd",i); //读取第i个npc的按钮对应的事件命令

	   p.cmd=IniGetInt(FilePath,temp,str);
		
	  

		AddNPC(p);
  }


}


MyNPC*  MyNPCMgr::GetNpcByID(int id)
{
	MyNPC* p_npc=0;
	::EnterCriticalSection(&g_cs_npclist);
	std::vector<MyNPC*>::iterator list_oper;
	for(list_oper=npc_list.begin();list_oper!=npc_list.end();++list_oper)
	{
		MyNPC *p=*list_oper;

		//id正确就返回
		if(p->getProp().ID==id) 
		{
			p_npc=p;
		}
	}
	::LeaveCriticalSection(&g_cs_npclist);

	return p_npc;
}


void MyNPCMgr::update()
{

	::EnterCriticalSection(&g_cs_npclist);
	std::vector<MyNPC*>::iterator list_oper;
	for(list_oper=npc_list.begin();list_oper!=npc_list.end();++list_oper)
	{

		MyNPC *p=*list_oper;

		if(p) 
		{
			//先标志未鼠标指向npc 
			p->mouseCurNPC=false;

			//先都可见
			p->getNode()->setVisible(true);

			p->update();

//---------------------------------------------------------------------------------------------------------------------
	//此代码是用于获得npc和cam的线段.然后和场景模型碰撞.如果有碰撞就说明他们遮挡了.把npc隐藏
			//得到npc坐标 是固定了的
			p->line.start=p->getProp().pos;
			p->line.start.Y+=10;
			//得到摄象机的坐标 
			p->line.end= m_pMySceneMgr->myCamera->getNode()->getPosition();
			//line.end.Y+=10;
			vector3df vec=m_pMySceneMgr->char_mgr->GetMainCharacter()->getPosition()-p->getProp().pos;
			vec.normalize();
		//	p->line.end-=vec*5;
		//	p->line.start-=vec*5;

			core::triangle3df triangle;
			const ISceneNode* outnode;
			//让npc到摄象机的3d线段和场景地形,建筑模型碰撞.如果有.就说明他们之间有遮挡.就把npc隐藏.
			if (smgr->getSceneCollisionManager()->getCollisionPoint(
				p->line, //[in] 射线
				m_pMySceneMgr->myscene_loader->getMeta(), //[in] 场景节点的三角形选择器
				p->line.end, //[out] 返回碰撞点
				triangle,outnode)) //[out] 返回碰撞的三角
			{
			   //printf("npc select id:%d\n",outnode->getID());
				
				p->getNode()->setVisible(false);
			}
	//---------------------------------------------------------------------------------------------------------------------

			/*
			ISceneNode * select_npc_node =smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(
				line,
				intersection, // This will be the position of the collision
				hitTriangle, // This will be the triangle hit in the collision
				0, // This ensures that only nodes that we have
				// set up to be pickable are considered
				0); // Check the entire scene (this is actually the implicit default)

			//拾取了某节点
			if(select_npc_node){

				printf("npc select id:%d\n",select_npc_node->getID());
				MyNPC* sel_npc=GetNpcByID(select_npc_node->getID());

				//拾取的不是npc
				if( sel_npc==NULL ){

					//否则就隐藏npc.因为遮挡了
					//p->getNode()->setVisible(false);
				}else{
					printf("npc%s未遮挡\n",p->getProp().strNPCName);
					p->getNode()->setVisible(true);
				}
				////获得的节点是npc的话,表示中间没有遮挡
				//if(select_npc_node==p->getNode()){
				//	p->getNode()->setVisible(true);
				//}else{
				//	//否则就隐藏npc.因为遮挡了
				//	p->getNode()->setVisible(false);
				//}
			}else{
				//否则就隐藏npc.因为遮挡了
				//p->getNode()->setVisible(false);
			}
	
*/


		}
	}
	::LeaveCriticalSection(&g_cs_npclist);
	//////---------------------------------------------------------------------------------------------///////////////	



	//////---------------------------------------------------------------------------------------------///////////////	
	//鼠标拾取npc 然后让角色对话操作
	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
	//得到屏幕鼠标转换到3d场景的一条射线
	vector2d<s32> curpos=dev->getCursorControl()->getPosition();
	core::line3d<f32> line;
	line=smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(curpos);
	//方法2.
	core::vector3df intersection;
	core::triangle3df hitTriangle;
	ISceneNode * select_npc_node =collMan->getSceneNodeAndCollisionPointFromRay(
		line,
		intersection, // This will be the position of the collision
		hitTriangle, // This will be the triangle hit in the collision
		0, // This ensures that only nodes that we have
		// set up to be pickable are considered
		0); // Check the entire scene (this is actually the implicit default)

	//拾取了npc
	if(select_npc_node){
		//printf("拾取%d!\n",select_npc_node->getID() );
		//根据节点得到怪物类
		MyNPC* sel_npc=GetNpcByID(select_npc_node->getID());
		
		//拾取的不是npc
		if( sel_npc==NULL ){
			NPCselected=false;

			MouseInNPC_GUI=false;
		}else {
			NPCselected=true;	
			selectedNPCnode=sel_npc;

			sel_npc->mouseCurNPC=true;
			

			MouseInNPC_GUI=true;

			//printf("拾取了Npc%s \n", sel_npc->getProp().strNPCName );
		}
	}else{
		NPCselected=false;

		MouseInNPC_GUI=false;
		// printf("拾取失败!!!!\n");
	}

}

void MyNPCMgr::UIRender()
{
	::EnterCriticalSection(&g_cs_npclist);
	std::vector<MyNPC*>::iterator list_oper;
	for(list_oper=npc_list.begin();list_oper!=npc_list.end();++list_oper)
	{
		MyNPC *p=*list_oper;
		if(p) 	p->UIRender();



		//渲染cam和npc之间的3dline.调试
		//必须提供一个矩阵--------------------------------------------------------------------------------------------------------------------------
		/*
		core::matrix4 mat;
		video::SMaterial material;
		material.TextureLayer[0].Texture= 0;
		material.Lighting = false;
		material.Thickness=2; //设置粗细
		dev->getVideoDriver()->setTransform(video::ETS_WORLD, mat);
		dev->getVideoDriver()->setMaterial(material);

		getIrrDriver()->draw3DLine(p->line.start,p->line.end,video::SColor(255, 255, 250, 0));
		*/
		//-----------------------------------------------------------------------------------------------------------------------------------------	
		
		
		


		
	}
	::LeaveCriticalSection(&g_cs_npclist);
	//////---------------------------------------------------------------------------------------------///////////////	

}
