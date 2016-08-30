#include "StdAfx.h"
#include ".\mymonstersmgr.h"

MyMonstersMgr::MyMonstersMgr(MyRoleMgr* role_mgr,	Socket::MySOCKServer* ser, MyItemMgr* m_pItem_mgr)
{
	::InitializeCriticalSection(&g_cs_monsterlist);//初始化临界区对象

	server=ser;

	this->role_mgr=role_mgr;

	this->m_pItem_mgr=m_pItem_mgr;


	count=1;

//读取所有怪物基本配置信息列表
	ReadAllMonstersInfo();

//加载场景地图刷怪文件
    ReadSceneMonster();

}


void MyMonstersMgr::ReadSceneMonster()
{
	char File[255]="data//scene_monster.txt";
	FILE* log;
	log = fopen(File, "r");
	//文件不存在.返回
	if (log == NULL)
		return ;
	char strtemp[255]="";

	printf("====加载怪物到场景开始!====\n");

	int rownum=GetTxtRowNum(File);
	for(int i=1;i<=rownum;i++){
		//每行读取
		GetTxtByRowNum(File,strtemp,i);
		if(strtemp[0]=='/' && strtemp[1]=='/'){
			//printf("%d行是注释\n",i-1);
			continue; //该行是注释.继续下一个
		}else{

			//即第一个是数字.表示是个物品信息
			if(strtemp[0]>=48 && strtemp[0]<=57 ){
				
				int sceneID=0;
				int monsterID=0;
				int monsterType=0;
				vector3df pos;
				sscanf(strtemp, "%d  %d  %d  %f,%f,%f",  
					&sceneID, ///SceneID(场景编号)
					&monsterID,   //ID(怪物ID在场景里的唯一标识)
					&monsterType,   // Type(怪物类型)   
					&pos.X,&pos.Y,&pos.Z // 刷怪坐标
					); 

				AddMonster(monsterType,monsterID,pos,sceneID);

				printf("场景:%d 怪id:%d 怪类型:%d 坐标:%3.1f,%3.1f,%3.1f\n",  
					sceneID, ///SceneID(场景编号)
					monsterID,   //ID(怪物ID在场景里的唯一标识)
					monsterType,   // Type(怪物类型)   
					pos.X,pos.Y,pos.Z // 刷怪坐标
					);

			}
		}
	}

	printf("====加载怪物到场景完毕!====\n");

}

//得到怪物基本配置信息
SMonsterInfo* MyMonstersMgr::getMonsterBaseInfoByTypeID(int type)
{
	SMonsterInfo *ppp=0;
	std::list<SMonsterInfo*>::iterator  list_oper;
	//monsters_info_list是稳定的数据list.只在程序初始化时添加,所以不用加锁
	for(list_oper=monsters_info_list.begin();list_oper!=monsters_info_list.end();++list_oper)
	{

		SMonsterInfo *p=*list_oper;

		if(p->type==type){
		    ppp=p;
			break;
		}
	}
	return ppp;
}

//读取所有怪物基本配置信息
void MyMonstersMgr::ReadAllMonstersInfo()
{
  
	   char File[255]="data//monsters.txt";
	   FILE* log;
	   log = fopen(File, "r");
	   //文件不存在.返回
	   if (log == NULL)
		   return ;
	   char strtemp[255]="";

   printf("===全局怪物配置读取开始===\n");

	   int rownum=GetTxtRowNum(File);
	   for(int i=1;i<=rownum;i++){
		   //每行读取
		   GetTxtByRowNum(File,strtemp,i);
		   if(strtemp[0]=='/' && strtemp[1]=='/'){
			   //printf("%d行是注释\n",i-1);
			   continue; //该行是注释.继续下一个
		   }else{

			   //即第一个是数字.表示是个物品信息
			   if(strtemp[0]>=48 && strtemp[0]<=57 ){
				   SMonsterInfo* m=new SMonsterInfo;

				   sscanf(strtemp, "%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d", 
					   &m->type,&m->Name, //怪物类型 名字
					   &m->Level,&m->HP,  //等级 最大血
					   &m->Min_Damage,&m->Max_Damage, //最小/大 伤害
					   &m->Defense,&m->MaxMove_distense, //防御力  //怪物在刷怪点移动范围最大半径
					   &m->Attack_distense,&m->View_distense,&m->MoveSpeed, //攻击距离, 怪物视野范围 移动速度
					   &m->ReBornTime,&m->ItemDropPer, //重生时间 物品暴率
					   &m->MoneyDropPer,&m->MaxItemDropLevel //金钱暴率 最大物品掉落等级
					   ); 

				   printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d\n",  
					   m->type,m->Name, //怪物类型 名字
					   m->Level,m->HP,  //等级 最大血
					   m->Min_Damage,m->Max_Damage, //最小/大 伤害
					   m->Defense,m->MaxMove_distense, //防御力  //怪物在刷怪点移动范围最大半径
					   m->Attack_distense,m->View_distense,m->MoveSpeed, //攻击距离, 怪物视野范围 移动速度
					   m->ReBornTime,m->ItemDropPer, //重生时间 物品暴率
					   m->MoneyDropPer,m->MaxItemDropLevel //金钱暴率 最大物品掉落等级
					  );

				   monsters_info_list.push_back(m);
			   }
		   }
	   }

	   printf("===全局怪物配置读取完毕!共有%d种怪物===\n",monsters_info_list.size());

   
}

