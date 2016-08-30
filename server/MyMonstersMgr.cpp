#include "StdAfx.h"
#include ".\mymonstersmgr.h"

MyMonstersMgr::MyMonstersMgr(MyRoleMgr* role_mgr,	Socket::MySOCKServer* ser, MyItemMgr* m_pItem_mgr)
{
	::InitializeCriticalSection(&g_cs_monsterlist);//��ʼ���ٽ�������

	server=ser;

	this->role_mgr=role_mgr;

	this->m_pItem_mgr=m_pItem_mgr;


	count=1;

//��ȡ���й������������Ϣ�б�
	ReadAllMonstersInfo();

//���س�����ͼˢ���ļ�
    ReadSceneMonster();

}


void MyMonstersMgr::ReadSceneMonster()
{
	char File[255]="data//scene_monster.txt";
	FILE* log;
	log = fopen(File, "r");
	//�ļ�������.����
	if (log == NULL)
		return ;
	char strtemp[255]="";

	printf("====���ع��ﵽ������ʼ!====\n");

	int rownum=GetTxtRowNum(File);
	for(int i=1;i<=rownum;i++){
		//ÿ�ж�ȡ
		GetTxtByRowNum(File,strtemp,i);
		if(strtemp[0]=='/' && strtemp[1]=='/'){
			//printf("%d����ע��\n",i-1);
			continue; //������ע��.������һ��
		}else{

			//����һ��������.��ʾ�Ǹ���Ʒ��Ϣ
			if(strtemp[0]>=48 && strtemp[0]<=57 ){
				
				int sceneID=0;
				int monsterID=0;
				int monsterType=0;
				vector3df pos;
				sscanf(strtemp, "%d  %d  %d  %f,%f,%f",  
					&sceneID, ///SceneID(�������)
					&monsterID,   //ID(����ID�ڳ������Ψһ��ʶ)
					&monsterType,   // Type(��������)   
					&pos.X,&pos.Y,&pos.Z // ˢ������
					); 

				AddMonster(monsterType,monsterID,pos,sceneID);

				printf("����:%d ��id:%d ������:%d ����:%3.1f,%3.1f,%3.1f\n",  
					sceneID, ///SceneID(�������)
					monsterID,   //ID(����ID�ڳ������Ψһ��ʶ)
					monsterType,   // Type(��������)   
					pos.X,pos.Y,pos.Z // ˢ������
					);

			}
		}
	}

	printf("====���ع��ﵽ�������!====\n");

}

//�õ��������������Ϣ
SMonsterInfo* MyMonstersMgr::getMonsterBaseInfoByTypeID(int type)
{
	SMonsterInfo *ppp=0;
	std::list<SMonsterInfo*>::iterator  list_oper;
	//monsters_info_list���ȶ�������list.ֻ�ڳ����ʼ��ʱ���,���Բ��ü���
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

//��ȡ���й������������Ϣ
void MyMonstersMgr::ReadAllMonstersInfo()
{
  
	   char File[255]="data//monsters.txt";
	   FILE* log;
	   log = fopen(File, "r");
	   //�ļ�������.����
	   if (log == NULL)
		   return ;
	   char strtemp[255]="";

   printf("===ȫ�ֹ������ö�ȡ��ʼ===\n");

	   int rownum=GetTxtRowNum(File);
	   for(int i=1;i<=rownum;i++){
		   //ÿ�ж�ȡ
		   GetTxtByRowNum(File,strtemp,i);
		   if(strtemp[0]=='/' && strtemp[1]=='/'){
			   //printf("%d����ע��\n",i-1);
			   continue; //������ע��.������һ��
		   }else{

			   //����һ��������.��ʾ�Ǹ���Ʒ��Ϣ
			   if(strtemp[0]>=48 && strtemp[0]<=57 ){
				   SMonsterInfo* m=new SMonsterInfo;

				   sscanf(strtemp, "%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d", 
					   &m->type,&m->Name, //�������� ����
					   &m->Level,&m->HP,  //�ȼ� ���Ѫ
					   &m->Min_Damage,&m->Max_Damage, //��С/�� �˺�
					   &m->Defense,&m->MaxMove_distense, //������  //������ˢ�ֵ��ƶ���Χ���뾶
					   &m->Attack_distense,&m->View_distense,&m->MoveSpeed, //��������, ������Ұ��Χ �ƶ��ٶ�
					   &m->ReBornTime,&m->ItemDropPer, //����ʱ�� ��Ʒ����
					   &m->MoneyDropPer,&m->MaxItemDropLevel //��Ǯ���� �����Ʒ����ȼ�
					   ); 

				   printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d\n",  
					   m->type,m->Name, //�������� ����
					   m->Level,m->HP,  //�ȼ� ���Ѫ
					   m->Min_Damage,m->Max_Damage, //��С/�� �˺�
					   m->Defense,m->MaxMove_distense, //������  //������ˢ�ֵ��ƶ���Χ���뾶
					   m->Attack_distense,m->View_distense,m->MoveSpeed, //��������, ������Ұ��Χ �ƶ��ٶ�
					   m->ReBornTime,m->ItemDropPer, //����ʱ�� ��Ʒ����
					   m->MoneyDropPer,m->MaxItemDropLevel //��Ǯ���� �����Ʒ����ȼ�
					  );

				   monsters_info_list.push_back(m);
			   }
		   }
	   }

	   printf("===ȫ�ֹ������ö�ȡ���!����%d�ֹ���===\n",monsters_info_list.size());

   
}

