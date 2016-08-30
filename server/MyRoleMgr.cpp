#include "StdAfx.h"
#include ".\myrolemgr.h"

//场景配置文件
char FilePath[255]="./scene.ini";

MyRoleMgr::MyRoleMgr(Socket::MySOCKServer* s)
{
	server=s;
	::InitializeCriticalSection(&g_cs_rolelist);//初始化临界区对象

	
}

MyRoleMgr::~MyRoleMgr(void)
{
}

//角色list加锁
void MyRoleMgr::RoleListLock()
{
	::EnterCriticalSection(&g_cs_rolelist);
}

//角色list解锁
void MyRoleMgr::RoleListUnLock()
{
	::LeaveCriticalSection(&g_cs_rolelist);
}

//得到一个角色
MyRole* MyRoleMgr::getRoleByID(int ID)
{
	MyRole* p=0;
	RoleListLock();
	std::vector<MyRole*>::iterator  list_oper;
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		p=(*list_oper);
		if(ID==p->getProp()->sock) 
		{

			break;
		}else{
			p=0;
		}

	}
	RoleListUnLock();
	return p;

}



//角色使用包裹里的hp恢复药剂
void MyRoleMgr::UsePackageHPItem(SOCKET sock)
{
	//查找对应的role角色
	MyRole* p=getRoleByID(sock);

	//角色未找到,返回
	if(p==NULL) return;

		//////---------------------------------------------------------------------------------------------///////////////
		//查找是否有hp恢复药
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<6;j++)
			{
				//1000是hp恢复药 还要个数大于1
				if(p->getPackage()->itembox[i][j]==1000 &&  p->getPackage()->itemParam2[i][j]>0)
				{
					//开始角色操作.加锁
					p->Lock();

					p->getPackage()->itemParam2[i][j]--; //个数-1 

					//hp增加
					p->getProp()->HP+=p->getPackage()->itemParam1[i][j];

					//hp有上限
					if( p->getProp()->HP> p->getProp()->MAX_HP){
						p->getProp()->HP=p->getProp()->MAX_HP;
					}

					//个数为0了后要清除这一栏
					if(p->getPackage()->itemParam2[i][j]==0){
						p->getPackage()->itembox[i][j]=0;
						p->getPackage()->itemParam1[i][j]=0;
						p->getPackage()->itemParam2[i][j]=0;
						p->getPackage()->itemParam3[i][j]=0;
						p->getPackage()->itemParam4[i][j]=0;
						p->getPackage()->itemParam5[i][j]=0;
						p->getPackage()->itemParam6[i][j]=0;
					}

				

					printf("角色%s HP恢复\n",p->getProp()->roleName );

					p->getProp()->cmd=CMD_S2C_Response_Prop;//角色属性面版更新
					server->SendMsg (p->getProp()->sock,(char*)p->getProp(),sizeof(RoleProperty));

					p->getPackage()->cmd=CMD_S2C_Response_Package;//包裹更新
					printf("角色%s更新包裹\n",p->getProp()->roleName );
					server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));

					//结束角色操作.解锁
					p->UnLock();

				}

			}
		}

}


//遍历所有角色在pos的length距离内.在就返回角色指针
RoleProperty* MyRoleMgr::GetNearestRole(vector3df pos,int length ,int worldMap)
{
	RoleProperty* rol=0;
	
	RoleListLock();

	std::vector<MyRole*>::iterator  list_oper;
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		MyRole* p=(*list_oper);

		//在距离内,还要角色没挂.而且在同一个世界地图内
		if(p->getProp()->pos.getDistanceFrom(pos)<length  &&  p->getProp()->isDeath==false && p->getProp()->HP>0 && worldMap==p->getProp()->worldMap_id )
		{
			rol=p->getProp();
			break;
		}
	}
	
	RoleListUnLock();

	return rol;
}



// 场景添加角色
void MyRoleMgr::addRole(RoleProperty* r,Package pa)
{
	


	MyRole* role =new MyRole(r,pa,server);

		RoleListLock();
	rolelist.push_back(role);
		RoleListUnLock();

}


//角色加点
void MyRoleMgr::RoleAddPoint(SOCKET sock,RoleProperty* rp)
{
	MyRole* p=0;

	//list加锁,查找对应角色
	RoleListLock();
	std::vector<MyRole*>::iterator  list_oper;
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		 p=(*list_oper);
		if(p && p->getProp()->sock==sock) 
		{
			break;
		}else{
			p=NULL;
		}
	}
	RoleListUnLock();

