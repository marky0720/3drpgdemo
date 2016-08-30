#include "StdAfx.h"
#include ".\myitemmgr.h"

MyItemMgr::MyItemMgr(Socket::MySOCKServer* serve,MyRoleMgr* m_pRoleMgr)
{
	::InitializeCriticalSection(&g_cs_itemlist);//初始化临界区对象

	server=serve;
	this->m_pRoleMgr=m_pRoleMgr;

	

	SceneItem* item=new SceneItem();
	item->cmd=SceneCMD_ITEMDROP;
	item->item_Type=ITEMType_Item;
	item->item_index=BLD_Weapon; //掉落什么物品
	item->param1=5; //武器参数1  
	sprintf(item->itemName,"[%s]+%d",getItembyID(BLD_Weapon)->itemName,item->param1);
	item->param2=0;
	item->param3=0;
	item->param4=0;
	item->param5=0;
	item->param6=0;

	item->worldMap_id=1;	//物品掉落在哪个世界地图
	item->pos=vector3df(0,0,0);

	addItem(item);

}

MyItemMgr::~MyItemMgr(void)
{
}

//list加锁
void MyItemMgr::ItemListLock()
{
	::EnterCriticalSection(&g_cs_itemlist);
}

//list解锁
void MyItemMgr::ItemListUnLock()
{
	::LeaveCriticalSection(&g_cs_itemlist);
}


//得到所有掉落物品数量
int MyItemMgr::getAllItemNum(int worldMap)
{
	int size=0;
	ItemListLock();
	std::vector<SceneItem*>::iterator  list_oper;
	for(list_oper= SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
	{
		SceneItem* item=(*list_oper);
		if(item->worldMap_id==worldMap){
			size++;
		}
	}
	ItemListUnLock();

	return size;
}

//向指定用户发送所有掉落物品数量
void MyItemMgr::SendAllSceneItemNum(SOCKET sock  ,int worldMap)
{

	SceneItemNum item;
	item.cmd=SceneCMD_ITEMDROPAllNum;
	item.itemnum=getAllItemNum(worldMap);
	server->SendMsg(sock,(char*)&item,sizeof(SceneItem));

}

// 场景添加掉落物品
void MyItemMgr::addItem(SceneItem* item)
{
  ItemListLock();
	SceneItem_list.push_back(item);
  ItemListUnLock();
}

//向指定用户发送某世界的所有掉落物品
void MyItemMgr::SendAllSceneItem(SOCKET sock ,int worldMap)
{

  ItemListLock();

	std::vector<SceneItem*>::iterator  list_oper;
	for(list_oper= SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
	{
		SceneItem* item=(*list_oper);

		//在同一个世界
		if(item->worldMap_id==worldMap){
			server->SendMsg(sock,(char*)item,sizeof(SceneItem));	
		}
		
	}

  ItemListUnLock();

}



//物品被角色捡了?
void MyItemMgr::ItemCollect(RoleProperty* role,Package* pack)
{
	SOCKET sock=role->sock;
	vector3df pos=role->pos;
	int money=0;

ItemListLock();

	std::vector<SceneItem*>::iterator  list_oper;
	for(list_oper= SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
	{
		SceneItem* t=(*list_oper);

		//距离近而且在同一个世界
		if(t->pos.getDistanceFrom(pos)<10 && role->worldMap_id==t->worldMap_id){

			if(t->item_Type==ITEMType_Money){
				money=t->param1;//金币的话,param1参数就是金钱数量
				//物品消失
				t->cmd=SceneCMD_ITEMDROPRemove;//消失

				m_pRoleMgr->RoleGainMoney(sock,money);
				
			}else if(t->item_Type==ITEMType_Item){
				t->cmd=SceneCMD_ITEMDROPRemove;//消失

				//int attack=getItemAttackById(t->item_index); //得到物品的攻击力
				m_pRoleMgr->RoleGainItem(sock,t->item_index,t->param1,t->param2,t->param3,t->param4,t->param5,t->param6);
			}
			
			//发送物品消失了,在同一个世界里的角色才能接收
			server->SendMsgToAllInWorld((char*)t,sizeof(SceneItem),t->worldMap_id);

			delete t;
			t=NULL;

			SceneItem_list.erase(list_oper);
			break;
		}

	}

ItemListUnLock();

	
}

//随机物品掉落物品生成
void MyItemMgr::RandomItemDrop(int birthworldMap,vector3df pos,int ItemDroprate)
{

	//发送给所有在线角色,物品掉落
	SceneItem* item=new SceneItem();
	item->cmd=SceneCMD_ITEMDROP;


	int ret=randomNum(1,100);

	//在物品暴率范围内,就掉物品
	if(ret<=ItemDroprate){
		item->item_Type=ITEMType_Item;
		item->item_index=randomNum(1,14); //掉落什么物品
		item->param1=randomNum(0,5); //武器参数1  
		sprintf(item->itemName,"[%s]+%d",getItembyID(item->item_index)->itemName,item->param1);
		item->param2=0;
		item->param3=0;
		item->param4=0;
		item->param5=0;
		item->param6=0;
	}else{
		item->item_Type=ITEMType_Money;
		item->item_index=0;
		item->param1=200; //200金钱
		sprintf(item->itemName,"%d",item->param1);
		item->param2=0;
		item->param3=0;
		item->param4=0;
		item->param5=0;
		item->param6=0;
	}


	
	item->worldMap_id=birthworldMap;//物品掉落在哪个世界地图
	item->pos=pos; //坐标

	addItem(item);
	
	server->SendMsgToAllInWorld((char*)item,sizeof(SceneItem),birthworldMap);

	printf("有物品掉落\n");
}
