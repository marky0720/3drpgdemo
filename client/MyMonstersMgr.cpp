#include "StdAfx.h"
#include ".\mymonstersmgr.h"

MyMonstersMgr::MyMonstersMgr( MySceneManager* mysmgr,Socket::CNetClient* net)
{
	//引擎对象
	device=getIrrDevice();						//引擎指针
	smgr=getIrrSmgr();		//场景管理

	this->m_pMySceneMgr=mysmgr;

	//网络
	this->net=net;

	//被选择的怪物
	selectednode=0;
	Nodeselected=false;


	 //定时器
	 timer=device->getTimer();
	 lasttime=timer->getRealTime();


	 ::InitializeCriticalSection(&g_cs_msglist);//初始化临界区对象

	 
	::InitializeCriticalSection(&g_cs_monsterlist);//初始化临界区对象

	//消息清空
	msglist.clear();

	//场景怪物列表清空
	monster_list.clear();


}

MyMonstersMgr::~MyMonstersMgr(void)
{

}

//移出所有在线怪物.用于切换场景时的.
void MyMonstersMgr::RemoveAllMonsters()
{
	::EnterCriticalSection(&g_cs_monsterlist);
	for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
	{
		
		MyMonster *p=*list_oper;
		p->drop();
		delete p;
		p=0;
	}
	monster_list.clear();

	::LeaveCriticalSection(&g_cs_monsterlist);

}

MyMonster* MyMonstersMgr::GetMonsterByID(int id)
{
	::EnterCriticalSection(&g_cs_monsterlist);
		for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
		{
				
			    MyMonster *p=*list_oper;
				//如果死亡就移除链表
				
				if(p->GetMonsterId()==id){
					::LeaveCriticalSection(&g_cs_monsterlist);
					return p;
				}
		}
	::LeaveCriticalSection(&g_cs_monsterlist);
		return 0;
}



//设置所有怪物是否可见
void MyMonstersMgr::setAllMonsterVisiable(bool v)
{
	::EnterCriticalSection(&g_cs_monsterlist);
	for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
	{
		 MyMonster *p=*list_oper;
		 if(p->GetNode())p->GetNode()->setVisible(v);
	}

	::LeaveCriticalSection(&g_cs_monsterlist);
}

/*

	添加一个怪物到场景中
	
*/
void MyMonstersMgr::AddMonster(MonsterProperty prop)
{
	
::EnterCriticalSection(&g_cs_monsterlist);
	MyMonster* beast=new MyMonster(prop,"zombie//zombie.b3d");
	monster_list.push_back(beast); //添加进链表
	::LeaveCriticalSection(&g_cs_monsterlist);
	beast->Draw(); //加载怪物模型信息
	beast->SetNetServer(this->net);
	beast->SetInitRotValue(90);
	
//	beast->GetNode()->setDebugDataVisible(scene::EDS_BBOX ); //设置显示包围盒

	printf("怪物%d完全信息添加完毕!\n",prop.ID);

}



/*
  更新所有怪物
 */