//未找到角色,返回
if(p==NULL) return;

	//有可用点..
	if(p->getProp()->usePoint>0)
	{
		//角色加锁
		p->Lock();

		//如果是加的力量点1点
		if(rp->Power_Point-p->getProp()->Power_Point==1){
			p->getProp()->Power_Point++;
			p->getProp()->usePoint--;
			p->getProp()->Attack++;//1力量加1点攻击力
			printf("加力量1点");
		}else if(rp->Minjie_Point-p->getProp()->Minjie_Point==1){
			p->getProp()->Minjie_Point++;
			p->getProp()->usePoint--;
			p->getProp()->Defense++; //1敏捷加1点防
			printf("加敏捷1点");
		}else if(rp->Tili_Point-p->getProp()->Tili_Point==1){
			p->getProp()->Tili_Point++;
			p->getProp()->usePoint--;	
			p->getProp()->MAX_HP+=2;//1体力加2点血
			printf("加体力1点");
		}

	

		p->getProp()->cmd=CMD_S2C_Response_Prop;//角色属性面版更新
		printf("角色%s加点 %d %d %d 剩%d\n",p->getProp()->roleName,p->getProp()->Power_Point,p->getProp()->Minjie_Point,p->getProp()->Tili_Point,p->getProp()->usePoint );
		server->SendMsg (p->getProp()->sock,(char*)p->getProp(),sizeof(RoleProperty));

		//角色解锁
		p->UnLock();
	}

}


//包裹物品交换 两个物品行列
void MyRoleMgr::ChangePackageItem(SOCKET sock,int item1row,int item1col,int item2row,int item2col )
{
	MyRole* p=getRoleByID(sock);

	if(p==NULL) return;

	//如果两个行列相同,就不用修改
	if (item1row==item2row && item1col==item2col)
	{
		return;
	}

	//两个包裹交换信息.每个包裹有6个信息
	int itemid=p->getPackage()->itembox[item1row][item1col];
	int param1=p->getPackage()->itemParam1[item1row][item1col];
	int param2=p->getPackage()->itemParam2[item1row][item1col];
	int param3=p->getPackage()->itemParam3[item1row][item1col];
	int param4=p->getPackage()->itemParam4[item1row][item1col];
	int param5=p->getPackage()->itemParam5[item1row][item1col];
	int param6=p->getPackage()->itemParam6[item1row][item1col];
	p->getPackage()->itembox[item1row][item1col]=p->getPackage()->itembox[item2row][item2col];
	p->getPackage()->itemParam1[item1row][item1col]=p->getPackage()->itemParam1[item2row][item2col];
	p->getPackage()->itemParam2[item1row][item1col]=p->getPackage()->itemParam2[item2row][item2col];
	p->getPackage()->itemParam3[item1row][item1col]=p->getPackage()->itemParam3[item2row][item2col];
	p->getPackage()->itemParam4[item1row][item1col]=p->getPackage()->itemParam4[item2row][item2col];
	p->getPackage()->itemParam5[item1row][item1col]=p->getPackage()->itemParam5[item2row][item2col];
	p->getPackage()->itemParam6[item1row][item1col]=p->getPackage()->itemParam6[item2row][item2col];
	p->getPackage()->itembox[item2row][item2col]=itemid;
	p->getPackage()->itemParam1[item2row][item2col]=param1;
	p->getPackage()->itemParam2[item2row][item2col]=param2;
	p->getPackage()->itemParam3[item2row][item2col]=param3;
	p->getPackage()->itemParam4[item2row][item2col]=param4;
	p->getPackage()->itemParam5[item2row][item2col]=param5;
	p->getPackage()->itemParam6[item2row][item2col]=param6;

	p->getPackage()->cmd=CMD_S2C_Response_Package;//包裹更新了
	printf("角色%s更新包裹\n",p->getProp()->roleName );
	server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));

}