MyMonstersMgr::~MyMonstersMgr(void)
{

}

//怪物list加锁
void MyMonstersMgr::MonsterListLock()
{
	::EnterCriticalSection(&g_cs_monsterlist);
}

//怪物list解锁
void MyMonstersMgr::MonsterListUnLock()
{
	::LeaveCriticalSection(&g_cs_monsterlist);
}

//得到怪物所在的世界地图id
int MyMonstersMgr::getMonsterWorldMap(int monsterID)
{
	int world=-1;
	
		MyMonster *p=GetMonsterByID(monsterID);

		if(p){
			world=p->getMonsterProperty().worldMap_id;
			
		}

	return world;

}

//得到指定怪物
MyMonster* MyMonstersMgr::GetMonsterByID(int id)
{
	MyMonster *p=0;

	MonsterListLock();
	std::vector<MyMonster*>::iterator  list_oper;
	for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
	{
		p=*list_oper;
		if(p && p->GetId()==id){
			break;
		}else{
			p=NULL;
		}
	}
	MonsterListUnLock();

	return p;
}





//怪物遍历所有角色,看是否在视线内.在就返回角色指针
RoleProperty* MyMonstersMgr::GetNearestRole(MyMonster* m)
{
	
	RoleProperty* p=role_mgr->GetNearestRole(m->getMonsterProperty().pos,//怪物的坐标
		m->m_info.View_distense, //怪物的视线
		m->getMonsterProperty().worldMap_id); //怪物视线内可发现玩家

	return p;

};

//得到所有怪物数量
int MyMonstersMgr::getAllMonsterNum()
{
	int size=0;
	  MonsterListLock();
	size=monster_list.size();
	  MonsterListUnLock();
	return size;
}

/*

添加一个怪物到场景中
TypeID是怪物类别编号
ID是怪物场景节点编号即nodeid
pos是怪物初始化中心地点
worldmap是怪物初始化在哪个地图

*/
void MyMonstersMgr::AddMonster(int TypeID,int ID,vector3df pos,int worldmap)
{
	//得到该类型的怪物基本配置
	SMonsterInfo* sm=getMonsterBaseInfoByTypeID(TypeID);

	//设置怪物属性
	MonsterProperty p;
	p.Attack=sm->Max_Damage;
	p.Defense=sm->Defense;
	if(strlen(sm->Name)<20){
		strcpy(p.Name,sm->Name); //怪物名称
	}else{
		MessageBox(0,"有怪物名称长度超过!请检查!","",0);
	}

	p.MAX_HP=sm->HP;
	p.HP=p.MAX_HP;
	p.ID=ID;
	p.isDeath=false;
	p.currState=cWalk;
	p.killedByUser=0;
	p.pos=pos;
	p.target=vector3df(0,0,0);
	p.rot=vector3df(0,0,0);
	p.worldMap_id=worldmap;
	
	MyMonster* beast=new MyMonster(p,sm,server,worldmap);
	beast->SetInitRotValue(90);
	beast->SetCharacter(0);
	beast->SetId(ID);
	
	//	beast->GetNode()->setDebugDataVisible(scene::EDS_BBOX ); //设置显示包围盒
		MonsterListLock();
	monster_list.push_back(beast); //添加进链表
	    MonsterListUnLock();
	printf("AddMonster id:%d \n",beast->GetId());

}



//根据怪物id让怪物被攻击,attack是角色攻击力
MyMonster* MyMonstersMgr::MyMonsterUnderAttack(int MonserID,int attack,SOCKET attacker)
{
	
		MyMonster* p=GetMonsterByID(MonserID);
		if(p){
			//找到指定怪物,并让他被攻击.
			p->UnderAttack(attack,attacker);
			
		}

	return p;

}