void MyMonstersMgr::Update()
{

	//先处理服务器传来的怪物消息
	DoListThread();


//////---------------------------------------------------------------------------------------------///////////////	
		::EnterCriticalSection(&g_cs_monsterlist);
		for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
		{
		

			    MyMonster *p=*list_oper;
				////如果死亡就移除链表
				if(p->IsDead() || p->getMonsterProperty()->HP<=0){
					
					if(p->isdrop==false)
					{
						if(p->GetNode() ) p->GetNode()->setVisible(false);
						p->AddGhostFlyEffect();
					}

					
					
					////防止这里移除了对象,角色类里的monster引用错了.所以先判断.然后设置个空给他main_char好判空
					//if(m_pMySceneMgr->char_mgr->GetMainCharacter()->monster!=0){
					//	if(m_pMySceneMgr->char_mgr->GetMainCharacter()->monster->GetMonsterId()==p->GetMonsterId()){
					//		m_pMySceneMgr->char_mgr->GetMainCharacter()->SetWantToAttackTarget(0);
					//		m_pMySceneMgr->char_mgr->GetMainCharacter()->readyAttack(true);
					//		m_pMySceneMgr->char_mgr->GetMainCharacter()->ReoprtToServer();
					//		//	//MessageBox(0,"AddGhostFlyEffect怪物挂了设置0","",0);
					//	}
					//}
					
					p->drop();
					delete p;
					p=0;
					monster_list.erase(list_oper);
					break;
				}
				
				//调用怪物自己的更新方法
				if(p) p->Update();

		//-------------------------------------------------------------------------------------
				//得到怪物pos
				p->line.start=p->getMonsterProperty()->pos;
				p->line.start.Y+=10;
				//得到摄象机的坐标 
				p->line.end = m_pMySceneMgr->myCamera->getNode()->getPosition();
				//line.end.Y+=10;
				vector3df vec=m_pMySceneMgr->char_mgr->GetMainCharacter()->getPosition()-p->getMonsterProperty()->pos;
				vec.normalize();
				//	p->line.end-=vec*5;
				//	p->line.start-=vec*5;

				core::triangle3df triangle;
				const ISceneNode* outnode;
				//让npc到摄象机的3d线段和场景地形,建筑模型碰撞.如果有.就说明他们之间有遮挡.就把npc隐藏.
				if (getIrrSmgr()->getSceneCollisionManager()->getCollisionPoint(
					p->line, //[in] 射线
					m_pMySceneMgr->myscene_loader->getMeta(), //[in] 场景节点的三角形选择器
					p->line.end, //[out] 返回碰撞点
					triangle,outnode)) //[out] 返回碰撞的三角
				{
					//printf("monstermgr select id:%d\n",outnode->getID());

					if(p && p->GetNode())
						p->GetNode()->setVisible(false);
					
				}else{
					if(p && p->GetNode())
						p->GetNode()->setVisible(true);
				}
				//------------------------------------------------------------------------------------------

		}
	   ::LeaveCriticalSection(&g_cs_monsterlist);
//////---------------------------------------------------------------------------------------------///////////////	
			scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();

	    //用时
		u32 usetime=timer->getRealTime()-lasttime;
		if( usetime<50) return;

			lasttime=timer->getRealTime();

			//if(selectednode) selectednode->GetNode()->setDebugDataVisible(scene::EDS_OFF ); //先off包围盒
			
			//得到屏幕鼠标转换到3d场景的一条射线
			vector2d<s32> curpos=device->getCursorControl()->getPosition();
			core::line3d<f32> line,line2;
			line=smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(curpos);
			curpos.Y-=8;
			line2=smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(curpos);

			
			//得到被选取的节点方法1.
			//ISceneNode* m=smgr->getSceneCollisionManager()->getSceneNodeFromRayBB(line);
			
			//ISceneNode *m=0;
			//for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
			//{
			//		
			//		MyMonster *p=*list_oper;
			//		
			//		aabbox3d<f32> box=p->GetNode()->getBoundingBox();
			//		
			//	//	box.MaxEdge+=vector3df(5,5,5);
			//	//	box.MinEdge+=vector3df(-5,-5,-5);

			//		if(box.intersectsWithLine(line)){
			//			m=p->GetNode();
			//		   break;
			//		}else{
			//		  
			//		}
			//}
				
			//方法2.
			core::vector3df intersection;
			core::triangle3df hitTriangle;
			ISceneNode * m =collMan->getSceneNodeAndCollisionPointFromRay(
					line,
					intersection, // This will be the position of the collision
					hitTriangle, // This will be the triangle hit in the collision
					0, // This ensures that only nodes that we have
							// set up to be pickable are considered
					0); // Check the entire scene (this is actually the implicit default)

			//拾取了怪物
			 if(m){
				 	//根据节点得到怪物类
					selectednode=GetMonster(m);
					//如果怪物已死,就不显示可攻击
					if( selectednode==NULL || selectednode->IsDead()){
							Nodeselected=false;
						//	printf("一次 false\n");
					}else{
							//node->setDebugDataVisible(scene::EDS_BBOX ); //设置显示包围盒
							//printf("一次 true\n");
							Nodeselected=true;
							
							
							//计算被选择怪物的坐标,并保存,用于ui显示text怪物名
							vector3df userpos=selectednode->GetNode()->getPosition();    //selectednode->getHPBar()->getAbsolutePosition();
							MonsterTextShowPos= smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(userpos);
							

							return;
					}
			 }else{
				 Nodeselected=false;
				// printf("一次 false\n");
			    
			 }


			 m =collMan->getSceneNodeAndCollisionPointFromRay(
					line2,
					intersection, // This will be the position of the collision
					hitTriangle, // This will be the triangle hit in the collision
					0, // This ensures that only nodes that we have
							// set up to be pickable are considered
					0); // Check the entire scene (this is actually the implicit default)

			//拾取了怪物
			 if(m){
				 	//根据节点得到怪物类
					selectednode=GetMonster(m);
					//如果怪物已死,就不显示可攻击
					if( selectednode==NULL || selectednode->IsDead()){
							Nodeselected=false;
						//	printf("二次得到怪物类 false\n");
					}else{
							//node->setDebugDataVisible(scene::EDS_BBOX ); //设置显示包围盒
						//	printf("二次 true\n");
							Nodeselected=true;
							
							
							//计算被选择怪物的坐标,并保存,用于ui显示text怪物名
							vector3df userpos=selectednode->GetNode()->getPosition();    //selectednode->getHPBar()->getAbsolutePosition();
							MonsterTextShowPos= smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(userpos);
							
							return;
					}
			 }else{
				 Nodeselected=false;
				// printf("二次 false\n");
			    
			 }

	

}


