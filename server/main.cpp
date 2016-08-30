// iocp.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#include "mysockserver.h"


#include "Myrolemgr.h"

#include "MyMonstersMgr.h"

#include "MyitemMgr.h"

#include "MyNPCMgr.h"

#include "MyWorldMapMgr.h"

 

irr::core::list<ClientMsg*> clientmsglist;  //��ͻ��������Ϣ��

using namespace Socket;

//��ɫ������
MyRoleMgr* m_pRoleMgr;
//���������
MyMonstersMgr* m_pMonsterMgr;
MyItemMgr* m_pItemMgr;
MyNPCMgr* m_pNPCMgr;
MyWorldMapMgr* m_pWorldMapMgr;

int tempcount=0;

CRITICAL_SECTION g_cs_msglist; //�ٽ�

void FirstLoginProc(char* pData,SOCKET sock,int DataLength);
void CMD_USERMSG_Proc(LPVOID lp);
void MonsterCMD_UnderAttack_Proc(LPVOID lp);


void SendAMonsterToClient(SOCKET sock,int monsterID);

//���ݹ���id�ù��ﱻ����
void MyMonsterUnderAttack(int ID,int damage);

void __cdecl HandleIO(LPVOID l);
//DWORD WINAPI HandleIO(LPVOID l);

MySOCKServer server;


 
 


////ʹ��boost�ڴ��
//#include <boost/pool/object_pool.hpp>
//using namespace boost;

//��ClientMsgΪ��λ�ĵ��ڴ��
//boost::object_pool<ClientMsg>  myMsgPool;

/*
��Ϣ�ص�����
*/
void _stdcall RecvData(RakNetGUID guid,
					   char * pData,
					   unsigned long DataLength)
{

	//������
	if(pData!=NULL){
		if(DataLength>0){
		//	printf("recv:%s cmd:%d msg:%s size:%d\n",guid.ToString(),pData[0], pData,DataLength);

			//1.ֱ��new
			ClientMsg* m=new ClientMsg();
				//char* tmp=new char[DataLength];//���������ڴ�,copy�Ĳ���.����ֱ������pdata.��Ϊpdata���������.��ô���þ���Ч��
				if(!m){
					MessageBox(0,"�ڴ�����ʧ��!","error",0);
				}
				//m->pData=tmp;
				memcpy(m->pData,pData,DataLength);

		 
		
			m->size=DataLength;
			m->sock=server.getClientInfoByGUID(guid)->ID;

	 
			HandleIO(m);
	
			delete m;
			m=NULL;
		}
		
	}else if(pData==NULL && DataLength==0){
		////�㲥�˳�����Ϣ ���Ƴ�����list
		m_pRoleMgr->RemoveRoleList(server.getClientInfoByGUID(guid)->ID);
	}

}




DWORD start,end;

int _tmain(int argc, _TCHAR* argv[])
{
	//char a[255]="123";
	//char* b=a;
	//b++;


	//printf("%d\n",sizeof(MonsterProperty));
	//return 1;

	//ɾ����־
	DeleteFile("server.txt");

	m_pWorldMapMgr=new MyWorldMapMgr();

	//��ȡ���д��͵�
	m_pWorldMapMgr->ReadWorldMapTeleport();

	//��ȡ������Ʒ
	ReadAllItems();

	start=GetTickCount();

 
	::InitializeCriticalSection(&g_cs_msglist);//��ʼ���ٽ�������

//��ʼ��npc������
m_pNPCMgr=new MyNPCMgr(&server);

//��ʼ����ɫ������
m_pRoleMgr=new MyRoleMgr(&server);

m_pRoleMgr->worldtelport_list=&m_pWorldMapMgr->worldtelport_list;

//��ʼ��������Ʒ������
m_pItemMgr=new MyItemMgr(&server,m_pRoleMgr);
m_pRoleMgr->m_pItemMgr=m_pItemMgr;

//��ʼ�����������
m_pMonsterMgr=new MyMonstersMgr(m_pRoleMgr,&server,m_pItemMgr);

//����������Ϣ
char FilePath[255]=".\\server.ini";
char inBuf [80];     //���ڱ����ȡ���ַ���

GetPrivateProfileString ("server","port", NULL, inBuf, 80, FilePath);  //��

int port=12;
port=atoi(inBuf);


	bool flag=server.Init(RecvData,port);
	if(flag){
			
		//DWORD id;
		//CreateThread(0,0,DoListThread,0,0,&id);
		printf("server is running!\nlisten port:%d OK\n�ȴ��û���½\n",port);
	//	printf("sizeof:%d",sizeof(RoleProperty));
	}else{
	   	printf("server  running error! ");
	}

	while (1)
	{
		Sleep(10);

		server.update();

		//��ɫ��Ҫ����.�������ڵ�ͼ��������.��Ҫ�����л���ͼ
	//	m_pRoleMgr->Update();

		printf("������Ʒ��:%d  ������:%d  ��������:%d ������������:%d \n",m_pItemMgr->getAllItemNum(1),m_pMonsterMgr->getAllMonsterNum(),m_pRoleMgr->getAllRoleNum(),server.getConnectedClientNum());


		end=GetTickCount();
		if(end-start>1000){
			start=end;

			m_pMonsterMgr->Update();

			
		
			//���ͨѶ
			//server.CheckLive();

		}
		
	}


	return 0;
}