//角色切换武器
void MyRoleMgr::ChangeWeapon(Package pack)
{ 

	int worldMap=0;

	::EnterCriticalSection(&g_cs_rolelist);
	std::vector<MyRole*>::iterator  list_oper;
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		MyRole* p=(*list_oper);
		if(pack.sock==p->getProp()->sock) 
		{
			//获取角色所在世界地图
			worldMap=p->getProp()->worldMap_id;

			//请求装上武器
			if(pack.currWeapon!=0 &&  //请求装上的武器id为0,表示卸下. 这里不为0,表示装上
				pack.currWeapon==p->getPackage()->itembox[pack.row1][pack.col1] //指定下标的武器包裹里存在才能装上 
				){
				
				//武器装上
				p->getPackage()->currWeapon=pack.currWeapon;
				//武器光武器参数
				p->getPackage()->weaponparam1=p->getPackage()->itemParam1[pack.row1][pack.col1];

				//服务器获得武器攻击力
				p->getPackage()->weaponparam2=getItemAttackById(pack.currWeapon);

				p->getPackage()->weaponparam3=p->getPackage()->itemParam3[pack.row1][pack.col1];
				p->getPackage()->weaponparam4=p->getPackage()->itemParam4[pack.row1][pack.col1];
				p->getPackage()->weaponparam5=p->getPackage()->itemParam5[pack.row1][pack.col1];
				p->getPackage()->weaponparam6=p->getPackage()->itemParam6[pack.row1][pack.col1];

				//包裹更新
				p->getPackage()->itembox[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam1[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam2[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam3[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam4[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam5[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam6[pack.row1][pack.col1]=0;

				
				//光武攻击力加倍
				if(p->getPackage()->weaponparam1>0){
					p->getPackage()->weaponparam2*=1.8;
				}

				char tem[255];
				sprintf(tem,"装上武器%d 行%d 列%d 攻击力%d",pack.currWeapon,pack.row1,pack.col1,p->getPackage()->weaponparam2 );
			//	MessageBox(0,tem,"",0);
				//切换武器成功,并向角色发送包裹更新
				
					p->getPackage()->cmd=CMD_S2C_Response_Package;//包裹
					printf("角色%s装上武器.更新包裹 武器%d 行%d 列%d 攻击力%d\n",p->getProp()->roleName ,pack.currWeapon,pack.row1,pack.col1,p->getPackage()->weaponparam2 );
				//再向角色发送更新信息
					server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));
				
				//将新数据copy到pack,用于下面的向同世界其他角色广播武器更新消息
				memcpy((char*)&pack,p->getPackage(),sizeof(Package));

				break;
			}

			//卸下武器
			else if(pack.currWeapon==0){

				//武器放入指定包裹栏保存
				p->getPackage()->itembox[pack.row1][pack.col1]=p->getPackage()->currWeapon;
				p->getPackage()->itemParam1[pack.row1][pack.col1]=p->getPackage()->weaponparam1;
				p->getPackage()->itemParam2[pack.row1][pack.col1]=p->getPackage()->weaponparam2;
				p->getPackage()->itemParam3[pack.row1][pack.col1]=p->getPackage()->weaponparam3;
				p->getPackage()->itemParam4[pack.row1][pack.col1]=p->getPackage()->weaponparam4;
				p->getPackage()->itemParam5[pack.row1][pack.col1]=p->getPackage()->weaponparam5;
				p->getPackage()->itemParam6[pack.row1][pack.col1]=p->getPackage()->weaponparam6;

				//手上武器0
				p->getPackage()->currWeapon=0;
			
				//武器属性0
				p->getPackage()->weaponparam1=0;
				p->getPackage()->weaponparam2=0;
				p->getPackage()->weaponparam3=0;
				p->getPackage()->weaponparam4=0;
				p->getPackage()->weaponparam5=0;
				p->getPackage()->weaponparam6=0;

				char tem[255];
				sprintf(tem,"卸下武器! 行%d 列%d 攻击力%d",pack.row1,pack.col1,p->getPackage()->weaponparam2 );
			//	MessageBox(0,tem,"",0);

				p->getPackage()->cmd=CMD_S2C_Response_Package;//包裹
				printf("角色%s卸下武器,更新包裹 行%d 列%d 攻击力%d \n",p->getProp()->roleName ,pack.row1,pack.col1,p->getPackage()->weaponparam2 );
				//再向角色发送更新信息
				server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));

	
				//将新数据copy到pack,用于下面的向同世界其他角色广播武器更新消息
				memcpy((char*)&pack,p->getPackage(),sizeof(Package));

				break;
			}
			
		}
	}
	::LeaveCriticalSection(&g_cs_rolelist);


	//广播给同世界其他角色此角色的武器更新消息
	
	pack.cmd=CMD_S2C_Response_ChangeWeapon;
	server->SendMsgToOtherInWorld(pack.sock,(char*)&pack, 9 ,worldMap);

}

//角色获得经验
void MyRoleMgr::RoleGainExp(SOCKET sock,int gainexp)
{
	
		MyRole* p=getRoleByID(sock);
		if(p==NULL) return;
				
		    p->Lock();

			//经验获得
			p->getProp()->Exp+=gainexp;
			//角色获得经验超过了下一级的经验,即升级!
			if(p->getProp()->Exp>=p->getProp()->NextLevelExp){
				p->getProp()->Exp=p->getProp()->Exp-p->getProp()->NextLevelExp;
				//升级!
				p->getProp()->Level++;
				//每级获得5点属性点
				p->getProp()->usePoint+=5;
				//增加攻击力
				p->getProp()->Attack+=2;
				//增加防御力
				p->getProp()->Defense+=2;
				//血上限增加
				p->getProp()->MAX_HP+=20;
				//血满
				p->getProp()->HP=p->getProp()->MAX_HP;

				p->getProp()->NextLevelExp=p->getProp()->Level*100; //升下一级必须的经验 计算公式
			}
			//角色增加经验
			p->getProp()->cmd=CMD_UserUPDATE;//角色更新
			printf("角色%s增加经验%d exp:%d lev:%d\n",p->getProp()->roleName ,  gainexp,p->getProp()->Exp,p->getProp()->Level);
			bool ret=server->SendMsg (p->getProp()->sock,(char*)p,sizeof(RoleProperty));
			if( false==ret)
			{
				//AfxMessageBox("发送角色增加经验失败");
				server->SendMsg(p->getProp()->sock,(char*)p->getProp(),sizeof(RoleProperty));
			}

			p->UnLock();

		

}

//角色出售物品给npc商店
void MyRoleMgr::RoleSellPackageItemToNpcShop(SOCKET sock,char item_x,char item_y)
{

		//得到角色		
		MyRole* p=getRoleByID(sock);
		if(p==NULL) return ;

			//查看角色要出售的物品是否存在
		if(p->getPackage()->itembox[item_x][item_y]!=0) 
		{
			
	
				//得到物品价格
				int item_price=getItembyID(p->getPackage()->itembox[item_x][item_y])->itemPrice;
				
				item_price*=0.5; //价格减半出售

				p->Lock();

				//存在则销毁物品
				p->getPackage()->itembox[item_x][item_y]=0;
				p->getPackage()->itemParam1[item_x][item_y]=0;
				p->getPackage()->itemParam2[item_x][item_y]=0;
				p->getPackage()->itemParam3[item_x][item_y]=0;
				p->getPackage()->itemParam4[item_x][item_y]=0;
				p->getPackage()->itemParam5[item_x][item_y]=0;
				p->getPackage()->itemParam6[item_x][item_y]=0;

				//角色金钱增加
				p->getPackage()->money+=item_price;

				//角色增加钱
				//包裹更新 也金钱更新 因为金钱属于包裹里的属性
	
				p->getPackage()->cmd=CMD_S2C_Response_Package;//包裹更新了

				printf("角色%s更新包裹\n",p->getProp()->roleName );
				server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));
				
				p->UnLock();

		}
	

}

