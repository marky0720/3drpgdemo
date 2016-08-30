#include "StdAfx.h"
#include ".\myitemmgr.h"

MyItemMgr::MyItemMgr(Socket::MySOCKServer* serve,MyRoleMgr* m_pRoleMgr)
{
	::InitializeCriticalSection(&g_cs_itemlist);//��ʼ���ٽ�������

	server=serve;
	this->m_pRoleMgr=m_pRoleMgr;

	

	SceneItem* item=new SceneItem();
	item->cmd=SceneCMD_ITEMDROP;
	item->item_Type=ITEMType_Item;
	item->item_index=BLD_Weapon; //����ʲô��Ʒ
	item->param1=5; //��������1  
	sprintf(item->itemName,"[%s]+%d",getItembyID(BLD_Weapon)->itemName,item->param1);
	item->param2=0;
	item->param3=0;
	item->param4=0;
	item->param5=0;
	item->param6=0;

	item->worldMap_id=1;	//��Ʒ�������ĸ������ͼ
	item->pos=vector3df(0,0,0);

	addItem(item);

}

MyItemMgr::~MyItemMgr(void)
{
}

//list����
void MyItemMgr::ItemListLock()
{
	::EnterCriticalSection(&g_cs_itemlist);
}

//list����
void MyItemMgr::ItemListUnLock()
{
	::LeaveCriticalSection(&g_cs_itemlist);
}


//�õ����е�����Ʒ����
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

//��ָ���û��������е�����Ʒ����
void MyItemMgr::SendAllSceneItemNum(SOCKET sock  ,int worldMap)
{

	SceneItemNum item;
	item.cmd=SceneCMD_ITEMDROPAllNum;
	item.itemnum=getAllItemNum(worldMap);
	server->SendMsg(sock,(char*)&item,sizeof(SceneItem));

}

// ������ӵ�����Ʒ
void MyItemMgr::addItem(SceneItem* item)
{
  ItemListLock();
	SceneItem_list.push_back(item);
  ItemListUnLock();
}

//��ָ���û�����ĳ��������е�����Ʒ
void MyItemMgr::SendAllSceneItem(SOCKET sock ,int worldMap)
{

  ItemListLock();

	std::vector<SceneItem*>::iterator  list_oper;
	for(list_oper= SceneItem_list.begin();list_oper!=SceneItem_list.end();++list_oper)
	{
		SceneItem* item=(*list_oper);

		//��ͬһ������
		if(item->worldMap_id==worldMap){
			server->SendMsg(sock,(char*)item,sizeof(SceneItem));	
		}
		
	}

  ItemListUnLock();

}



//��Ʒ����ɫ����?
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

		//�����������ͬһ������
		if(t->pos.getDistanceFrom(pos)<10 && role->worldMap_id==t->worldMap_id){

			if(t->item_Type==ITEMType_Money){
				money=t->param1;//��ҵĻ�,param1�������ǽ�Ǯ����
				//��Ʒ��ʧ
				t->cmd=SceneCMD_ITEMDROPRemove;//��ʧ

				m_pRoleMgr->RoleGainMoney(sock,money);
				
			}else if(t->item_Type==ITEMType_Item){
				t->cmd=SceneCMD_ITEMDROPRemove;//��ʧ

				//int attack=getItemAttackById(t->item_index); //�õ���Ʒ�Ĺ�����
				m_pRoleMgr->RoleGainItem(sock,t->item_index,t->param1,t->param2,t->param3,t->param4,t->param5,t->param6);
			}
			
			//������Ʒ��ʧ��,��ͬһ��������Ľ�ɫ���ܽ���
			server->SendMsgToAllInWorld((char*)t,sizeof(SceneItem),t->worldMap_id);

			delete t;
			t=NULL;

			SceneItem_list.erase(list_oper);
			break;
		}

	}

ItemListUnLock();

	
}

//�����Ʒ������Ʒ����
void MyItemMgr::RandomItemDrop(int birthworldMap,vector3df pos,int ItemDroprate)
{

	//���͸��������߽�ɫ,��Ʒ����
	SceneItem* item=new SceneItem();
	item->cmd=SceneCMD_ITEMDROP;


	int ret=randomNum(1,100);

	//����Ʒ���ʷ�Χ��,�͵���Ʒ
	if(ret<=ItemDroprate){
		item->item_Type=ITEMType_Item;
		item->item_index=randomNum(1,14); //����ʲô��Ʒ
		item->param1=randomNum(0,5); //��������1  
		sprintf(item->itemName,"[%s]+%d",getItembyID(item->item_index)->itemName,item->param1);
		item->param2=0;
		item->param3=0;
		item->param4=0;
		item->param5=0;
		item->param6=0;
	}else{
		item->item_Type=ITEMType_Money;
		item->item_index=0;
		item->param1=200; //200��Ǯ
		sprintf(item->itemName,"%d",item->param1);
		item->param2=0;
		item->param3=0;
		item->param4=0;
		item->param5=0;
		item->param6=0;
	}


	
	item->worldMap_id=birthworldMap;//��Ʒ�������ĸ������ͼ
	item->pos=pos; //����

	addItem(item);
	
	server->SendMsgToAllInWorld((char*)item,sizeof(SceneItem),birthworldMap);

	printf("����Ʒ����\n");
}