//��ɫ�������纰��
void CMD_USERMSG_Proc(LPVOID lp)
{
	ClientMsg* msg=(ClientMsg*)lp;
	SOCKET sock=msg->sock;
	char* pData=msg->pData;
	int size=msg->size;

	//�������纰��
	ServerRoleMSG m;
	//�������ݵ��ṹ
	memcpy((char*)&m, pData, size);
	m.sock= sock;//���������ͻ�ʱ�����ʾsock��֪��..sock�Ǹ���server������sock
	printf("DoListThread �������ϢCMD_USERMSG ĳ��sock:%d ����:-%s-\n",m.sock,m.msg);

	int worldMap=m_pRoleMgr->getRoleWorldMap(sock);

	server.SendMsgToOtherInWorld( sock,(char*)&m,sizeof(ServerRoleMSG) ,worldMap );

	printf("�ͻ�%d�����纰��[��ɫ%s]:%s",sock, m_pRoleMgr->getRoleNameBySocket(sock),m.msg);
}

//��ɫ���󹥻�����
void MonsterCMD_UnderAttack_Proc(LPVOID lp)
{
	ClientMsg* msg=(ClientMsg*)lp;
	SOCKET sock=msg->sock;
	char* pData=msg->pData;
	int size=msg->size;

	//��ɫ���ڵ�ͼ
	int roleworldMap=m_pRoleMgr->getRoleWorldMap(sock);

	//���󹥻�����
	MonsterProperty ppt;
	memcpy((char*)&ppt, pData, size);

	//�������ڵ�ͼ
	int monsterworldMap=m_pMonsterMgr->getMonsterWorldMap(ppt.ID);
	if(monsterworldMap==-1){
		//����Ĺ��ﲻ����.
		return;
	}else if(monsterworldMap!=roleworldMap){
		//��ɫ�͹��ﲻ��ͬһ�������ͼ��
		return;
	}

	//���ﱻ��ɫ����,�Ȳ���ָ��sock�ͻ��Ľ�ɫ������
	int attack=m_pRoleMgr->GetRoleAttackNum( sock);

	//������ĸ��º�����ݹ㲥
	//����ָ������ı���������,�������id�ͽ�ɫ�Ĺ�����,
	//�ý�ɫ�������������һ��ʱ,�ٵ��ù��ﱻ��
	Sleep(500);
	MyMonster* m=m_pMonsterMgr->MyMonsterUnderAttack(ppt.ID,attack, sock);

	printf("\n==�ͻ�%d���󹥻�����,�ͻ�������%d\n",sock,attack);
	if(m){
		m->setMonsterCMD(MonsterCMD_UnderAttack);//��������

		//�㲥ĳ�ֱ��˿���.��ͬһ����ͼ�������˶�֪��,������ʾ�����Ѫ��Ϣ
		server.SendMsgToAllInWorld((char*)&m->getMonsterProperty(),sizeof(MonsterProperty),roleworldMap);

		//����������Ĺ�������,�����Ӿ������ɫ
		if(m->IsDead() || m->getMonsterProperty().HP<=0){

			int exp=m->getMonsterMaxHP()/10;
			printf("����Ʒ����,��ɫ���exp:%d\n",exp);

			//��ɫ���Ӿ���
			m_pRoleMgr->RoleGainExp(sock,exp);
			printf("��ɫ���Ӿ���㲥\n");
		}

	}


}