//角色添加物品到包裹
void MyRoleMgr::RoleGainItem(SOCKET sock,int itembox,int itemparam1,int itemparam2,int itemparam3,int itemparam4,int itemparam5,int itemparam6)
{
		
		MyRole* p=getRoleByID(sock);
		if(p==NULL) return; 
		
			//查找是否有空栏
		for(int i=0;i<4;i++)
		{
				for(int j=0;j<6;j++)
				{
					//是空的
					if(p->getPackage()->itembox[i][j]==0)
					{
						p->Lock();
						p->getProp()->cmd=CMD_S2C_Response_Package;//包裹更新了
						
						p->getPackage()->itembox[i][j]=itembox;
						p->getPackage()->itemParam1[i][j]=itemparam1;
						p->getPackage()->itemParam2[i][j]=itemparam2;
						p->getPackage()->itemParam3[i][j]=itemparam3;
						p->getPackage()->itemParam4[i][j]=itemparam4;
						p->getPackage()->itemParam5[i][j]=itemparam5;
						p->getPackage()->itemParam6[i][j]=itemparam6;
						

						printf("角色%s增加物品id:%d\n",p->getProp()->roleName ,itembox  );
						server->SendMsg (p->getPackage()->sock,(char*)p->getPackage(),sizeof(Package));

						p->UnLock();

						return;
					}
					//这里有个问题.,就是如果包裹满了.就返回了.
				}
		}

}

//  角色更新money,并返回更新的钱
int MyRoleMgr::RoleGainMoney(SOCKET sock,int money)
{
	int m=0;
	 MyRole* p=getRoleByID(sock);
	 if(p)
	 {
		    p->Lock();

			//角色金钱增加
			p->getPackage()->money+=money;
			//保存最新的金钱
			m=p->getPackage()->money;

			

			//角色增加钱
			p->getProp()->cmd=CMD_S2C_USERMoneyUpdate;//金钱更新
			printf("角色%s增加钱%d\n",p->getProp()->roleName ,  p->getPackage()->money);
			server->SendMsg (p->getPackage()->sock,(char*)p->getPackage(),sizeof(Package));

			p->UnLock();
		
	}

	//返回最新的金钱
	return m;

}

//请求包裹
void MyRoleMgr::Response_Package(SOCKET sock)
{
	
		MyRole* p=getRoleByID(sock);
		if(p) 
		{
			p->getPackage()->cmd=CMD_S2C_Response_Package;//包裹
			printf("角色%s请求包裹\n",p->getProp()->roleName );
			server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));
		
		}
}