//发送所有怪物数据给客户端
void MyMonstersMgr::SendAllMonsterListToClient(SOCKET sock , int worldMap)
{

	printf("发送所有怪物数据给客户端 sock:%d 怪总数:%d \n",sock, monster_list.size());

	MonsterListLock();
	std::vector<MyMonster*>::iterator  list_oper;

	for(list_oper=  monster_list.begin();list_oper!= monster_list.end();++list_oper)
	{
		MyMonster* p=(*list_oper);

		//怪物在指定的场景地图内
		if(worldMap==p->getMonsterProperty().worldMap_id){
			//将用户数据一个一个都发给sock
			if(p->IsDead() || p->getMonsterProperty().HP<=0 ){
				continue;
			}
			p->setMonsterCMD(MonsterCMD_LOGIN);
			bool f=server->SendMsg (sock,(char*)&(p->getMonsterProperty()),sizeof(MonsterProperty));
			if(f) printf("将怪物%d数据都发给客户:%d ok\n",p->getMonsterProperty().ID,sock);
			else
				printf("将怪物数%d据都发给客户:%d 失败\n",p->getMonsterProperty().ID,sock);

			char ss[255];
			sprintf(ss,"将怪物id:%d的数据发送",p->getMonsterProperty().ID);
			WriteToLog(ss);
		}
	
	}


	MonsterListUnLock();
}



/*
更新所有怪物
*/
void MyMonstersMgr::Update()
{


	MonsterListLock();
	std::vector<MyMonster*>::iterator  list_oper;

	for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
	{

		MyMonster* p=(*list_oper);

		//得到第一个在怪物视线内的活角色
		RoleProperty* rp=GetNearestRole(p);

		if(rp!=0){
			//有在视线内的
			p->SetCharacter(rp);
		}else{
			p->SetCharacter(0);
		}
		if(p) p->Update();

		if(p->IsDead() && p->m_breborn==false){
			printf("怪物id:%d ,死亡 移除list 广播数据\t",p->GetId());
			//发送给所有在线角色,这个怪物的信息,他们得到了信息,判断是否dead,然后list移除怪物
			//p->setMonsterCMD(MonsterCMD_UPDATE);
			server->SendMsgToAllInWorld((char*)&(p->getMonsterProperty()),sizeof(RoleProperty),p->getMonsterProperty().worldMap_id);
			printf("怪物死亡广播\n");

			//随机物品掉落
			m_pItem_mgr->RandomItemDrop(p->getMonsterProperty().worldMap_id,p->getMonsterProperty().pos,p->m_info.ItemDropPer);

			//怪物准备重生
			p->m_breborn=true;
			
			//记录下死亡时间
			p->dwDieTime=GetTickCount();

		}else if(p->getMonsterProperty().HP>0){
			//怪物还活着就广播
			//printf("怪物id:%d,没死亡 广播数据 pos:%3.1f %3.1f %3.1f rot:%3.1f %3.1f %3.1f\t",p->GetNodeId(),p->getMonsterProperty().pos.X,p->getMonsterProperty().pos.Y,p->getMonsterProperty().pos.Z,p->getMonsterProperty().rot.X,p->getMonsterProperty().rot.Y,p->getMonsterProperty().rot.Z);
			p->setMonsterCMD(MonsterCMD_UPDATE);
			server->SendMsgToAllInWorld((char*)&(p->getMonsterProperty()),sizeof(RoleProperty),p->getMonsterProperty().worldMap_id);
		}

		//怪物准备重生标志了
		if(p->m_breborn){
			DWORD use=abs((int)GetTickCount() - (int)p->dwDieTime);
			//死亡时间达到了重生时间,就重生怪物
			if(use>p->m_info.ReBornTime){
				p->reborn();
				printf("scene%d 怪物%d重生\n",p->getMonsterProperty().worldMap_id,p->GetId());

				//MessageBox(0,"reborn","",0);
			}
		}

	}

	MonsterListUnLock();

}




//向sock发送monsterID的怪物,怪物还要在worldMap世界内
void MyMonstersMgr::SendAMonsterToClient(SOCKET sock,int monsterID,int worldMap)
{

		MyMonster* p=GetMonsterByID(monsterID);

		//指明怪物id 并所在地图
		if( p->getMonsterProperty().worldMap_id == worldMap	&& p->getMonsterProperty().HP>0
			){

			//将一个用户数据发给sock
			p->setMonsterCMD(MonsterCMD_LOGIN);
			bool ret= server->SendMsg  (sock,(char*)&p->getMonsterProperty(),sizeof(MonsterProperty));
			if(ret){
				printf("将一个怪物id:%d数据发送客户:%d ok\n",p->getMonsterProperty().ID,sock);
			}else{
				printf("将一个怪物id:%d数据发送客户:%d 失败\n",p->getMonsterProperty().ID,sock);
			}
		}

}