//�����û�����
void __cdecl HandleIO(LPVOID lp)
//DWORD WINAPI HandleIO(LPVOID lp)
{
	ClientMsg* msg=(ClientMsg*)lp;
	SOCKET sock=msg->sock;
	char* pData=msg->pData;
	int size=msg->size;



	unsigned char cmd= pData[0];

	switch (cmd)
	{
	case CMD_C2S_UserRequest_LOGIN:
		//�����½
		FirstLoginProc( pData, sock, size);
		break;

	case CMD_UserUPDATE:
		//��ɫ�������������Ϣ	
		m_pRoleMgr->UserUPDATEProc( pData, sock, size);
		break;

	case CMD_USERMSG:
		//��ɫ���纰��
		CMD_USERMSG_Proc(lp);
		break;

	case MonsterCMD_UnderAttack:
		//��ɫ���󹥻�����
		MonsterCMD_UnderAttack_Proc(lp);
		break;
	
	case CMD_C2S_UNKNOWUSER:
		{
			//��ɫ���ڵ�ͼ
			int roleworldMap=m_pRoleMgr->getRoleWorldMap(sock);
			//�����ɫ��Ϣ,
			RoleProperty rpt;
			memset((char*)&rpt,'\0',sizeof(RoleProperty));
			memcpy((char*)&rpt, pData, size);
			m_pRoleMgr->SendRoleToClient( sock,rpt.sock , roleworldMap);
			printf("�ͻ�%d�����ɫ��Ϣ",sock);
		}
		break;

	case MonsterCMD_UNKNOWMONSTER:
		{
			//��ɫ���ڵ�ͼ
			int roleworldMap=m_pRoleMgr->getRoleWorldMap(sock);
			//���������Ϣ,id����ʶ
			MonsterProperty rpt;
			memset((char*)&rpt,'\0',sizeof(MonsterProperty));
			memcpy((char*)&rpt, pData, size);
			m_pMonsterMgr->SendAMonsterToClient( sock,rpt.ID , roleworldMap);
			printf("�ͻ�%d���������Ϣ",sock);
		}
		break;

	case SceneCMD_ITEMDROPAll:
		{
			int worldMap=m_pRoleMgr->getRoleWorldMap(sock);
			//���󳡾����е�����Ʒ������Ϣ
			m_pItemMgr->SendAllSceneItem( sock , worldMap);
			printf("�ͻ�%d���󳡾����е�����Ʒ",sock);
		}
		break;

	case SceneCMD_ITEMDROPAllNum:
		{
			int worldMap=m_pRoleMgr->getRoleWorldMap(sock);
			//���󳡾����е�����Ʒ����
			m_pItemMgr->SendAllSceneItemNum( sock , worldMap);
			printf("�ͻ�%d���󳡾����е�����Ʒ����",sock);
		}
		break;

	case CMD_C2S_USERCollectItem:
		{
			//����������Ʒ
			//���Ҷ�Ӧ��ɫ
			Package* p=m_pRoleMgr->getRoleByID( sock)->getPackage();
			RoleProperty* role=m_pRoleMgr->getRoleByID(sock)->getProp();
			//��Ʒ��ʧ,��ɫ����.
			m_pItemMgr->ItemCollect(role,p);
			printf("�ͻ�%d����������Ʒ",sock);
		 }
		break;

	case MonsterCMD_MonsterAll:
		{
			//͵��4λ�̶���С��.����ûʲô��Ҫ����
			if(size!=4){
				printf("�ͻ�%d���󳡾����й�����Ϣʧ��!ԭ��:size!=4\n",sock);
				return ;
			}
			//���󳡾����й�����Ϣ
			int worldMap=m_pRoleMgr->getRoleWorldMap(sock);
			m_pMonsterMgr->SendAllMonsterListToClient( sock ,worldMap);
			printf("�ͻ�%d���󳡾����й�����Ϣ",sock);			  
		}
		break;

	case CMD_C2S_Request_AddPoint:
		{
			//printf("��ɫ����ӵ�");
			//��ɫ����ӵ�
			RoleProperty rpt;
			memset((char*)&rpt,'\0',sizeof(RoleProperty));
			memcpy((char*)&rpt, pData, size);
			m_pRoleMgr->RoleAddPoint( sock,&rpt);
			printf("�ͻ�%d����ӵ�",sock);
		}
		break;

	case CMD_C2S_Request_Prop:
		{
			//��ɫ�������������Ϣ
			m_pRoleMgr->SendRolePropToClient( sock);
			printf("�ͻ�%d�������������Ϣ\n",sock);
		}
		break;

	case CMD_C2S_Request_ChangeWeapon:
		{
			//��ɫ��������
			Package pack;
			memset((char*)&pack,'\0',sizeof(Package));
			memcpy((char*)&pack, pData, sizeof(Package));
			//�л�����
			pack.sock=sock;
			m_pRoleMgr->ChangeWeapon( pack);
		}
		break;

	case CMD_C2S_Request_Package:
		//��ɫ����򿪰���
		m_pRoleMgr->Response_Package(sock);
		break;

	case CMD_C2S_Request_ChangePackageItem:
		{
			//�����ɫ������Ʒ����λ��
			Package pack;
			memcpy((char*)&pack,pData,sizeof(Package));
			m_pRoleMgr->ChangePackageItem(sock,pack.row1,pack.col1,pack.row2,pack.col2);
		}
		break;

	case CMD_C2S_Request_UsePackageHPItem:
		{
			//����ʹ��װ�������������Ʒ
			m_pRoleMgr->UsePackageHPItem(sock);
			printf("��ɫ����ʹ��װ�������������Ʒ\n");
		}
		break;

	case CMD_C2S_UserRequest_NPC:
		{
			//�����npc�̵���Ϣ
			if(size!=6){
				printf("size%d ����npc�̵�.\n",size);
				return;//���ݰ�������.��������Ϣ
			}
			//��ɫ�����������ͼ���
			int worldMap=m_pRoleMgr->getRoleWorldMap(sock);
			NPCProperty npc;
			memcpy((char*)&npc,pData,size); //copyǰ6λ.��ʾcmd id worldMap
			printf("size%d ����npc:%d map:%d �̵�.\n",size,npc.ID,worldMap);
			m_pNPCMgr->SendNPCShopPackage( sock, npc.ID, worldMap);
		}
		break;

	case CMD_C2S_UserBuyShop:
		{
			//������npc�̵�ĳ��Ʒ ����Ҫ�����Ʒ���±�
			ShopPackage sp;
			memcpy((char*)&sp,pData,6); //ֻ��Ҫ6λ
			MyNPC* npc=m_pNPCMgr->getNPCbyID(sp.NPC_ID,sp.worldMap_id);
			m_pRoleMgr->RoleBuyNpcShop(sock,sp.col1,sp.row1,npc->shoppack);
			printf("��ɫ����npc�̵�\n");
		}
		break;

	case CMD_C2S_Request_DropPackageItem:
		{
			//���󶪰������ĳ��Ʒ
			if(size<22){
				printf("size%d ���󶪰������ĳ��Ʒ.���ݰ�������.��������Ϣ\n",size);
				return;//���ݰ�������.��������Ϣ
			}
			//��ɫ����
			Package p;
			memcpy((char*)&p,pData, 22 ); //ֻ��22���ֽھ͹���.��Ϊֻ��Ҫrow col����
			//��ɫ��������Ķ���
			m_pRoleMgr->RoleDropItem(sock,p.row1,p.col1);
			printf("size%d ��ɫ���������ĳ��Ʒ%d %d\n",size,p.row1,p.col1);
		}
		break;

	case CMD_C2S_Request_SellPackageItem:
		{
			//������۰��������Ʒ��npc�̵�
			if(size<22){
				printf("size%d ���󶪰������ĳ��Ʒ.���ݰ�������.��������Ϣ\n",size);
				return  ;//���ݰ�������.��������Ϣ
			}
			//��ɫ����
			Package p;
			memcpy((char*)&p,pData, 22 ); //ֻ��22���ֽھ͹���.��Ϊֻ��Ҫrow col����
			//��ɫsock,���۵���Ʒ���±�.
			m_pRoleMgr->RoleSellPackageItemToNpcShop(sock,p.row1,p.col1);
			printf("��ɫ������Ʒ%d %d��npc�̵�\n",p.col1,p.row1);
		}
		break;


	default:

		printf("�յ�δ֪id:%d��\n",cmd);
	}
   
 
	 

//	return 1;
}