//更新角色信息list,从客户端获得的信息..
void MyRoleMgr::UpdateRoleList(RoleProperty* r)
{
		//查找对应角色
		MyRole* p=getRoleByID(r->sock);
		//未找到就返回
		if(p==NULL) return;
			
		//前提角色所在世界地图还是同一个
		if(p->getProp()->worldMap_id==r->worldMap_id){

				//超过200毫秒才能更新.避免客户端加速
				if(GetTickCount() - p->lastupdatetime>200){

					float speed=0;
					  //根据角色跑步模式得到步长
					if(p->getProp()->RunMode==cRun) speed=0.7f;
					else speed=0.35f;

					//计算上次坐标和角色要更新的坐标的步长 允许最大步长=应该步长*1.5
				//	if(r->pos.getDistanceFrom(p->getProp()->pos)<= speed*1.5  )
					{
						//memcpy((*list_oper),r,sizeof(RoleProperty));
						//p->Lock();

						//只更新角色状态
						p->getProp()->currState=r->currState;
						//坐标
						p->getProp()->pos=r->pos;
						//旋转
						p->getProp()->rot=r->rot;
						//跑步模式
						p->getProp()->RunMode=r->RunMode;

						



						printf("角色名:%s exp:%d exp2:%d lev:%d p:%d \n",p->getProp()->roleName ,p->getProp()->Exp,p->getProp()->NextLevelExp,p->getProp()->Level,p->getProp()->usePoint );
						//将更新的信息发送给其他角色,在同一个世界地图内的
						server->SendMsgToOtherInWorld(r->sock,(char*)r,sizeof(RoleProperty),p->getProp()->worldMap_id);

						//p->UnLock();

						//检查角色是否在世界传送区域内,在则传送
						RoleCheckChangeMap(p);

						p->lastupdatetime=GetTickCount();
					}
					
				}


			}


}


//移除角色信息list
void MyRoleMgr::RemoveRoleList(SOCKET sock)
{
	//////---------------------------------------------------------------------------------------------///////////////
	RoleListLock();

	std::vector<MyRole*>::iterator  list_oper;
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		MyRole* p=(*list_oper);
		if(sock==p->getProp()->sock) 
		{
			p->Lock();

			p->getProp()->cmd=CMD_QUIT;
			printf("------角色:%s 退出\n",p->getProp()->roleName);
			server->SendMsgToOtherInWorld(sock,(char*)p,sizeof(RoleProperty),p->getProp()->worldMap_id);
			p->UnLock();

			delete p;
			p=NULL;
			rolelist.erase(list_oper);

			break;
		}
	}

	//////---------------------------------------------------------------------------------------------///////////////
	RoleListUnLock();
}

//得到角色所在的世界地图id
int MyRoleMgr::getRoleWorldMap(SOCKET sock)
{
  int world=0;

	  MyRole* p=getRoleByID(sock);

	  //角色找到了
	  if(p)
			world=p->getProp()->worldMap_id;	

	return world;
}

//将所有在线用户数据发送给其他指定客户,前提是他们在同一个场景地图内
void MyRoleMgr::SendAllRolelistToClient(SOCKET sock,int worldMap)
{
	printf("SendAllRolelistToClient sock:%d 人总数:%d \n",sock,rolelist.size());
	std::vector<MyRole*>::iterator  list_oper;
	//////---------------------------------------------------------------------------------------------///////////////
	RoleListLock();
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		MyRole* p=(*list_oper);

		//在用一个地图内
		if(p->getProp()->worldMap_id==worldMap){
			//将用户数据一个一个都发给sock
			p->getProp()->cmd=CMD_S2C_NEWUSER;
			bool ret= server->SendMsg (sock,(char*)p->getProp(),sizeof(RoleProperty));


			//包裹信息.主要是装备的武器信息发送给sock
			p->getPackage()->cmd=CMD_S2C_Response_ChangeWeapon; //包裹更新显示.只要是发送武器信息前9字节
			ret=server->SendMsg (sock,(char*)p->getPackage(), 9 );

		}
		
	}
	//////---------------------------------------------------------------------------------------------///////////////
	RoleListUnLock();
}

//根据客户socket得到角色名
char* MyRoleMgr::getRoleNameBySocket(SOCKET sock)
{
	char* name=0;
	
		MyRole* p=getRoleByID(sock);
		if(p)
			name=p->getProp()->roleName;
		
	return name;
}

//发送角色的属性面版
void MyRoleMgr::SendRolePropToClient(SOCKET sock)
{
	//////---------------------------------------------------------------------------------------------///////////////
	//查找角色
	MyRole* p=getRoleByID(sock);
	//角色存在
	if(p){

		//将一个用户的属性信息发送
		p->getProp()->cmd=CMD_S2C_Response_Prop;
		bool ret= server->SendMsg (sock,(char*)p->getProp(),sizeof(RoleProperty));
		if(ret){
				printf("将一个用户数据%d发送ok\n",p->getProp()->sock);
		}else{
				printf("将一个用户数据%d发送失败\n",p->getProp()->sock);
		}
	}

}

//更新
void MyRoleMgr::UserUPDATEProc(char* pData,SOCKET sock,int DataLength)
{
	RoleProperty r;
	memset((char*)&r,'\0',sizeof(RoleProperty));
	memcpy((char*)&r,(char*)pData,sizeof(RoleProperty));
	if(r.cmd==CMD_UserUPDATE)
	{
		r.sock=sock;
		//更新到角色list.
		UpdateRoleList(&r);

	}

}