MyMonstersMgr::~MyMonstersMgr(void)
{

}

//����list����
void MyMonstersMgr::MonsterListLock()
{
	::EnterCriticalSection(&g_cs_monsterlist);
}

//����list����
void MyMonstersMgr::MonsterListUnLock()
{
	::LeaveCriticalSection(&g_cs_monsterlist);
}

//�õ��������ڵ������ͼid
int MyMonstersMgr::getMonsterWorldMap(int monsterID)
{
	int world=-1;
	
		MyMonster *p=GetMonsterByID(monsterID);

		if(p){
			world=p->getMonsterProperty().worldMap_id;
			
		}

	return world;

}

//�õ�ָ������
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





//����������н�ɫ,���Ƿ���������.�ھͷ��ؽ�ɫָ��
RoleProperty* MyMonstersMgr::GetNearestRole(MyMonster* m)
{
	
	RoleProperty* p=role_mgr->GetNearestRole(m->getMonsterProperty().pos,//���������
		m->m_info.View_distense, //���������
		m->getMonsterProperty().worldMap_id); //���������ڿɷ������

	return p;

};

//�õ����й�������
int MyMonstersMgr::getAllMonsterNum()
{
	int size=0;
	  MonsterListLock();
	size=monster_list.size();
	  MonsterListUnLock();
	return size;
}

