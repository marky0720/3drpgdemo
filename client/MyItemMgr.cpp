#include ".\myitemmgr.h"

MyItemMgr::MyItemMgr(MySceneManager* mysmgr, Socket::CNetClient* net)
{
	this->dev=getIrrDevice();
	this->net=net;

	::InitializeCriticalSection(&g_cs_SceneItem_list);//初始化临界区对象

	::InitializeCriticalSection(&g_cs_msglist);//初始化临界区对象

	SceneItem_list.clear();

	m_pMySceneMgr=mysmgr;
}

MyItemMgr::~MyItemMgr(void)
{
}

//处理服务器发来的物品消息
void  MyItemMgr::DoListThread( )
{
	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_msglist);

	//反复处理消息.直到处理完才返回..
	//	while (msglist.getSize()>0)
	{

		irr::core::list<ClientMsg*>::Iterator  msg_oper;
		for(msg_oper=msglist.begin();msg_oper!=msglist.end();++msg_oper)
		{

			unsigned char cmd=(*msg_oper)->pData[0];
			printf("MyCharacterMgr DoListThread cmd:%d\n",cmd);

			//物品添加
			if(cmd==SceneCMD_ITEMDROP){
				SceneCMD_ITEMDROP_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
				
			}else if(cmd==SceneCMD_ITEMDROPRemove){
				SceneCMD_ITEMDROPRemoveProc((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			
			}

		}
	}

	::LeaveCriticalSection(&g_cs_msglist);
	//////---------------------------------------------------------------------------------------------///////////////	


}


//是否所有场景地上物品是否可见
void MyItemMgr::setAllItemVisiable(bool v)
{
	::EnterCriticalSection(&g_cs_SceneItem_list);
	std::list<MyItem*>::iterator  list_oper;
	for(list_oper=SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
	{
		MyItem* t=(*list_oper);
		if(t->node) t->node->setVisible(v);
	}
	::LeaveCriticalSection(&g_cs_SceneItem_list);

}

////地图的掉落物品清空
void MyItemMgr::RemoveAllItem()
{
	::EnterCriticalSection(&g_cs_SceneItem_list);
	std::list<MyItem*>::iterator  list_oper;
	for(list_oper=SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
	{
		MyItem* t=(*list_oper);
		t->drop();
		delete t;
		t=0;
	}

	SceneItem_list.clear();

	::LeaveCriticalSection(&g_cs_SceneItem_list);
}

//得到所有场景的掉落物品
void MyItemMgr::getSceneCMD_ITEMDROPAll()
{
	printf("MySceneItemMgr 请求所有掉落物品\n");
	SceneItem item;
	item.cmd=SceneCMD_ITEMDROPAll;
	item.item_Type=0;
	strcpy(item.itemName,"a");
	item.pos=vector3df(0,0,0);
	net->SendMsg((char*)&item,sizeof(SceneItem));


}

//插入用户消息,来自服务器
void MyItemMgr::insertItemData(const char* pData,const unsigned int DataLength)
{
	printf("MySceneItemMgr insertItemData\n");

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


void MyItemMgr::SceneCMD_ITEMDROP_PROC(char* pData,unsigned long length)
{

	printf("SceneCMD_ITEMDROP \n");
	SceneItem item;
	memcpy((char*)&item,(char*)pData,sizeof(SceneItem));

	//先查找一遍是否已存在该物品
	bool already_exist=false;
	::EnterCriticalSection(&g_cs_SceneItem_list);
	std::list<MyItem*>::iterator  list_oper;
	for(list_oper=SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
	{
		MyItem* t=(*list_oper);
		if(t->item.item_Type==item.item_Type 
			&& t->item.param1==item.param1 
			&& t->item.param2==item.param2 
			&& t->item.param3==item.param3 
			&& t->item.param4==item.param4 
		    && t->item.param5==item.param5
			&& t->item.param6==item.param6 
			&& t->item.item_index==item.item_index
			&& t->item.pos==item.pos
			){
				already_exist=true;
				break;
			}
	}
	::LeaveCriticalSection(&g_cs_SceneItem_list);

	//不存在
	if( already_exist==false){

		printf("index:%d %3.1f %3.1f %3.1f HP:%d\n",(int)item.item_index,item.pos.X,item.pos.Y,item.pos.Z );

		//金币
		if(item.item_Type==ITEMType_Money)
		{
			////MessageBox(0,"有金币掉落!","",0);
			::EnterCriticalSection(&g_cs_SceneItem_list);
			//////---------------------------------------------------------------------------------------------///////////////
			//只加载物品类.初始化物品属性,不在该线程加载模型。因为irr线程不安全.只设置物品待加载模型状态.在update里加载. update在主线程里.是安全的
			MyItem* money=new MyItem("item/coin/coin.ms3d",item);
			money->Draw();
			SceneItem_list.push_back(money);
			::LeaveCriticalSection(&g_cs_SceneItem_list);
			//////---------------------------------------------------------------------------------------------///////////////
			printf("ITEM_Money \n");
		}else if(item.item_Type==ITEMType_Item){
			::EnterCriticalSection(&g_cs_SceneItem_list);
			//////---------------------------------------------------------------------------------------------///////////////
			//只加载物品类.初始化物品属性,不在该线程加载模型。因为irr线程不安全.只设置物品待加载模型状态.在update里加载. update在主线程里.是安全的
			MyItem* money=new MyItem("item/box/box.ms3d",item);
			money->Draw();
			SceneItem_list.push_back(money);
			::LeaveCriticalSection(&g_cs_SceneItem_list);
			printf("ITEMType_Item \n");
		}

	}else{
		//已存在该物品返回
		printf("id:%d的物品已存在或正在添加,不重复添加!\n",item);
		return;
	}


}

//物品消失
void MyItemMgr::SceneCMD_ITEMDROPRemoveProc(char* pData,unsigned long length)
{	
	printf("CMD_ITEM Remove \n");
	SceneItem item;
	memcpy((char*)&item,(char*)pData,sizeof(SceneItem));
	
//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_SceneItem_list);
		std::list<MyItem*>::iterator  list_oper;
		for(list_oper=SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
		{
			 MyItem* t=(*list_oper);
			 if(t->item.item_Type==item.item_Type 
				 //&& t->item.param1==item.param1 
				 //&& t->item.param2==item.param2 
				 //&& t->item.param3==item.param3 
				 //&& t->item.param4==item.param4 
			  //   && t->item.param5==item.param5
				 //&& t->item.param6==item.param6 
				 && t->item.item_index==item.item_index
				 && t->item.pos==item.pos
			   ){
				  
				   //只设置物品类的标志 待删除 
				   //t->ModelState=3;
				   t->drop();
				   delete t;
				   t=0;
				   SceneItem_list.erase(list_oper);
				   break;
			 }
			
		}
	
		if(SceneItem_list.size()>0){
			
		}

	::LeaveCriticalSection(&g_cs_SceneItem_list);
//////---------------------------------------------------------------------------------------------///////////////

	
}

void MyItemMgr::update()
{
	//先处理服务器传来的物品消息
	DoListThread();

	//return;

	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_SceneItem_list);
	std::list<MyItem*>::iterator  list_oper;
	for(list_oper=SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
	{
		MyItem* t=(*list_oper);
	
//-------------------------------------------------------------------------------------
		//得到物品坐标 是固定了的
		t->line.start=t->item.pos;
		t->line.start.Y+=10;
		//得到摄象机的坐标 
		t->line.end= m_pMySceneMgr->myCamera->getNode()->getPosition();
		//line.end.Y+=10;
		vector3df vec=m_pMySceneMgr->char_mgr->GetMainCharacter()->getPosition()-t->item.pos;
		vec.normalize();
		//	p->line.end-=vec*5;
		//	p->line.start-=vec*5;

		core::triangle3df triangle;
		const ISceneNode* outnode;
		//让npc到摄象机的3d线段和场景地形,建筑模型碰撞.如果有.就说明他们之间有遮挡.就把npc隐藏.
		if (getIrrSmgr()->getSceneCollisionManager()->getCollisionPoint(
			t->line, //[in] 射线
			m_pMySceneMgr->myscene_loader->getMeta(), //[in] 场景节点的三角形选择器
			t->line.end, //[out] 返回碰撞点
			triangle,outnode)) //[out] 返回碰撞的三角
		{
			//printf("itemmgr select id:%d\n",outnode->getID());

			t->node->setVisible(false);
		}else{
			t->node->setVisible(true);
		}
//------------------------------------------------------------------------------------------

	}

	::LeaveCriticalSection(&g_cs_SceneItem_list);
	//////---------------------------------------------------------------------------------------------///////////////

}


//显示物品名称
void MyItemMgr::UIRender()
{
	::EnterCriticalSection(&g_cs_SceneItem_list);
	//////---------------------------------------------------------------------------------------------///////////////
	std::list<MyItem*>::iterator  list_oper;
	for(list_oper=SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
	{
		MyItem* item=(*list_oper);
		item->UIRender();
	}
	::LeaveCriticalSection(&g_cs_SceneItem_list);
	//////---------------------------------------------------------------------------------------------///////////////
}