//角色丢弃包裹里的某物品.xy是物品下标.
void MyRoleMgr::RoleDropItem(SOCKET sock,int x,int y)
{
	int itembox=0,itemParam1,itemParam2,itemParam3,itemParam4,itemParam5,itemParam6;
int worldMap;
vector3df pos;
//找到对应角色 
MyRole* p=getRoleByID(sock);
//未找到就返回
if(p==NULL) return;

//先销毁包裹里的指定物品,然后记录下物品属性.后面添加到场景.世界地图广播有物品掉落


		//包裹那一栏要有东西
		if( p->getPackage()->itembox[x][y]!=0)
		{
			//保存物品属性到临时变量
			worldMap=p->getProp()->worldMap_id;
			pos=p->getProp()->pos;
			
			itembox=p->getPackage()->itembox[x][y];
			itemParam1=p->getPackage()->itemParam1[x][y];
			itemParam2=p->getPackage()->itemParam2[x][y];
			itemParam3=p->getPackage()->itemParam3[x][y];
			itemParam4=p->getPackage()->itemParam4[x][y];
			itemParam5=p->getPackage()->itemParam5[x][y];
			itemParam6=p->getPackage()->itemParam6[x][y];

			p->getPackage()->itembox[x][y]=0;
			p->getPackage()->itemParam1[x][y]=0;
			p->getPackage()->itemParam2[x][y]=0;
			p->getPackage()->itemParam3[x][y]=0;
			p->getPackage()->itemParam4[x][y]=0;
			p->getPackage()->itemParam5[x][y]=0;
			p->getPackage()->itemParam6[x][y]=0;

		}

	//如果没有物品丢弃.就返回
	if(itembox==0) return;



	//用户的包裹.发给角色
	p->getPackage()->cmd=CMD_S2C_Response_Package; //包裹更新显示.只要是发送武器信息
	server->SendMsg (sock,(char*)p->getPackage(), sizeof(Package) );




	SceneItem* item=new SceneItem();
	item->cmd=SceneCMD_ITEMDROP;
	 //掉落的肯定是物品,不是金钱.因为包裹里只丢弃物品
	item->item_Type=ITEMType_Item;
	item->item_index=itembox; //物品索引id
	item->param1=itemParam1; 
	item->param2=itemParam2;
	item->param3=itemParam3;
	item->param4=itemParam4;
	item->param5=itemParam5;
	item->param6=itemParam6;

	if(itembox<1000){
		//如果是武器,就显示+几
		sprintf(item->itemName,"[%s]+%d",getItembyID(itembox)->itemName,item->param1);
	}else{
		//否则就只显示名称
		sprintf(item->itemName,"[%s]",getItembyID(itembox)->itemName);
	}

	item->worldMap_id= worldMap;//物品掉落在哪个世界地图
	item->pos=pos;
	m_pItemMgr->addItem(item);
	//广播该物品掉落在地上
	server->SendMsgToAllInWorld((char*)item,sizeof(SceneItem),worldMap);


}