//���ε�½,���͸��ͻ����Լ�����Ϣ,����������������Ϣ������
void FirstLoginProc(char* pData,SOCKET sock,int DataLength)
{
	printf("�ͻ�%d�����½",sock);

	tempcount++;

	RoleProperty r;
	//�������ݵ��ṹ
	memcpy((char*)&r,(char*)pData,sizeof(RoleProperty));
	if(r.cmd==CMD_C2S_UserRequest_LOGIN)
	{
		//���û��������Ľ�ɫ����
		RoleProperty* r2=new RoleProperty();
		memset((char*)r2,'\0',sizeof(RoleProperty));
		r2->cmd=CMD_S2C_User_LOGIN;
		r2->sock=sock;
		r2->worldMap_id=1; //��ɫ���ĸ������ͼ

		//����ͨѶserver��Ŀͻ����������ͼid
		server.setClientWorldMap(sock,r2->worldMap_id);

		r2->HP=500;
		r2->MAX_HP=500;
		r2->Defense=10;
		r2->Attack=0;

		r2->Level=1;
		r2->Exp=0;
		r2->NextLevelExp=100; //������һ�����辭��100��
		r2->Power_Point=0;
		r2->Minjie_Point=0;
		r2->Tili_Point=0;
		r2->usePoint=200; //5�����Ե�

		//if(tempcount==1) 
		r2->pos=vector3df(110,0.9,110);
		//if(tempcount==2)  r2->pos=vector3df(10,0.9,0);
		r2->rot=vector3df(0,0,0);
		r2->target=vector3df(0,0,0);
		r2->RunMode=true;
		r2->isDeath=0;


		sprintf(r2->roleName,"%s",r.roleName);
		//MessageBox(0,r2->roleName,"r2",0);

		//if(r2->roleName[0]='\0') sprintf(r2->roleName,"С��ɫ%d%s",tempcount,r.roleName);


		if ( r.charSex==2 || r.charSex==1 )
		{
			r2->charSex=r.charSex;
		}else{
			r2->charSex=0;
		}

		//if(tempcount%2==0){
  //         	r2->charSex=1; //��ɫ�Ա� 1 �� 2 Ů
		//}else{
		//	r2->charSex=1;
		//}
		
		r2->currState=cIdle;

		//��ʼ����ɫ����
		Package pack;
		pack.cmd=CMD_S2C_Response_Package;

		//��������
		for(int i=0;i<4;i++){
			for(int j=0;j<6;j++){
				pack.itembox[i][j]=0; //����Ʒ
				pack.itemParam1[i][j]=0; //��Ʒ����1����
				pack.itemParam2[i][j]=0; //��Ʒ����2����
				pack.itemParam3[i][j]=0; //��Ʒ����3����
				pack.itemParam4[i][j]=0; //��Ʒ����4����
				pack.itemParam5[i][j]=0; //��Ʒ����5����
				pack.itemParam6[i][j]=0; //��Ʒ����6����
			}
		}

		pack.money=200000; //0��Ǯ

		//������ӵ����
		pack.sock=sock;
		pack.currWeapon=STD_Weapon; //��ʼ����������Ϊ������ŵ�
		pack.weaponparam1=3; //��ʼ����������6������
		pack.weaponparam2=getItemAttackById(STD_Weapon); //����������
		pack.weaponparam3=0;
		pack.weaponparam4=0;
		pack.weaponparam5=0;
		pack.weaponparam6=0;


		//���乥�����ӱ�
		if(pack.weaponparam1>0){
			pack.weaponparam2*=1.8;
		}
		

	//	pack.itembox[0][0]=SRJ_Weapon; //1��1�� ˫�н�
	
		pack.itembox[1][0]=CWJ_Weapon; //2��1�� ������
		pack.itemParam2[1][0]=getItemAttackById(CWJ_Weapon);

		pack.itembox[1][1]=JXKJ_Weapon; //2��2�� ��е����
		pack.itemParam2[1][1]=getItemAttackById(JXKJ_Weapon);

		pack.itembox[2][0]=XBJ_Weapon; //3��1�� ������
		pack.itemParam2[2][0]=getItemAttackById(XBJ_Weapon);

		pack.itembox[2][1]=DMGSJ_Weapon; //3��2�� �����
		pack.itemParam2[2][1]=getItemAttackById(DMGSJ_Weapon); //3��2�� �����

		pack.itembox[3][2]=BLD_Weapon; //4��1�� ��ɏ��
		pack.itemParam1[3][2]=6; //4��1�� ����
		pack.itemParam2[3][2]=getItemAttackById(BLD_Weapon)*1.8; //

		pack.itembox[3][3]=BLD_Weapon; //4��1�� ��ɏ��
		pack.itemParam1[3][3]=2; //4��1�� ����
		pack.itemParam2[3][3]=getItemAttackById(BLD_Weapon)*1.8; //

		pack.itembox[3][4]=BLD_Weapon; //4��1�� ��ɏ��
		pack.itemParam1[3][4]=3; //4��1�� ����
		pack.itemParam2[3][4]=getItemAttackById(BLD_Weapon)*1.8; //

		pack.itembox[3][5]=BLD_Weapon; //4��1�� ��ɏ��
		pack.itemParam1[3][5]=5; //4��1�� ����
		pack.itemParam2[3][5]=getItemAttackById(BLD_Weapon)*1.8; //

		pack.itembox[2][0]=STD_Weapon; 
		pack.itemParam1[2][0]=1; // ����
		pack.itemParam2[2][0]=getItemAttackById(STD_Weapon)*1.8;

		pack.itembox[2][3]=STD_Weapon; 
		pack.itemParam1[2][3]=2; // ����
		pack.itemParam2[2][3]=getItemAttackById(STD_Weapon)*1.8;

		pack.itembox[2][1]=STD_Weapon; 
		pack.itemParam1[2][1]=5; // ����
		pack.itemParam2[2][1]=getItemAttackById(STD_Weapon)*1.8;

		pack.itembox[0][5]=Cai7_Weapon; 
		pack.itemParam1[0][5]=6; // ����
		pack.itemParam2[0][5]=getItemAttackById(Cai7_Weapon)*1.8;

		pack.itembox[0][4]=Cai7_Weapon; 
		pack.itemParam1[0][4]=4; // ����
		pack.itemParam2[0][4]=getItemAttackById(Cai7_Weapon)*1.8;


		//���͸��յ�½�Ŀͻ����Լ��Ľ�ɫ��Ϣ
		bool re ;// =send(sock,(char*)r2,sizeof(RoleProperty),0);//���ͻ����ͽṹ���� 
		re=	server.SendMsg(sock,(char*)r2,sizeof(RoleProperty));
		if(re==false){
			//	AfxMessageBox("���͸��յ�½�Ŀͻ����Լ��Ľ�ɫ��Ϣʧ��!");
		} 

		//���͸���ɫ���İ�����Ϣ.Ҫȫ����Ϣ.��Ϊ��Ҫ֪��������Ϣ
		re=	server.SendMsg(sock,(char*)&pack,sizeof(Package));
		if(re==false){
			
		}

		//�����������û����ݷ��͸������յ�½�Ľ�ɫ,ǰ����������ͬһ��������ͼ��
		//m_pRoleMgr->SendAllRolelistToClient(sock,r2->worldMap_id);

		//Sleep(20);
		//���͸��յ�½�Ŀͻ�,����������Ϣ
		//m_pMonsterMgr->SendAllMonsterListToClient(sock,r2->worldMap_id);

		//���͸�����ͬ��������߽�ɫ��,�����������½�ɫ��½��
		r2->cmd=CMD_S2C_NEWUSER;
		server.SendMsgToOtherInWorld(r2->sock,(char*)r2,sizeof(RoleProperty),r2->worldMap_id);

		//���͸�����ͬ��������߽�ɫ��,�����������½�ɫ������������
		pack.cmd=CMD_S2C_Response_ChangeWeapon;
		server.SendMsgToOtherInWorld(r2->sock,(char*)&pack, 9 ,r2->worldMap_id);
		//////---------------------------------------------------------------------------------------------///////////////
		m_pRoleMgr->addRole(r2,pack); //���ѽ�ɫ��ӽ�����
		//////---------------------------------------------------------------------------------------------///////////////

		printf("�ͻ�%d��½�ɹ�,��ɫ��:%s cmd:%d",sock,r2->roleName,r2->cmd );
		 
		//printf("hp:%d    rot:%3.1f %3.1f %3.1f \n",r2->HP,r2->rot.X,r2->rot.Y,r2->rot.Z);

	}
}