/*

���һ�����ﵽ������
TypeID�ǹ��������
ID�ǹ��ﳡ���ڵ��ż�nodeid
pos�ǹ����ʼ�����ĵص�
worldmap�ǹ����ʼ�����ĸ���ͼ

*/
void MyMonstersMgr::AddMonster(int TypeID,int ID,vector3df pos,int worldmap)
{
	//�õ������͵Ĺ����������
	SMonsterInfo* sm=getMonsterBaseInfoByTypeID(TypeID);

	//���ù�������
	MonsterProperty p;
	p.Attack=sm->Max_Damage;
	p.Defense=sm->Defense;
	if(strlen(sm->Name)<20){
		strcpy(p.Name,sm->Name); //��������
	}else{
		MessageBox(0,"�й������Ƴ��ȳ���!����!","",0);
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
	
	//	beast->GetNode()->setDebugDataVisible(scene::EDS_BBOX ); //������ʾ��Χ��
		MonsterListLock();
	monster_list.push_back(beast); //��ӽ�����
	    MonsterListUnLock();
	printf("AddMonster id:%d \n",beast->GetId());

}



//���ݹ���id�ù��ﱻ����,attack�ǽ�ɫ������
MyMonster* MyMonstersMgr::MyMonsterUnderAttack(int MonserID,int attack,SOCKET attacker)
{
	
		MyMonster* p=GetMonsterByID(MonserID);
		if(p){
			//�ҵ�ָ������,������������.
			p->UnderAttack(attack,attacker);
			
		}

	return p;

}



//�������й������ݸ��ͻ���
void MyMonstersMgr::SendAllMonsterListToClient(SOCKET sock , int worldMap)
{

	printf("�������й������ݸ��ͻ��� sock:%d ������:%d \n",sock, monster_list.size());

	MonsterListLock();
	std::vector<MyMonster*>::iterator  list_oper;

	for(list_oper=  monster_list.begin();list_oper!= monster_list.end();++list_oper)
	{
		MyMonster* p=(*list_oper);

		//������ָ���ĳ�����ͼ��
		if(worldMap==p->getMonsterProperty().worldMap_id){
			//���û�����һ��һ��������sock
			if(p->IsDead() || p->getMonsterProperty().HP<=0 ){
				continue;
			}
			p->setMonsterCMD(MonsterCMD_LOGIN);
			bool f=server->SendMsg (sock,(char*)&(p->getMonsterProperty()),sizeof(MonsterProperty));
			if(f) printf("������%d���ݶ������ͻ�:%d ok\n",p->getMonsterProperty().ID,sock);
			else
				printf("��������%d�ݶ������ͻ�:%d ʧ��\n",p->getMonsterProperty().ID,sock);

			char ss[255];
			sprintf(ss,"������id:%d�����ݷ���",p->getMonsterProperty().ID);
			WriteToLog(ss);
		}
	
	}


	MonsterListUnLock();
}



/*
�������й���
*/
void MyMonstersMgr::Update()
{


	MonsterListLock();
	std::vector<MyMonster*>::iterator  list_oper;

	for(list_oper=monster_list.begin();list_oper!=monster_list.end();++list_oper)
	{

		MyMonster* p=(*list_oper);

		//�õ���һ���ڹ��������ڵĻ��ɫ
		RoleProperty* rp=GetNearestRole(p);

		if(rp!=0){
			//���������ڵ�
			p->SetCharacter(rp);
		}else{
			p->SetCharacter(0);
		}
		if(p) p->Update();

		if(p->IsDead() && p->m_breborn==false){
			printf("����id:%d ,���� �Ƴ�list �㲥����\t",p->GetId());
			//���͸��������߽�ɫ,����������Ϣ,���ǵõ�����Ϣ,�ж��Ƿ�dead,Ȼ��list�Ƴ�����
			//p->setMonsterCMD(MonsterCMD_UPDATE);
			server->SendMsgToAllInWorld((char*)&(p->getMonsterProperty()),sizeof(RoleProperty),p->getMonsterProperty().worldMap_id);
			printf("���������㲥\n");

			//�����Ʒ����
			m_pItem_mgr->RandomItemDrop(p->getMonsterProperty().worldMap_id,p->getMonsterProperty().pos,p->m_info.ItemDropPer);

			//����׼������
			p->m_breborn=true;
			
			//��¼������ʱ��
			p->dwDieTime=GetTickCount();

		}else if(p->getMonsterProperty().HP>0){
			//���ﻹ���ž͹㲥
			//printf("����id:%d,û���� �㲥���� pos:%3.1f %3.1f %3.1f rot:%3.1f %3.1f %3.1f\t",p->GetNodeId(),p->getMonsterProperty().pos.X,p->getMonsterProperty().pos.Y,p->getMonsterProperty().pos.Z,p->getMonsterProperty().rot.X,p->getMonsterProperty().rot.Y,p->getMonsterProperty().rot.Z);
			p->setMonsterCMD(MonsterCMD_UPDATE);
			server->SendMsgToAllInWorld((char*)&(p->getMonsterProperty()),sizeof(RoleProperty),p->getMonsterProperty().worldMap_id);
		}

		//����׼��������־��
		if(p->m_breborn){
			DWORD use=abs((int)GetTickCount() - (int)p->dwDieTime);
			//����ʱ��ﵽ������ʱ��,����������
			if(use>p->m_info.ReBornTime){
				p->reborn();
				printf("scene%d ����%d����\n",p->getMonsterProperty().worldMap_id,p->GetId());

				//MessageBox(0,"reborn","",0);
			}
		}

	}

	MonsterListUnLock();

}




//��sock����monsterID�Ĺ���,���ﻹҪ��worldMap������
void MyMonstersMgr::SendAMonsterToClient(SOCKET sock,int monsterID,int worldMap)
{

		MyMonster* p=GetMonsterByID(monsterID);

		//ָ������id �����ڵ�ͼ
		if( p->getMonsterProperty().worldMap_id == worldMap	&& p->getMonsterProperty().HP>0
			){

			//��һ���û����ݷ���sock
			p->setMonsterCMD(MonsterCMD_LOGIN);
			bool ret= server->SendMsg  (sock,(char*)&p->getMonsterProperty(),sizeof(MonsterProperty));
			if(ret){
				printf("��һ������id:%d���ݷ��Ϳͻ�:%d ok\n",p->getMonsterProperty().ID,sock);
			}else{
				printf("��һ������id:%d���ݷ��Ϳͻ�:%d ʧ��\n",p->getMonsterProperty().ID,sock);
			}
		}

}