//角色购买npc商店,指定哪个npc 和他的商店物品下标 sock表示哪个角色
bool MyRoleMgr::RoleBuyNpcShop(SOCKET roleSOCK,char item_x,char item_y ,ShopPackage* shoppack)
{
	//标志购买成功失败
	bool buyok=false;
	//////---------------------------------------------------------------------------------------------///////////////
	//先查找对应角色
	MyRole* p=getRoleByID(roleSOCK);
	//角色未找到,返回
	if(p==NULL) return false;

	
	//---------------------------------------------------------------------------------------------
			
	//获得用户指定的物品
	int buyItem=shoppack->itembox[item_x][item_y];
	//获得物品的价格
	int price=shoppack->itemPrice[item_x][item_y];
	
	//购买前判断物品是否存在.

	//如果选择的物品栏是没物品,则流程结束
	if(buyItem==0 ){
		goto end;
	}	
	
	//开始购买
	//查看角色的金钱是否足够买这个物品
if(p->getPackage()->money>=price)
{

		//如果买的物品是消耗品.比如药品.可叠加的 
		if(buyItem>=1000 && buyItem<2000)
		{
			//---------------------------------------------------------------------------------------------
				//查找是否已有一栏物品是一类该物品.则个数加1
				for(int i=0;i<4;i++)
				{
						for(int j=0;j<6;j++)
						{
								if(p->getPackage()->itembox[i][j]==buyItem)
								{
									p->Lock();

									p->getPackage()->itemParam2[i][j]++; //物品个数++
									//标志购买物品成功.
									buyok=true;

									//金钱扣除
									p->getPackage()->money-=price;
									
									
									if(buyok)
									{
										//用户的包裹.发给sock
										p->getPackage()->cmd=CMD_S2C_Response_Package; //包裹更新显示.只要是发送武器信息
										server->SendMsg (roleSOCK,(char*)p->getPackage(), sizeof(Package) );

										p->UnLock();
										//购买流程结束
										goto end;
									}

								}
						}
				}
			//---------------------------------------------------------------------------------------------
				//能运行到这里.表示包裹里还没有这类物品,运行到下面按照正常物品添加

				for(int i=0;i<4;i++)
				{
					for(int j=0;j<6;j++)
					{

						//找个空物品栏放入刚买的物品
						if(p->getPackage()->itembox[i][j]==0)
						{
								p->Lock();

								p->getPackage()->itembox[i][j]=buyItem;
								p->getPackage()->itemParam1[i][j]=shoppack->itemParam1[item_x][item_y]; //物品恢复属性
								p->getPackage()->itemParam2[i][j]=1; //物品叠加个数
								p->getPackage()->itemParam3[i][j]=0; //物品属性3清零
								p->getPackage()->itemParam4[i][j]=0; //物品属性4清零
								p->getPackage()->itemParam5[i][j]=0; //物品属性5清零
								p->getPackage()->itemParam6[i][j]=0; //物品属性6清零

								//标志购买物品成功.
								buyok=true;

								//金钱扣除
								p->getPackage()->money-=price;

								

								if(buyok)
								{
									//用户的包裹.发给sock
									p->getPackage()->cmd=CMD_S2C_Response_Package; //包裹更新显示.只要是发送武器信息
									server->SendMsg (roleSOCK,(char*)p->getPackage(), sizeof(Package) );

									p->UnLock();

									//购买流程结束
									goto end;
								}

						}

					}
				}



			
		}else if(buyItem<1000)
		{	
			//此为购买武器.

		//---------------------------------------------------------------------------------------------
			//武器等物品添加.即无法叠加的物品
			for(int i=0;i<4;i++)
			{
				for(int j=0;j<6;j++)
				{
					//找个空物品栏放入刚买的物品
					if(p->getPackage()->itembox[i][j]==0)
					{
						p->Lock();

						p->getPackage()->itembox[i][j]=buyItem;
						p->getPackage()->itemParam1[i][j]=shoppack->itemParam1[item_x][item_y]; //物品属性1清零
						p->getPackage()->itemParam2[i][j]=shoppack->itemParam2[item_x][item_y]; //物品属性2清零
						p->getPackage()->itemParam3[i][j]=shoppack->itemParam3[item_x][item_y]; //物品属性3清零
						p->getPackage()->itemParam4[i][j]=shoppack->itemParam4[item_x][item_y]; //物品属性4清零
						p->getPackage()->itemParam5[i][j]=shoppack->itemParam5[item_x][item_y]; //物品属性5清零
						p->getPackage()->itemParam6[i][j]=shoppack->itemParam6[item_x][item_y]; //物品属性6清零

						//标志购买物品成功.
						buyok=true;

						//金钱扣除
						p->getPackage()->money-=price;



							if(buyok){
								//用户的包裹.发给sock
								p->getPackage()->cmd=CMD_S2C_Response_Package; //包裹更新显示.只要是发送武器信息
								server->SendMsg (roleSOCK,(char*)p->getPackage(), sizeof(Package) );

								p->UnLock();

								//购买流程结束
								goto end;
							}

					}

				}
			}


		//---------------------------------------------------------------------------------------------
		}

}

end:


	return buyok;

}

//发送role ID为roleSOCK的角色信息给sock
void MyRoleMgr::SendRoleToClient(SOCKET sock,int roleSOCK,int worldMap)
{
	//查找角色
	MyRole* p=getRoleByID(roleSOCK);
	//未找到角色.返回
	if(p==NULL) return;

		//必须在同一个世界才能发送给他
		if( p->getProp()->worldMap_id == worldMap)
		{

			//将一个用户的属性发给sock
			p->getProp()->cmd=CMD_S2C_NEWUSER;
			bool ret= server->SendMsg (sock,(char*)p->getProp(),sizeof(RoleProperty));
			if(ret){
				printf("将一个用户数据%d发送ok\n",p->getProp()->sock);
			}else{
				printf("将一个用户数据%d发送失败\n",p->getProp()->sock);
			}

			//用户的包裹.发给sock
			p->getPackage()->cmd=CMD_S2C_Response_ChangeWeapon; //包裹更新显示.只要是发送武器信息
			server->SendMsg (sock,(char*)p->getPackage(),  9 );

		}

}

//得到一个角色的攻击力,sock是他的标识
int MyRoleMgr::GetRoleAttackNum(SOCKET sock)
{	
	int attack=0;

		MyRole* p=getRoleByID(sock);
		//角色找到了
		if(p){
			//将一个角色的攻击力返回
			attack= p->getProp()->Attack+p->getPackage()->weaponparam2; //角色攻击力由基本攻击力和武器攻击力叠加
			
		}


	return attack;
}

