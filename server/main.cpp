// iocp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "mysockserver.h"


#include "Myrolemgr.h"

#include "MyMonstersMgr.h"

#include "MyitemMgr.h"

#include "MyNPCMgr.h"

#include "MyWorldMapMgr.h"

 

irr::core::list<ClientMsg*> clientmsglist;  //存客户请求的消息链

using namespace Socket;

//角色管理器
MyRoleMgr* m_pRoleMgr;
//怪物管理器
MyMonstersMgr* m_pMonsterMgr;
MyItemMgr* m_pItemMgr;
MyNPCMgr* m_pNPCMgr;
MyWorldMapMgr* m_pWorldMapMgr;

int tempcount=0;

CRITICAL_SECTION g_cs_msglist; //临界

void FirstLoginProc(char* pData,SOCKET sock,int DataLength);
void CMD_USERMSG_Proc(LPVOID lp);
void MonsterCMD_UnderAttack_Proc(LPVOID lp);


void SendAMonsterToClient(SOCKET sock,int monsterID);

//根据怪物id让怪物被攻击
void MyMonsterUnderAttack(int ID,int damage);

void __cdecl HandleIO(LPVOID l);
//DWORD WINAPI HandleIO(LPVOID l);

MySOCKServer server;


 
 


////使用boost内存池
//#include <boost/pool/object_pool.hpp>
//using namespace boost;

//以ClientMsg为单位的的内存池
//boost::object_pool<ClientMsg>  myMsgPool;