//所有怪物的ui显示.比如血条 名称
void MyMonstersMgr::UIRender()
{
//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_monsterlist);
		for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
		{
				
			    MyMonster *p=*list_oper;
				if(p) p->UIRender();
		}

//////---------------------------------------------------------------------------------------------///////////////	
	::LeaveCriticalSection(&g_cs_monsterlist);


	//显示被鼠标选择了的怪物名称
	if(Nodeselected){
		if(selectednode && selectednode->IsDead()==false) TextCenter(selectednode->getMonsterProperty()->Name,MonsterTextShowPos,SColor(1,255,0,0),14,600);
	}

}

void MyMonstersMgr::OnEvent (const irr::SEvent& e)
{ 
 
}



// 根据节点得到怪物类
MyMonster* MyMonstersMgr::GetMonster(ISceneNode* node)
{
//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_monsterlist);
		for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
		{
			     MyMonster *p=*list_oper;
				 if(p->GetNode()==node)
				{
						::LeaveCriticalSection(&g_cs_monsterlist);
					return p;
				}
		}
//////---------------------------------------------------------------------------------------------///////////////	
		::LeaveCriticalSection(&g_cs_monsterlist);
return 0;
}




//先把服务器发来的怪物消息处理
void   MyMonstersMgr::DoListThread( )
{


	::EnterCriticalSection(&g_cs_monsterlist);
	//反复处理消息.直到处理完才返回..
 //   while (msglist.getSize()>0)
	{
	
		core::list<ClientMsg*>::Iterator  msg_oper;
		for(msg_oper=msglist.begin();msg_oper!=msglist.end();++msg_oper)
		{
			ClientMsg* msg=(*msg_oper);
			unsigned char cmd=msg->pData[0];

			printf("MyMonstersMgr DoListThread cmd:%d\n",cmd);
			if(cmd==MonsterCMD_LOGIN){

				MonsterCMD_LOGIN_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete msg->pData;
				delete msg;
				msg_oper=msglist.erase(msg_oper);
				break;
			}else if(cmd==MonsterCMD_UPDATE){
				MonsterCMD_UPDATE_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete msg->pData;
				delete msg;
				msg_oper=msglist.erase(msg_oper);
				break;
			}else if(cmd==MonsterCMD_UnderAttack){
				MonsterCMD_UnderAttack_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete msg->pData;
				delete msg;
				msg_oper=msglist.erase(msg_oper);
				break;
			}

		}

	}

	::LeaveCriticalSection(&g_cs_monsterlist);
	
	
}

//向服务器请求所有的怪物信息
void MyMonstersMgr::getMonsterCMD_MonsterAll()
{
    MonsterProperty p;
	memset((char*)&p,'\0',sizeof(MonsterProperty));
	p.cmd=MonsterCMD_MonsterAll;
	net->SendMsg((char*)&p,4); //偷懒4位固定大小吧.反正没什么重要数据
}