//得到所有在线角色数
int MyRoleMgr::getAllRoleNum()
{
	int size=0;
	//////---------------------------------------------------------------------------------------------///////////////
	RoleListLock();

	size=rolelist.size();

	//////---------------------------------------------------------------------------------------------///////////////
	RoleListUnLock();

	return size;
}

//检查角色是否需要切换地图
void MyRoleMgr::RoleCheckChangeMap(MyRole* role)
{
	//得到角色所在世界地图id
	int worldMap=role->getProp()->worldMap_id;

	vector3df p,StartPos; 
	int NextWorldMap;
	irr::core::list<STeleport*>::Iterator  tel_oper;
	for(tel_oper=worldtelport_list->begin();tel_oper!=worldtelport_list->end();++tel_oper)
	{
		STeleport* tel=(*tel_oper);

		//是同一个世界地图,就获取他的传送点
		if(tel->worldMap_id==worldMap){
			p=tel->teleport_POS;
			StartPos=tel->teleport_StartPos;
			NextWorldMap=tel->NextworldMap_id;

			//在传送区域内.
			if( role->getProp()->pos.getDistanceFrom(p)<10 ){


				//向同一个世界的其他在线角色发送此角色移除消息
				role->getProp()->cmd=CMD_QUIT;
				server->SendMsgToOtherInWorld( role->getProp()->sock,(char*)role->getProp(),sizeof(RoleProperty),role->getProp()->worldMap_id);


				//角色切换地图的新地图id
				role->getProp()->worldMap_id=NextWorldMap;

				//
				printf("角色:%s切换地图 \n",role->getProp()->roleName);

				//角色切换地图后初始化坐标
				role->getProp()->pos=StartPos;

				role->getProp()->cmd=CMD_S2C_Response_ChangeWorld;
				//向此角色客户端发送新坐标.以及要切换地图的命令
				bool ret= server->SendMsg (role->getProp()->sock,(char*)role->getProp(),sizeof(RoleProperty));
				

				//通讯列表更新客户的世界地图id
				server->setClientWorldMap(role->getProp()->sock, role->getProp()->worldMap_id);



			}


		}
		

	}
}

//角色需要更新.比如他在地图传送区域.就要帮他切换地图
void MyRoleMgr::Update()
{
	/*
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_rolelist);
	std::vector<MyRole*>::iterator  list_oper;
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		MyRole* rol=(*list_oper);
		
		//得到角色所在世界地图id
		//int worldMap=rol->getProp()->worldMap_id;

		//vector3df p,StartPos; 
		//int NextWorldMap;
		//irr::core::list<STeleport*>::Iterator  tel_oper;
		//for(tel_oper=worldtelport_list->begin();tel_oper!=worldtelport_list->end();++tel_oper)
		//{
		//	STeleport* tel=(*tel_oper);

		//	//是同一个世界地图,就获取他的传送点
		//	if(tel->worldMap_id==worldMap){
		//		p=tel->teleport_POS;
		//		StartPos=tel->teleport_StartPos;
		//		NextWorldMap=tel->NextworldMap_id;
		//	}
		//	

		//}
		

		////在传送区域内.
		//if( rol->getProp()->pos.getDistanceFrom(p)<10 ){
		//	

		//	//向同一个世界的其他在线角色发送此角色移除消息
		//	rol->getProp()->cmd=CMD_QUIT;
		//	server->SendMsgToOtherInWorld( rol->getProp()->sock,(char*)rol->getProp(),sizeof(RoleProperty),rol->getProp()->worldMap_id);

		//	//
		//	printf("角色:%s切换地图 \n",rol->getProp()->roleName);
		//	
		//	//角色切换地图后初始化坐标
		//	rol->getProp()->pos=StartPos;

		//	//角色切换地图的新地图id
		//	rol->getProp()->worldMap_id=NextWorldMap;

		//	//通讯列表更新客户的世界地图id
		//	server->setClientWorldMap(rol->getProp()->sock, rol->getProp()->worldMap_id);


		//	rol->getProp()->cmd=CMD_S2C_Response_ChangeWorld;
		//	//向此角色发送新坐标.以及要切换地图的命令
		//	bool ret= server->SendMsg (rol->getProp()->sock,(char*)rol->getProp(),sizeof(RoleProperty));

		//	
		//}
	}

	//////---------------------------------------------------------------------------------------------///////////////
	LeaveCriticalSection(&g_cs_rolelist);

	*/

}

//是否已存在
bool MyRoleMgr::isalreadyinlist(SOCKET sock)
{
	//查找角色.如果找不到就返回null
	MyRole* rr=getRoleByID(sock);

	if(rr) return true;
	else return false;



}