/*
消息回调函数
*/
void _stdcall RecvData(RakNetGUID guid,
					   char * pData,
					   unsigned long DataLength)
{

	//有数据
	if(pData!=NULL){
		if(DataLength>0){
		//	printf("recv:%s cmd:%d msg:%s size:%d\n",guid.ToString(),pData[0], pData,DataLength);

			//1.直接new
			ClientMsg* m=new ClientMsg();
				//char* tmp=new char[DataLength];//必须申请内存,copy的操作.不能直接引用pdata.因为pdata如果回收了.那么引用就无效了
				if(!m){
					MessageBox(0,"内存申请失败!","error",0);
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
		////广播退出的消息 并移出在线list
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

	//删除日志
	DeleteFile("server.txt");

	m_pWorldMapMgr=new MyWorldMapMgr();

	//读取所有传送点
	m_pWorldMapMgr->ReadWorldMapTeleport();

	//读取所有物品
	ReadAllItems();

	start=GetTickCount();

 
	::InitializeCriticalSection(&g_cs_msglist);//初始化临界区对象

//初始化npc管理器
m_pNPCMgr=new MyNPCMgr(&server);

//初始化角色管理类
m_pRoleMgr=new MyRoleMgr(&server);

m_pRoleMgr->worldtelport_list=&m_pWorldMapMgr->worldtelport_list;

//初始化场景物品管理类
m_pItemMgr=new MyItemMgr(&server,m_pRoleMgr);
m_pRoleMgr->m_pItemMgr=m_pItemMgr;

//初始化怪物管理类
m_pMonsterMgr=new MyMonstersMgr(m_pRoleMgr,&server,m_pItemMgr);

//武器配置信息
char FilePath[255]=".\\server.ini";
char inBuf [80];     //用于保存读取的字符串

GetPrivateProfileString ("server","port", NULL, inBuf, 80, FilePath);  //读

int port=12;
port=atoi(inBuf);


	bool flag=server.Init(RecvData,port);
	if(flag){
			
		//DWORD id;
		//CreateThread(0,0,DoListThread,0,0,&id);
		printf("server is running!\nlisten port:%d OK\n等待用户登陆\n",port);
	//	printf("sizeof:%d",sizeof(RoleProperty));
	}else{
	   	printf("server  running error! ");
	}

	while (1)
	{
		Sleep(10);

		server.update();

		//角色需要更新.比如他在地图传送区域.就要帮他切换地图
	//	m_pRoleMgr->Update();

		printf("掉落物品数:%d  怪物数:%d  在线人数:%d 服务器连接数:%d \n",m_pItemMgr->getAllItemNum(1),m_pMonsterMgr->getAllMonsterNum(),m_pRoleMgr->getAllRoleNum(),server.getConnectedClientNum());


		end=GetTickCount();
		if(end-start>1000){
			start=end;

			m_pMonsterMgr->Update();

			
		
			//检查通讯
			//server.CheckLive();

		}
		
	}


	return 0;
}

//角色请求世界喊话
void CMD_USERMSG_Proc(LPVOID lp)
{
	ClientMsg* msg=(ClientMsg*)lp;
	SOCKET sock=msg->sock;
	char* pData=msg->pData;
	int size=msg->size;

	//请求世界喊话
	ServerRoleMSG m;
	//接收数据到结构
	memcpy((char*)&m, pData, size);
	m.sock= sock;//发给其他客户时必须表示sock才知道..sock是根据server的连接sock
	printf("DoListThread 处理的消息CMD_USERMSG 某人sock:%d 喊话:-%s-\n",m.sock,m.msg);

	int worldMap=m_pRoleMgr->getRoleWorldMap(sock);

	server.SendMsgToOtherInWorld( sock,(char*)&m,sizeof(ServerRoleMSG) ,worldMap );

	printf("客户%d在世界喊话[角色%s]:%s",sock, m_pRoleMgr->getRoleNameBySocket(sock),m.msg);
}

//角色请求攻击怪物
void MonsterCMD_UnderAttack_Proc(LPVOID lp)
{
	ClientMsg* msg=(ClientMsg*)lp;
	SOCKET sock=msg->sock;
	char* pData=msg->pData;
	int size=msg->size;

	//角色所在地图
	int roleworldMap=m_pRoleMgr->getRoleWorldMap(sock);

	//请求攻击怪物
	MonsterProperty ppt;
	memcpy((char*)&ppt, pData, size);

	//怪物所在地图
	int monsterworldMap=m_pMonsterMgr->getMonsterWorldMap(ppt.ID);
	if(monsterworldMap==-1){
		//请求的怪物不存在.
		return;
	}else if(monsterworldMap!=roleworldMap){
		//角色和怪物不在同一个世界地图内
		return;
	}

	//怪物被角色攻击,先查找指定sock客户的角色攻击力
	int attack=m_pRoleMgr->GetRoleAttackNum( sock);

	//将怪物的更新后的数据广播
	//调用指定怪物的被攻击方法,传怪物的id和角色的攻击力,
	//让角色先做动作到差不多一半时,再调用怪物被砍
	Sleep(500);
	MyMonster* m=m_pMonsterMgr->MyMonsterUnderAttack(ppt.ID,attack, sock);

	printf("\n==客户%d请求攻击怪物,客户攻击力%d\n",sock,attack);
	if(m){
		m->setMonsterCMD(MonsterCMD_UnderAttack);//设置命令

		//广播某怪被人砍了.让同一个地图的所有人都知道,更新显示怪物飙血信息
		server.SendMsgToAllInWorld((char*)&m->getMonsterProperty(),sizeof(MonsterProperty),roleworldMap);

		//如果被攻击的怪物死了,就增加经验给角色
		if(m->IsDead() || m->getMonsterProperty().HP<=0){

			int exp=m->getMonsterMaxHP()/10;
			printf("有物品掉落,角色获得exp:%d\n",exp);

			//角色增加经验
			m_pRoleMgr->RoleGainExp(sock,exp);
			printf("角色增加经验广播\n");
		}

	}


}


//处理用户请求
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
		//请求登陆
		FirstLoginProc( pData, sock, size);
		break;

	case CMD_UserUPDATE:
		//角色向服务器更新信息	
		m_pRoleMgr->UserUPDATEProc( pData, sock, size);
		break;

	case CMD_USERMSG:
		//角色世界喊话
		CMD_USERMSG_Proc(lp);
		break;

	case MonsterCMD_UnderAttack:
		//角色请求攻击怪物
		MonsterCMD_UnderAttack_Proc(lp);
		break;
	
	case CMD_C2S_UNKNOWUSER:
		{
			//角色所在地图
			int roleworldMap=m_pRoleMgr->getRoleWorldMap(sock);
			//请求角色信息,
			RoleProperty rpt;
			memset((char*)&rpt,'\0',sizeof(RoleProperty));
			memcpy((char*)&rpt, pData, size);
			m_pRoleMgr->SendRoleToClient( sock,rpt.sock , roleworldMap);
			printf("客户%d请求角色信息",sock);
		}
		break;

	case MonsterCMD_UNKNOWMONSTER:
		{
			//角色所在地图
			int roleworldMap=m_pRoleMgr->getRoleWorldMap(sock);
			//请求怪物信息,id做标识
			MonsterProperty rpt;
			memset((char*)&rpt,'\0',sizeof(MonsterProperty));
			memcpy((char*)&rpt, pData, size);
			m_pMonsterMgr->SendAMonsterToClient( sock,rpt.ID , roleworldMap);
			printf("客户%d请求怪物信息",sock);
		}
		break;

	case SceneCMD_ITEMDROPAll:
		{
			int worldMap=m_pRoleMgr->getRoleWorldMap(sock);
			//请求场景所有掉落物品具体信息
			m_pItemMgr->SendAllSceneItem( sock , worldMap);
			printf("客户%d请求场景所有掉落物品",sock);
		}
		break;

	case SceneCMD_ITEMDROPAllNum:
		{
			int worldMap=m_pRoleMgr->getRoleWorldMap(sock);
			//请求场景所有掉落物品数量
			m_pItemMgr->SendAllSceneItemNum( sock , worldMap);
			printf("客户%d请求场景所有掉落物品数量",sock);
		}
		break;

	case CMD_C2S_USERCollectItem:
		{
			//请求捡地上物品
			//查找对应角色
			Package* p=m_pRoleMgr->getRoleByID( sock)->getPackage();
			RoleProperty* role=m_pRoleMgr->getRoleByID(sock)->getProp();
			//物品消失,角色更新.
			m_pItemMgr->ItemCollect(role,p);
			printf("客户%d请求捡地上物品",sock);
		 }
		break;

	case MonsterCMD_MonsterAll:
		{
			//偷懒4位固定大小吧.反正没什么重要数据
			if(size!=4){
				printf("客户%d请求场景所有怪物信息失败!原因:size!=4\n",sock);
				return ;
			}
			//请求场景所有怪物信息
			int worldMap=m_pRoleMgr->getRoleWorldMap(sock);
			m_pMonsterMgr->SendAllMonsterListToClient( sock ,worldMap);
			printf("客户%d请求场景所有怪物信息",sock);			  
		}
		break;

	case CMD_C2S_Request_AddPoint:
		{
			//printf("角色请求加点");
			//角色请求加点
			RoleProperty rpt;
			memset((char*)&rpt,'\0',sizeof(RoleProperty));
			memcpy((char*)&rpt, pData, size);
			m_pRoleMgr->RoleAddPoint( sock,&rpt);
			printf("客户%d请求加点",sock);
		}
		break;

	case CMD_C2S_Request_Prop:
		{
			//角色请求属性面版信息
			m_pRoleMgr->SendRolePropToClient( sock);
			printf("客户%d请求属性面版信息\n",sock);
		}
		break;

	case CMD_C2S_Request_ChangeWeapon:
		{
			//角色请求换武器
			Package pack;
			memset((char*)&pack,'\0',sizeof(Package));
			memcpy((char*)&pack, pData, sizeof(Package));
			//切换武器
			pack.sock=sock;
			m_pRoleMgr->ChangeWeapon( pack);
		}
		break;

	case CMD_C2S_Request_Package:
		//角色请求打开包裹
		m_pRoleMgr->Response_Package(sock);
		break;

	case CMD_C2S_Request_ChangePackageItem:
		{
			//请求角色包裹物品交换位置
			Package pack;
			memcpy((char*)&pack,pData,sizeof(Package));
			m_pRoleMgr->ChangePackageItem(sock,pack.row1,pack.col1,pack.row2,pack.col2);
		}
		break;

	case CMD_C2S_Request_UsePackageHPItem:
		{
			//请求使用装备包裹里的消耗品
			m_pRoleMgr->UsePackageHPItem(sock);
			printf("角色请求使用装备包裹里的消耗品\n");
		}
		break;

	case CMD_C2S_UserRequest_NPC:
		{
			//请求打开npc商店信息
			if(size!=6){
				printf("size%d 请求npc商店.\n",size);
				return;//数据包不完整.丢弃此消息
			}
			//角色所属的世界地图编号
			int worldMap=m_pRoleMgr->getRoleWorldMap(sock);
			NPCProperty npc;
			memcpy((char*)&npc,pData,size); //copy前6位.表示cmd id worldMap
			printf("size%d 请求npc:%d map:%d 商店.\n",size,npc.ID,worldMap);
			m_pNPCMgr->SendNPCShopPackage( sock, npc.ID, worldMap);
		}
		break;

	case CMD_C2S_UserBuyShop:
		{
			//请求购买npc商店某物品 传来要买的物品的下标
			ShopPackage sp;
			memcpy((char*)&sp,pData,6); //只需要6位
			MyNPC* npc=m_pNPCMgr->getNPCbyID(sp.NPC_ID,sp.worldMap_id);
			m_pRoleMgr->RoleBuyNpcShop(sock,sp.col1,sp.row1,npc->shoppack);
			printf("角色购买npc商店\n");
		}
		break;

	case CMD_C2S_Request_DropPackageItem:
		{
			//请求丢包裹里的某物品
			if(size<22){
				printf("size%d 请求丢包裹里的某物品.数据包不完整.丢弃此消息\n",size);
				return;//数据包不完整.丢弃此消息
			}
			//角色包裹
			Package p;
			memcpy((char*)&p,pData, 22 ); //只收22个字节就够了.因为只需要row col而已
			//角色丢包裹里的东西
			m_pRoleMgr->RoleDropItem(sock,p.row1,p.col1);
			printf("size%d 角色丢包裹里的某物品%d %d\n",size,p.row1,p.col1);
		}
		break;

	case CMD_C2S_Request_SellPackageItem:
		{
			//请求出售包裹里的物品给npc商店
			if(size<22){
				printf("size%d 请求丢包裹里的某物品.数据包不完整.丢弃此消息\n",size);
				return  ;//数据包不完整.丢弃此消息
			}
			//角色包裹
			Package p;
			memcpy((char*)&p,pData, 22 ); //只收22个字节就够了.因为只需要row col而已
			//角色sock,出售的物品的下标.
			m_pRoleMgr->RoleSellPackageItemToNpcShop(sock,p.row1,p.col1);
			printf("角色出售物品%d %d给npc商店\n",p.col1,p.row1);
		}
		break;


	default:

		printf("收到未知id:%d包\n",cmd);
	}
   
 
	 

//	return 1;
}


//初次登陆,传送给客户他自己的信息,并把所有在线人信息发给他
void FirstLoginProc(char* pData,SOCKET sock,int DataLength)
{
	printf("客户%d请求登陆",sock);

	tempcount++;

	RoleProperty r;
	//接收数据到结构
	memcpy((char*)&r,(char*)pData,sizeof(RoleProperty));
	if(r.cmd==CMD_C2S_UserRequest_LOGIN)
	{
		//向用户发送他的角色数据
		RoleProperty* r2=new RoleProperty();
		memset((char*)r2,'\0',sizeof(RoleProperty));
		r2->cmd=CMD_S2C_User_LOGIN;
		r2->sock=sock;
		r2->worldMap_id=1; //角色在哪个世界地图

		//更新通讯server里的客户所在世界地图id
		server.setClientWorldMap(sock,r2->worldMap_id);

		r2->HP=500;
		r2->MAX_HP=500;
		r2->Defense=10;
		r2->Attack=0;

		r2->Level=1;
		r2->Exp=0;
		r2->NextLevelExp=100; //升到下一级所需经验100点
		r2->Power_Point=0;
		r2->Minjie_Point=0;
		r2->Tili_Point=0;
		r2->usePoint=200; //5点属性点

		//if(tempcount==1) 
		r2->pos=vector3df(110,0.9,110);
		//if(tempcount==2)  r2->pos=vector3df(10,0.9,0);
		r2->rot=vector3df(0,0,0);
		r2->target=vector3df(0,0,0);
		r2->RunMode=true;
		r2->isDeath=0;


		sprintf(r2->roleName,"%s",r.roleName);
		//MessageBox(0,r2->roleName,"r2",0);

		//if(r2->roleName[0]='\0') sprintf(r2->roleName,"小角色%d%s",tempcount,r.roleName);


		if ( r.charSex==2 || r.charSex==1 )
		{
			r2->charSex=r.charSex;
		}else{
			r2->charSex=0;
		}

		//if(tempcount%2==0){
  //         	r2->charSex=1; //角色性别 1 男 2 女
		//}else{
		//	r2->charSex=1;
		//}
		
		r2->currState=cIdle;

		//初始化角色包裹
		Package pack;
		pack.cmd=CMD_S2C_Response_Package;

		//包裹清零
		for(int i=0;i<4;i++){
			for(int j=0;j<6;j++){
				pack.itembox[i][j]=0; //空物品
				pack.itemParam1[i][j]=0; //物品属性1清零
				pack.itemParam2[i][j]=0; //物品属性2清零
				pack.itemParam3[i][j]=0; //物品属性3清零
				pack.itemParam4[i][j]=0; //物品属性4清零
				pack.itemParam5[i][j]=0; //物品属性5清零
				pack.itemParam6[i][j]=0; //物品属性6清零
			}
		}

		pack.money=200000; //0金钱

		//包裹的拥有者
		pack.sock=sock;
		pack.currWeapon=STD_Weapon; //初始化手上武器为武器编号的
		pack.weaponparam1=3; //初始化手上武器6个属性
		pack.weaponparam2=getItemAttackById(STD_Weapon); //武器攻击力
		pack.weaponparam3=0;
		pack.weaponparam4=0;
		pack.weaponparam5=0;
		pack.weaponparam6=0;


		//光武攻击力加倍
		if(pack.weaponparam1>0){
			pack.weaponparam2*=1.8;
		}
		

	//	pack.itembox[0][0]=SRJ_Weapon; //1行1列 双刃剑
	
		pack.itembox[1][0]=CWJ_Weapon; //2行1列 楚王剑
		pack.itemParam2[1][0]=getItemAttackById(CWJ_Weapon);

		pack.itembox[1][1]=JXKJ_Weapon; //2行2列 机械阔剑
		pack.itemParam2[1][1]=getItemAttackById(JXKJ_Weapon);

		pack.itembox[2][0]=XBJ_Weapon; //3行1列 玄冰剑
		pack.itemParam2[2][0]=getItemAttackById(XBJ_Weapon);

		pack.itembox[2][1]=DMGSJ_Weapon; //3行2列 大马革
		pack.itemParam2[2][1]=getItemAttackById(DMGSJ_Weapon); //3行2列 大马革

		pack.itembox[3][2]=BLD_Weapon; //4行1列 白蓮刀
		pack.itemParam1[3][2]=6; //4行1列 光武
		pack.itemParam2[3][2]=getItemAttackById(BLD_Weapon)*1.8; //

		pack.itembox[3][3]=BLD_Weapon; //4行1列 白蓮刀
		pack.itemParam1[3][3]=2; //4行1列 光武
		pack.itemParam2[3][3]=getItemAttackById(BLD_Weapon)*1.8; //

		pack.itembox[3][4]=BLD_Weapon; //4行1列 白蓮刀
		pack.itemParam1[3][4]=3; //4行1列 光武
		pack.itemParam2[3][4]=getItemAttackById(BLD_Weapon)*1.8; //

		pack.itembox[3][5]=BLD_Weapon; //4行1列 白蓮刀
		pack.itemParam1[3][5]=5; //4行1列 光武
		pack.itemParam2[3][5]=getItemAttackById(BLD_Weapon)*1.8; //

		pack.itembox[2][0]=STD_Weapon; 
		pack.itemParam1[2][0]=1; // 光武
		pack.itemParam2[2][0]=getItemAttackById(STD_Weapon)*1.8;

		pack.itembox[2][3]=STD_Weapon; 
		pack.itemParam1[2][3]=2; // 光武
		pack.itemParam2[2][3]=getItemAttackById(STD_Weapon)*1.8;

		pack.itembox[2][1]=STD_Weapon; 
		pack.itemParam1[2][1]=5; // 光武
		pack.itemParam2[2][1]=getItemAttackById(STD_Weapon)*1.8;

		pack.itembox[0][5]=Cai7_Weapon; 
		pack.itemParam1[0][5]=6; // 光武
		pack.itemParam2[0][5]=getItemAttackById(Cai7_Weapon)*1.8;

		pack.itembox[0][4]=Cai7_Weapon; 
		pack.itemParam1[0][4]=4; // 光武
		pack.itemParam2[0][4]=getItemAttackById(Cai7_Weapon)*1.8;


		//发送给刚登陆的客户他自己的角色信息
		bool re ;// =send(sock,(char*)r2,sizeof(RoleProperty),0);//给客户发送结构数据 
		re=	server.SendMsg(sock,(char*)r2,sizeof(RoleProperty));
		if(re==false){
			//	AfxMessageBox("发送给刚登陆的客户他自己的角色信息失败!");
		} 

		//发送给角色他的包裹信息.要全部信息.因为他要知道包裹信息
		re=	server.SendMsg(sock,(char*)&pack,sizeof(Package));
		if(re==false){
			
		}

		//将所有在线用户数据发送给其他刚登陆的角色,前提是他们在同一个场景地图内
		//m_pRoleMgr->SendAllRolelistToClient(sock,r2->worldMap_id);

		//Sleep(20);
		//发送给刚登陆的客户,场景怪物信息
		//m_pMonsterMgr->SendAllMonsterListToClient(sock,r2->worldMap_id);

		//发送给其他同世界的在线角色的,告诉他们有新角色登陆了
		r2->cmd=CMD_S2C_NEWUSER;
		server.SendMsgToOtherInWorld(r2->sock,(char*)r2,sizeof(RoleProperty),r2->worldMap_id);

		//发送给其他同世界的在线角色的,告诉他们有新角色的武器更新了
		pack.cmd=CMD_S2C_Response_ChangeWeapon;
		server.SendMsgToOtherInWorld(r2->sock,(char*)&pack, 9 ,r2->worldMap_id);
		//////---------------------------------------------------------------------------------------------///////////////
		m_pRoleMgr->addRole(r2,pack); //并把角色添加进链表
		//////---------------------------------------------------------------------------------------------///////////////

		printf("客户%d登陆成功,角色名:%s cmd:%d",sock,r2->roleName,r2->cmd );
		 
		//printf("hp:%d    rot:%3.1f %3.1f %3.1f \n",r2->HP,r2->rot.X,r2->rot.Y,r2->rot.Z);

	}
}