void MyMonstersMgr::MonsterCMD_UnderAttack_PROC(char* pData,unsigned long length)
{
	if(length<sizeof(MonsterProperty)){

	}

	//怪物被攻击,只接收被攻击的伤害值和当前hp.
	//标识是否找到指定id的怪物
	bool found=false;
	MonsterProperty prop;
	memset((char*)&prop,'\0',sizeof(MonsterProperty));
	memcpy((char*)&prop,(char*)pData,sizeof(MonsterProperty));
	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_monsterlist);
	for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
	{
		MyMonster *p=*list_oper;
		if(p){
			if(p->GetMonsterId()==prop.ID)
			{
				//memcpy((char*)&p->prop,(char*)pData,sizeof(MonsterProperty)); 
				//只需更新hp和伤害.
				p->setMonsterHP(prop.HP);
				p->setMonsterDamage(prop.damage);
				//并显示伤害数字
				p->ShowAttackTextNum(prop.damage);

				found=true;
				break;
			}
		}

		
	}
	//////---------------------------------------------------------------------------------------------///////////////	
	::LeaveCriticalSection(&g_cs_monsterlist);
	//如果有未知怪物,就请求服务器信息
	if(found==false)
	{
		printf("有未知怪物id:%d,就请求服务器信息\n",prop.ID);
		prop.cmd=MonsterCMD_UNKNOWMONSTER;
		net->SendMsg((char*)&prop,sizeof(MonsterProperty));
	}

}


void MyMonstersMgr::MonsterCMD_UPDATE_PROC(char* pData,unsigned long length)
{
	printf("MonsterCMD_UPDATE\n");
	//标识是否找到指定id的怪物
	bool found=false;
	MonsterProperty prop;
	memset((char*)&prop,'\0',sizeof(MonsterProperty));
	memcpy((char*)&prop,(char*)pData,sizeof(MonsterProperty));
	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_monsterlist);
	for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
	{
		MyMonster *p=*list_oper;
		if(p->GetMonsterId()==prop.ID){
			printf("id:%d  %3.1f %3.1f %3.1f\n",prop.ID,prop.pos.X,prop.pos.Y,prop.pos.Z);
			p->setMonsterProperty(prop);
			found=true;
			break;
		}
	}
	//////---------------------------------------------------------------------------------------------///////////////	
	::LeaveCriticalSection(&g_cs_monsterlist);
	//如果有未知怪物,就请求服务器信息
	if(found==false ){
		printf("有未知怪物id:%d,就请求服务器信息\n",prop.ID);
		prop.cmd=MonsterCMD_UNKNOWMONSTER;
		net->SendMsg((char*)&prop,sizeof(MonsterProperty));
	}
	

}

void MyMonstersMgr::MonsterCMD_LOGIN_PROC(char* pData,unsigned long length)
{

	printf("MonsterCMD_LOGIN\n");
	MonsterProperty prop;
	memset((char*)&prop,'\0',sizeof(MonsterProperty));
	memcpy((char*)&prop,(char*)pData,sizeof(MonsterProperty));


	//表示列表中不存在该id的怪物.
	if(  GetMonsterByID(prop.ID) ==NULL){

		
		printf("id:%d的怪物不存在,添加!\n",prop.ID);

		//	//MessageBox(0,"的怪物不存在,添加!","",0);
		printf("id:%d %3.1f %3.1f %3.1f HP:%d\n",prop.ID,prop.pos.X,prop.pos.Y,prop.pos.Z,prop.HP);

		AddMonster(prop);
	
	}	else{
		printf("id:%d的怪物已存在或正在添加,不重复添加!\n",prop.ID);
	}


}


//怪物之接收服务器信息,并不发送
void MyMonstersMgr::insertMonsterData(const char* pData,const unsigned int DataLength)
{
	ClientMsg* m=new ClientMsg();
	char* tmp=new char[DataLength];
	memcpy(tmp,pData,DataLength);
	m->pData=tmp;
	m->size=DataLength;

	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_msglist);
	msglist.push_back(m); //添加进消息链表
	::LeaveCriticalSection(&g_cs_msglist);
	//////---------------------------------------------------------------------------------------------///////////////	




}

