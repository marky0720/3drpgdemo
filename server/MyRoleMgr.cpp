#include "StdAfx.h"
#include ".\myrolemgr.h"

//���������ļ�
char FilePath[255]="./scene.ini";

MyRoleMgr::MyRoleMgr(Socket::MySOCKServer* s)
{
	server=s;
	::InitializeCriticalSection(&g_cs_rolelist);//��ʼ���ٽ�������

	
}

MyRoleMgr::~MyRoleMgr(void)
{
}

//��ɫlist����
void MyRoleMgr::RoleListLock()
{
	::EnterCriticalSection(&g_cs_rolelist);
}

//��ɫlist����
void MyRoleMgr::RoleListUnLock()
{
	::LeaveCriticalSection(&g_cs_rolelist);
}

//�õ�һ����ɫ
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



//��ɫʹ�ð������hp�ָ�ҩ��
void MyRoleMgr::UsePackageHPItem(SOCKET sock)
{
	//���Ҷ�Ӧ��role��ɫ
	MyRole* p=getRoleByID(sock);

	//��ɫδ�ҵ�,����
	if(p==NULL) return;

		//////---------------------------------------------------------------------------------------------///////////////
		//�����Ƿ���hp�ָ�ҩ
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<6;j++)
			{
				//1000��hp�ָ�ҩ ��Ҫ��������1
				if(p->getPackage()->itembox[i][j]==1000 &&  p->getPackage()->itemParam2[i][j]>0)
				{
					//��ʼ��ɫ����.����
					p->Lock();

					p->getPackage()->itemParam2[i][j]--; //����-1 

					//hp����
					p->getProp()->HP+=p->getPackage()->itemParam1[i][j];

					//hp������
					if( p->getProp()->HP> p->getProp()->MAX_HP){
						p->getProp()->HP=p->getProp()->MAX_HP;
					}

					//����Ϊ0�˺�Ҫ�����һ��
					if(p->getPackage()->itemParam2[i][j]==0){
						p->getPackage()->itembox[i][j]=0;
						p->getPackage()->itemParam1[i][j]=0;
						p->getPackage()->itemParam2[i][j]=0;
						p->getPackage()->itemParam3[i][j]=0;
						p->getPackage()->itemParam4[i][j]=0;
						p->getPackage()->itemParam5[i][j]=0;
						p->getPackage()->itemParam6[i][j]=0;
					}

				

					printf("��ɫ%s HP�ָ�\n",p->getProp()->roleName );

					p->getProp()->cmd=CMD_S2C_Response_Prop;//��ɫ����������
					server->SendMsg (p->getProp()->sock,(char*)p->getProp(),sizeof(RoleProperty));

					p->getPackage()->cmd=CMD_S2C_Response_Package;//��������
					printf("��ɫ%s���°���\n",p->getProp()->roleName );
					server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));

					//������ɫ����.����
					p->UnLock();

				}

			}
		}

}


//�������н�ɫ��pos��length������.�ھͷ��ؽ�ɫָ��
RoleProperty* MyRoleMgr::GetNearestRole(vector3df pos,int length ,int worldMap)
{
	RoleProperty* rol=0;
	
	RoleListLock();

	std::vector<MyRole*>::iterator  list_oper;
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		MyRole* p=(*list_oper);

		//�ھ�����,��Ҫ��ɫû��.������ͬһ�������ͼ��
		if(p->getProp()->pos.getDistanceFrom(pos)<length  &&  p->getProp()->isDeath==false && p->getProp()->HP>0 && worldMap==p->getProp()->worldMap_id )
		{
			rol=p->getProp();
			break;
		}
	}
	
	RoleListUnLock();

	return rol;
}



// ������ӽ�ɫ
void MyRoleMgr::addRole(RoleProperty* r,Package pa)
{
	


	MyRole* role =new MyRole(r,pa,server);

		RoleListLock();
	rolelist.push_back(role);
		RoleListUnLock();

}


//��ɫ�ӵ�
void MyRoleMgr::RoleAddPoint(SOCKET sock,RoleProperty* rp)
{
	MyRole* p=0;

	//list����,���Ҷ�Ӧ��ɫ
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

//δ�ҵ���ɫ,����
if(p==NULL) return;

	//�п��õ�..
	if(p->getProp()->usePoint>0)
	{
		//��ɫ����
		p->Lock();

		//����Ǽӵ�������1��
		if(rp->Power_Point-p->getProp()->Power_Point==1){
			p->getProp()->Power_Point++;
			p->getProp()->usePoint--;
			p->getProp()->Attack++;//1������1�㹥����
			printf("������1��");
		}else if(rp->Minjie_Point-p->getProp()->Minjie_Point==1){
			p->getProp()->Minjie_Point++;
			p->getProp()->usePoint--;
			p->getProp()->Defense++; //1���ݼ�1���
			printf("������1��");
		}else if(rp->Tili_Point-p->getProp()->Tili_Point==1){
			p->getProp()->Tili_Point++;
			p->getProp()->usePoint--;	
			p->getProp()->MAX_HP+=2;//1������2��Ѫ
			printf("������1��");
		}

	

		p->getProp()->cmd=CMD_S2C_Response_Prop;//��ɫ����������
		printf("��ɫ%s�ӵ� %d %d %d ʣ%d\n",p->getProp()->roleName,p->getProp()->Power_Point,p->getProp()->Minjie_Point,p->getProp()->Tili_Point,p->getProp()->usePoint );
		server->SendMsg (p->getProp()->sock,(char*)p->getProp(),sizeof(RoleProperty));

		//��ɫ����
		p->UnLock();
	}

}


//������Ʒ���� ������Ʒ����
void MyRoleMgr::ChangePackageItem(SOCKET sock,int item1row,int item1col,int item2row,int item2col )
{
	MyRole* p=getRoleByID(sock);

	if(p==NULL) return;

	//�������������ͬ,�Ͳ����޸�
	if (item1row==item2row && item1col==item2col)
	{
		return;
	}

	//��������������Ϣ.ÿ��������6����Ϣ
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

	p->getPackage()->cmd=CMD_S2C_Response_Package;//����������
	printf("��ɫ%s���°���\n",p->getProp()->roleName );
	server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));

}

//��ɫ�л�����
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
			//��ȡ��ɫ���������ͼ
			worldMap=p->getProp()->worldMap_id;

			//����װ������
			if(pack.currWeapon!=0 &&  //����װ�ϵ�����idΪ0,��ʾж��. ���ﲻΪ0,��ʾװ��
				pack.currWeapon==p->getPackage()->itembox[pack.row1][pack.col1] //ָ���±��������������ڲ���װ�� 
				){
				
				//����װ��
				p->getPackage()->currWeapon=pack.currWeapon;
				//��������������
				p->getPackage()->weaponparam1=p->getPackage()->itemParam1[pack.row1][pack.col1];

				//�������������������
				p->getPackage()->weaponparam2=getItemAttackById(pack.currWeapon);

				p->getPackage()->weaponparam3=p->getPackage()->itemParam3[pack.row1][pack.col1];
				p->getPackage()->weaponparam4=p->getPackage()->itemParam4[pack.row1][pack.col1];
				p->getPackage()->weaponparam5=p->getPackage()->itemParam5[pack.row1][pack.col1];
				p->getPackage()->weaponparam6=p->getPackage()->itemParam6[pack.row1][pack.col1];

				//��������
				p->getPackage()->itembox[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam1[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam2[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam3[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam4[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam5[pack.row1][pack.col1]=0;
				p->getPackage()->itemParam6[pack.row1][pack.col1]=0;

				
				//���乥�����ӱ�
				if(p->getPackage()->weaponparam1>0){
					p->getPackage()->weaponparam2*=1.8;
				}

				char tem[255];
				sprintf(tem,"װ������%d ��%d ��%d ������%d",pack.currWeapon,pack.row1,pack.col1,p->getPackage()->weaponparam2 );
			//	MessageBox(0,tem,"",0);
				//�л������ɹ�,�����ɫ���Ͱ�������
				
					p->getPackage()->cmd=CMD_S2C_Response_Package;//����
					printf("��ɫ%sװ������.���°��� ����%d ��%d ��%d ������%d\n",p->getProp()->roleName ,pack.currWeapon,pack.row1,pack.col1,p->getPackage()->weaponparam2 );
				//�����ɫ���͸�����Ϣ
					server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));
				
				//��������copy��pack,�����������ͬ����������ɫ�㲥����������Ϣ
				memcpy((char*)&pack,p->getPackage(),sizeof(Package));

				break;
			}

			//ж������
			else if(pack.currWeapon==0){

				//��������ָ������������
				p->getPackage()->itembox[pack.row1][pack.col1]=p->getPackage()->currWeapon;
				p->getPackage()->itemParam1[pack.row1][pack.col1]=p->getPackage()->weaponparam1;
				p->getPackage()->itemParam2[pack.row1][pack.col1]=p->getPackage()->weaponparam2;
				p->getPackage()->itemParam3[pack.row1][pack.col1]=p->getPackage()->weaponparam3;
				p->getPackage()->itemParam4[pack.row1][pack.col1]=p->getPackage()->weaponparam4;
				p->getPackage()->itemParam5[pack.row1][pack.col1]=p->getPackage()->weaponparam5;
				p->getPackage()->itemParam6[pack.row1][pack.col1]=p->getPackage()->weaponparam6;

				//��������0
				p->getPackage()->currWeapon=0;
			
				//��������0
				p->getPackage()->weaponparam1=0;
				p->getPackage()->weaponparam2=0;
				p->getPackage()->weaponparam3=0;
				p->getPackage()->weaponparam4=0;
				p->getPackage()->weaponparam5=0;
				p->getPackage()->weaponparam6=0;

				char tem[255];
				sprintf(tem,"ж������! ��%d ��%d ������%d",pack.row1,pack.col1,p->getPackage()->weaponparam2 );
			//	MessageBox(0,tem,"",0);

				p->getPackage()->cmd=CMD_S2C_Response_Package;//����
				printf("��ɫ%sж������,���°��� ��%d ��%d ������%d \n",p->getProp()->roleName ,pack.row1,pack.col1,p->getPackage()->weaponparam2 );
				//�����ɫ���͸�����Ϣ
				server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));

	
				//��������copy��pack,�����������ͬ����������ɫ�㲥����������Ϣ
				memcpy((char*)&pack,p->getPackage(),sizeof(Package));

				break;
			}
			
		}
	}
	::LeaveCriticalSection(&g_cs_rolelist);


	//�㲥��ͬ����������ɫ�˽�ɫ������������Ϣ
	
	pack.cmd=CMD_S2C_Response_ChangeWeapon;
	server->SendMsgToOtherInWorld(pack.sock,(char*)&pack, 9 ,worldMap);

}

//��ɫ��þ���
void MyRoleMgr::RoleGainExp(SOCKET sock,int gainexp)
{
	
		MyRole* p=getRoleByID(sock);
		if(p==NULL) return;
				
		    p->Lock();

			//������
			p->getProp()->Exp+=gainexp;
			//��ɫ��þ��鳬������һ���ľ���,������!
			if(p->getProp()->Exp>=p->getProp()->NextLevelExp){
				p->getProp()->Exp=p->getProp()->Exp-p->getProp()->NextLevelExp;
				//����!
				p->getProp()->Level++;
				//ÿ�����5�����Ե�
				p->getProp()->usePoint+=5;
				//���ӹ�����
				p->getProp()->Attack+=2;
				//���ӷ�����
				p->getProp()->Defense+=2;
				//Ѫ��������
				p->getProp()->MAX_HP+=20;
				//Ѫ��
				p->getProp()->HP=p->getProp()->MAX_HP;

				p->getProp()->NextLevelExp=p->getProp()->Level*100; //����һ������ľ��� ���㹫ʽ
			}
			//��ɫ���Ӿ���
			p->getProp()->cmd=CMD_UserUPDATE;//��ɫ����
			printf("��ɫ%s���Ӿ���%d exp:%d lev:%d\n",p->getProp()->roleName ,  gainexp,p->getProp()->Exp,p->getProp()->Level);
			bool ret=server->SendMsg (p->getProp()->sock,(char*)p,sizeof(RoleProperty));
			if( false==ret)
			{
				//AfxMessageBox("���ͽ�ɫ���Ӿ���ʧ��");
				server->SendMsg(p->getProp()->sock,(char*)p->getProp(),sizeof(RoleProperty));
			}

			p->UnLock();

		

}

//��ɫ������Ʒ��npc�̵�
void MyRoleMgr::RoleSellPackageItemToNpcShop(SOCKET sock,char item_x,char item_y)
{

		//�õ���ɫ		
		MyRole* p=getRoleByID(sock);
		if(p==NULL) return ;

			//�鿴��ɫҪ���۵���Ʒ�Ƿ����
		if(p->getPackage()->itembox[item_x][item_y]!=0) 
		{
			
	
				//�õ���Ʒ�۸�
				int item_price=getItembyID(p->getPackage()->itembox[item_x][item_y])->itemPrice;
				
				item_price*=0.5; //�۸�������

				p->Lock();

				//������������Ʒ
				p->getPackage()->itembox[item_x][item_y]=0;
				p->getPackage()->itemParam1[item_x][item_y]=0;
				p->getPackage()->itemParam2[item_x][item_y]=0;
				p->getPackage()->itemParam3[item_x][item_y]=0;
				p->getPackage()->itemParam4[item_x][item_y]=0;
				p->getPackage()->itemParam5[item_x][item_y]=0;
				p->getPackage()->itemParam6[item_x][item_y]=0;

				//��ɫ��Ǯ����
				p->getPackage()->money+=item_price;

				//��ɫ����Ǯ
				//�������� Ҳ��Ǯ���� ��Ϊ��Ǯ���ڰ����������
	
				p->getPackage()->cmd=CMD_S2C_Response_Package;//����������

				printf("��ɫ%s���°���\n",p->getProp()->roleName );
				server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));
				
				p->UnLock();

		}
	

}

//��ɫ�����Ʒ������
void MyRoleMgr::RoleGainItem(SOCKET sock,int itembox,int itemparam1,int itemparam2,int itemparam3,int itemparam4,int itemparam5,int itemparam6)
{
		
		MyRole* p=getRoleByID(sock);
		if(p==NULL) return; 
		
			//�����Ƿ��п���
		for(int i=0;i<4;i++)
		{
				for(int j=0;j<6;j++)
				{
					//�ǿյ�
					if(p->getPackage()->itembox[i][j]==0)
					{
						p->Lock();
						p->getProp()->cmd=CMD_S2C_Response_Package;//����������
						
						p->getPackage()->itembox[i][j]=itembox;
						p->getPackage()->itemParam1[i][j]=itemparam1;
						p->getPackage()->itemParam2[i][j]=itemparam2;
						p->getPackage()->itemParam3[i][j]=itemparam3;
						p->getPackage()->itemParam4[i][j]=itemparam4;
						p->getPackage()->itemParam5[i][j]=itemparam5;
						p->getPackage()->itemParam6[i][j]=itemparam6;
						

						printf("��ɫ%s������Ʒid:%d\n",p->getProp()->roleName ,itembox  );
						server->SendMsg (p->getPackage()->sock,(char*)p->getPackage(),sizeof(Package));

						p->UnLock();

						return;
					}
					//�����и�����.,���������������.�ͷ�����.
				}
		}

}

//  ��ɫ����money,�����ظ��µ�Ǯ
int MyRoleMgr::RoleGainMoney(SOCKET sock,int money)
{
	int m=0;
	 MyRole* p=getRoleByID(sock);
	 if(p)
	 {
		    p->Lock();

			//��ɫ��Ǯ����
			p->getPackage()->money+=money;
			//�������µĽ�Ǯ
			m=p->getPackage()->money;

			

			//��ɫ����Ǯ
			p->getProp()->cmd=CMD_S2C_USERMoneyUpdate;//��Ǯ����
			printf("��ɫ%s����Ǯ%d\n",p->getProp()->roleName ,  p->getPackage()->money);
			server->SendMsg (p->getPackage()->sock,(char*)p->getPackage(),sizeof(Package));

			p->UnLock();
		
	}

	//�������µĽ�Ǯ
	return m;

}

//�������
void MyRoleMgr::Response_Package(SOCKET sock)
{
	
		MyRole* p=getRoleByID(sock);
		if(p) 
		{
			p->getPackage()->cmd=CMD_S2C_Response_Package;//����
			printf("��ɫ%s�������\n",p->getProp()->roleName );
			server->SendMsg (p->getProp()->sock,(char*)p->getPackage(),sizeof(Package));
		
		}
}


//���½�ɫ��Ϣlist,�ӿͻ��˻�õ���Ϣ..
void MyRoleMgr::UpdateRoleList(RoleProperty* r)
{
		//���Ҷ�Ӧ��ɫ
		MyRole* p=getRoleByID(r->sock);
		//δ�ҵ��ͷ���
		if(p==NULL) return;
			
		//ǰ���ɫ���������ͼ����ͬһ��
		if(p->getProp()->worldMap_id==r->worldMap_id){

				//����200������ܸ���.����ͻ��˼���
				if(GetTickCount() - p->lastupdatetime>200){

					float speed=0;
					  //���ݽ�ɫ�ܲ�ģʽ�õ�����
					if(p->getProp()->RunMode==cRun) speed=0.7f;
					else speed=0.35f;

					//�����ϴ�����ͽ�ɫҪ���µ�����Ĳ��� ������󲽳�=Ӧ�ò���*1.5
				//	if(r->pos.getDistanceFrom(p->getProp()->pos)<= speed*1.5  )
					{
						//memcpy((*list_oper),r,sizeof(RoleProperty));
						//p->Lock();

						//ֻ���½�ɫ״̬
						p->getProp()->currState=r->currState;
						//����
						p->getProp()->pos=r->pos;
						//��ת
						p->getProp()->rot=r->rot;
						//�ܲ�ģʽ
						p->getProp()->RunMode=r->RunMode;

						



						printf("��ɫ��:%s exp:%d exp2:%d lev:%d p:%d \n",p->getProp()->roleName ,p->getProp()->Exp,p->getProp()->NextLevelExp,p->getProp()->Level,p->getProp()->usePoint );
						//�����µ���Ϣ���͸�������ɫ,��ͬһ�������ͼ�ڵ�
						server->SendMsgToOtherInWorld(r->sock,(char*)r,sizeof(RoleProperty),p->getProp()->worldMap_id);

						//p->UnLock();

						//����ɫ�Ƿ������紫��������,������
						RoleCheckChangeMap(p);

						p->lastupdatetime=GetTickCount();
					}
					
				}


			}


}


//�Ƴ���ɫ��Ϣlist
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
			printf("------��ɫ:%s �˳�\n",p->getProp()->roleName);
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

//�õ���ɫ���ڵ������ͼid
int MyRoleMgr::getRoleWorldMap(SOCKET sock)
{
  int world=0;

	  MyRole* p=getRoleByID(sock);

	  //��ɫ�ҵ���
	  if(p)
			world=p->getProp()->worldMap_id;	

	return world;
}

//�����������û����ݷ��͸�����ָ���ͻ�,ǰ����������ͬһ��������ͼ��
void MyRoleMgr::SendAllRolelistToClient(SOCKET sock,int worldMap)
{
	printf("SendAllRolelistToClient sock:%d ������:%d \n",sock,rolelist.size());
	std::vector<MyRole*>::iterator  list_oper;
	//////---------------------------------------------------------------------------------------------///////////////
	RoleListLock();
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		MyRole* p=(*list_oper);

		//����һ����ͼ��
		if(p->getProp()->worldMap_id==worldMap){
			//���û�����һ��һ��������sock
			p->getProp()->cmd=CMD_S2C_NEWUSER;
			bool ret= server->SendMsg (sock,(char*)p->getProp(),sizeof(RoleProperty));


			//������Ϣ.��Ҫ��װ����������Ϣ���͸�sock
			p->getPackage()->cmd=CMD_S2C_Response_ChangeWeapon; //����������ʾ.ֻҪ�Ƿ���������Ϣǰ9�ֽ�
			ret=server->SendMsg (sock,(char*)p->getPackage(), 9 );

		}
		
	}
	//////---------------------------------------------------------------------------------------------///////////////
	RoleListUnLock();
}

//���ݿͻ�socket�õ���ɫ��
char* MyRoleMgr::getRoleNameBySocket(SOCKET sock)
{
	char* name=0;
	
		MyRole* p=getRoleByID(sock);
		if(p)
			name=p->getProp()->roleName;
		
	return name;
}

//���ͽ�ɫ���������
void MyRoleMgr::SendRolePropToClient(SOCKET sock)
{
	//////---------------------------------------------------------------------------------------------///////////////
	//���ҽ�ɫ
	MyRole* p=getRoleByID(sock);
	//��ɫ����
	if(p){

		//��һ���û���������Ϣ����
		p->getProp()->cmd=CMD_S2C_Response_Prop;
		bool ret= server->SendMsg (sock,(char*)p->getProp(),sizeof(RoleProperty));
		if(ret){
				printf("��һ���û�����%d����ok\n",p->getProp()->sock);
		}else{
				printf("��һ���û�����%d����ʧ��\n",p->getProp()->sock);
		}
	}

}

//����
void MyRoleMgr::UserUPDATEProc(char* pData,SOCKET sock,int DataLength)
{
	RoleProperty r;
	memset((char*)&r,'\0',sizeof(RoleProperty));
	memcpy((char*)&r,(char*)pData,sizeof(RoleProperty));
	if(r.cmd==CMD_UserUPDATE)
	{
		r.sock=sock;
		//���µ���ɫlist.
		UpdateRoleList(&r);

	}

}


//��ɫ�����������ĳ��Ʒ.xy����Ʒ�±�.
void MyRoleMgr::RoleDropItem(SOCKET sock,int x,int y)
{
	int itembox=0,itemParam1,itemParam2,itemParam3,itemParam4,itemParam5,itemParam6;
int worldMap;
vector3df pos;
//�ҵ���Ӧ��ɫ 
MyRole* p=getRoleByID(sock);
//δ�ҵ��ͷ���
if(p==NULL) return;

//�����ٰ������ָ����Ʒ,Ȼ���¼����Ʒ����.������ӵ�����.�����ͼ�㲥����Ʒ����


		//������һ��Ҫ�ж���
		if( p->getPackage()->itembox[x][y]!=0)
		{
			//������Ʒ���Ե���ʱ����
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

	//���û����Ʒ����.�ͷ���
	if(itembox==0) return;



	//�û��İ���.������ɫ
	p->getPackage()->cmd=CMD_S2C_Response_Package; //����������ʾ.ֻҪ�Ƿ���������Ϣ
	server->SendMsg (sock,(char*)p->getPackage(), sizeof(Package) );




	SceneItem* item=new SceneItem();
	item->cmd=SceneCMD_ITEMDROP;
	 //����Ŀ϶�����Ʒ,���ǽ�Ǯ.��Ϊ������ֻ������Ʒ
	item->item_Type=ITEMType_Item;
	item->item_index=itembox; //��Ʒ����id
	item->param1=itemParam1; 
	item->param2=itemParam2;
	item->param3=itemParam3;
	item->param4=itemParam4;
	item->param5=itemParam5;
	item->param6=itemParam6;

	if(itembox<1000){
		//���������,����ʾ+��
		sprintf(item->itemName,"[%s]+%d",getItembyID(itembox)->itemName,item->param1);
	}else{
		//�����ֻ��ʾ����
		sprintf(item->itemName,"[%s]",getItembyID(itembox)->itemName);
	}

	item->worldMap_id= worldMap;//��Ʒ�������ĸ������ͼ
	item->pos=pos;
	m_pItemMgr->addItem(item);
	//�㲥����Ʒ�����ڵ���
	server->SendMsgToAllInWorld((char*)item,sizeof(SceneItem),worldMap);


}

//��ɫ����npc�̵�,ָ���ĸ�npc �������̵���Ʒ�±� sock��ʾ�ĸ���ɫ
bool MyRoleMgr::RoleBuyNpcShop(SOCKET roleSOCK,char item_x,char item_y ,ShopPackage* shoppack)
{
	//��־����ɹ�ʧ��
	bool buyok=false;
	//////---------------------------------------------------------------------------------------------///////////////
	//�Ȳ��Ҷ�Ӧ��ɫ
	MyRole* p=getRoleByID(roleSOCK);
	//��ɫδ�ҵ�,����
	if(p==NULL) return false;

	
	//---------------------------------------------------------------------------------------------
			
	//����û�ָ������Ʒ
	int buyItem=shoppack->itembox[item_x][item_y];
	//�����Ʒ�ļ۸�
	int price=shoppack->itemPrice[item_x][item_y];
	
	//����ǰ�ж���Ʒ�Ƿ����.

	//���ѡ�����Ʒ����û��Ʒ,�����̽���
	if(buyItem==0 ){
		goto end;
	}	
	
	//��ʼ����
	//�鿴��ɫ�Ľ�Ǯ�Ƿ��㹻�������Ʒ
if(p->getPackage()->money>=price)
{

		//��������Ʒ������Ʒ.����ҩƷ.�ɵ��ӵ� 
		if(buyItem>=1000 && buyItem<2000)
		{
			//---------------------------------------------------------------------------------------------
				//�����Ƿ�����һ����Ʒ��һ�����Ʒ.�������1
				for(int i=0;i<4;i++)
				{
						for(int j=0;j<6;j++)
						{
								if(p->getPackage()->itembox[i][j]==buyItem)
								{
									p->Lock();

									p->getPackage()->itemParam2[i][j]++; //��Ʒ����++
									//��־������Ʒ�ɹ�.
									buyok=true;

									//��Ǯ�۳�
									p->getPackage()->money-=price;
									
									
									if(buyok)
									{
										//�û��İ���.����sock
										p->getPackage()->cmd=CMD_S2C_Response_Package; //����������ʾ.ֻҪ�Ƿ���������Ϣ
										server->SendMsg (roleSOCK,(char*)p->getPackage(), sizeof(Package) );

										p->UnLock();
										//�������̽���
										goto end;
									}

								}
						}
				}
			//---------------------------------------------------------------------------------------------
				//�����е�����.��ʾ�����ﻹû��������Ʒ,���е����水��������Ʒ���

				for(int i=0;i<4;i++)
				{
					for(int j=0;j<6;j++)
					{

						//�Ҹ�����Ʒ������������Ʒ
						if(p->getPackage()->itembox[i][j]==0)
						{
								p->Lock();

								p->getPackage()->itembox[i][j]=buyItem;
								p->getPackage()->itemParam1[i][j]=shoppack->itemParam1[item_x][item_y]; //��Ʒ�ָ�����
								p->getPackage()->itemParam2[i][j]=1; //��Ʒ���Ӹ���
								p->getPackage()->itemParam3[i][j]=0; //��Ʒ����3����
								p->getPackage()->itemParam4[i][j]=0; //��Ʒ����4����
								p->getPackage()->itemParam5[i][j]=0; //��Ʒ����5����
								p->getPackage()->itemParam6[i][j]=0; //��Ʒ����6����

								//��־������Ʒ�ɹ�.
								buyok=true;

								//��Ǯ�۳�
								p->getPackage()->money-=price;

								

								if(buyok)
								{
									//�û��İ���.����sock
									p->getPackage()->cmd=CMD_S2C_Response_Package; //����������ʾ.ֻҪ�Ƿ���������Ϣ
									server->SendMsg (roleSOCK,(char*)p->getPackage(), sizeof(Package) );

									p->UnLock();

									//�������̽���
									goto end;
								}

						}

					}
				}



			
		}else if(buyItem<1000)
		{	
			//��Ϊ��������.

		//---------------------------------------------------------------------------------------------
			//��������Ʒ���.���޷����ӵ���Ʒ
			for(int i=0;i<4;i++)
			{
				for(int j=0;j<6;j++)
				{
					//�Ҹ�����Ʒ������������Ʒ
					if(p->getPackage()->itembox[i][j]==0)
					{
						p->Lock();

						p->getPackage()->itembox[i][j]=buyItem;
						p->getPackage()->itemParam1[i][j]=shoppack->itemParam1[item_x][item_y]; //��Ʒ����1����
						p->getPackage()->itemParam2[i][j]=shoppack->itemParam2[item_x][item_y]; //��Ʒ����2����
						p->getPackage()->itemParam3[i][j]=shoppack->itemParam3[item_x][item_y]; //��Ʒ����3����
						p->getPackage()->itemParam4[i][j]=shoppack->itemParam4[item_x][item_y]; //��Ʒ����4����
						p->getPackage()->itemParam5[i][j]=shoppack->itemParam5[item_x][item_y]; //��Ʒ����5����
						p->getPackage()->itemParam6[i][j]=shoppack->itemParam6[item_x][item_y]; //��Ʒ����6����

						//��־������Ʒ�ɹ�.
						buyok=true;

						//��Ǯ�۳�
						p->getPackage()->money-=price;



							if(buyok){
								//�û��İ���.����sock
								p->getPackage()->cmd=CMD_S2C_Response_Package; //����������ʾ.ֻҪ�Ƿ���������Ϣ
								server->SendMsg (roleSOCK,(char*)p->getPackage(), sizeof(Package) );

								p->UnLock();

								//�������̽���
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

//����role IDΪroleSOCK�Ľ�ɫ��Ϣ��sock
void MyRoleMgr::SendRoleToClient(SOCKET sock,int roleSOCK,int worldMap)
{
	//���ҽ�ɫ
	MyRole* p=getRoleByID(roleSOCK);
	//δ�ҵ���ɫ.����
	if(p==NULL) return;

		//������ͬһ��������ܷ��͸���
		if( p->getProp()->worldMap_id == worldMap)
		{

			//��һ���û������Է���sock
			p->getProp()->cmd=CMD_S2C_NEWUSER;
			bool ret= server->SendMsg (sock,(char*)p->getProp(),sizeof(RoleProperty));
			if(ret){
				printf("��һ���û�����%d����ok\n",p->getProp()->sock);
			}else{
				printf("��һ���û�����%d����ʧ��\n",p->getProp()->sock);
			}

			//�û��İ���.����sock
			p->getPackage()->cmd=CMD_S2C_Response_ChangeWeapon; //����������ʾ.ֻҪ�Ƿ���������Ϣ
			server->SendMsg (sock,(char*)p->getPackage(),  9 );

		}

}

//�õ�һ����ɫ�Ĺ�����,sock�����ı�ʶ
int MyRoleMgr::GetRoleAttackNum(SOCKET sock)
{	
	int attack=0;

		MyRole* p=getRoleByID(sock);
		//��ɫ�ҵ���
		if(p){
			//��һ����ɫ�Ĺ���������
			attack= p->getProp()->Attack+p->getPackage()->weaponparam2; //��ɫ�������ɻ�������������������������
			
		}


	return attack;
}

//�õ��������߽�ɫ��
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

//����ɫ�Ƿ���Ҫ�л���ͼ
void MyRoleMgr::RoleCheckChangeMap(MyRole* role)
{
	//�õ���ɫ���������ͼid
	int worldMap=role->getProp()->worldMap_id;

	vector3df p,StartPos; 
	int NextWorldMap;
	irr::core::list<STeleport*>::Iterator  tel_oper;
	for(tel_oper=worldtelport_list->begin();tel_oper!=worldtelport_list->end();++tel_oper)
	{
		STeleport* tel=(*tel_oper);

		//��ͬһ�������ͼ,�ͻ�ȡ���Ĵ��͵�
		if(tel->worldMap_id==worldMap){
			p=tel->teleport_POS;
			StartPos=tel->teleport_StartPos;
			NextWorldMap=tel->NextworldMap_id;

			//�ڴ���������.
			if( role->getProp()->pos.getDistanceFrom(p)<10 ){


				//��ͬһ��������������߽�ɫ���ʹ˽�ɫ�Ƴ���Ϣ
				role->getProp()->cmd=CMD_QUIT;
				server->SendMsgToOtherInWorld( role->getProp()->sock,(char*)role->getProp(),sizeof(RoleProperty),role->getProp()->worldMap_id);


				//��ɫ�л���ͼ���µ�ͼid
				role->getProp()->worldMap_id=NextWorldMap;

				//
				printf("��ɫ:%s�л���ͼ \n",role->getProp()->roleName);

				//��ɫ�л���ͼ���ʼ������
				role->getProp()->pos=StartPos;

				role->getProp()->cmd=CMD_S2C_Response_ChangeWorld;
				//��˽�ɫ�ͻ��˷���������.�Լ�Ҫ�л���ͼ������
				bool ret= server->SendMsg (role->getProp()->sock,(char*)role->getProp(),sizeof(RoleProperty));
				

				//ͨѶ�б���¿ͻ��������ͼid
				server->setClientWorldMap(role->getProp()->sock, role->getProp()->worldMap_id);



			}


		}
		

	}
}

//��ɫ��Ҫ����.�������ڵ�ͼ��������.��Ҫ�����л���ͼ
void MyRoleMgr::Update()
{
	/*
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_rolelist);
	std::vector<MyRole*>::iterator  list_oper;
	for(list_oper=rolelist.begin();list_oper!=rolelist.end();++list_oper)
	{
		MyRole* rol=(*list_oper);
		
		//�õ���ɫ���������ͼid
		//int worldMap=rol->getProp()->worldMap_id;

		//vector3df p,StartPos; 
		//int NextWorldMap;
		//irr::core::list<STeleport*>::Iterator  tel_oper;
		//for(tel_oper=worldtelport_list->begin();tel_oper!=worldtelport_list->end();++tel_oper)
		//{
		//	STeleport* tel=(*tel_oper);

		//	//��ͬһ�������ͼ,�ͻ�ȡ���Ĵ��͵�
		//	if(tel->worldMap_id==worldMap){
		//		p=tel->teleport_POS;
		//		StartPos=tel->teleport_StartPos;
		//		NextWorldMap=tel->NextworldMap_id;
		//	}
		//	

		//}
		

		////�ڴ���������.
		//if( rol->getProp()->pos.getDistanceFrom(p)<10 ){
		//	

		//	//��ͬһ��������������߽�ɫ���ʹ˽�ɫ�Ƴ���Ϣ
		//	rol->getProp()->cmd=CMD_QUIT;
		//	server->SendMsgToOtherInWorld( rol->getProp()->sock,(char*)rol->getProp(),sizeof(RoleProperty),rol->getProp()->worldMap_id);

		//	//
		//	printf("��ɫ:%s�л���ͼ \n",rol->getProp()->roleName);
		//	
		//	//��ɫ�л���ͼ���ʼ������
		//	rol->getProp()->pos=StartPos;

		//	//��ɫ�л���ͼ���µ�ͼid
		//	rol->getProp()->worldMap_id=NextWorldMap;

		//	//ͨѶ�б���¿ͻ��������ͼid
		//	server->setClientWorldMap(rol->getProp()->sock, rol->getProp()->worldMap_id);


		//	rol->getProp()->cmd=CMD_S2C_Response_ChangeWorld;
		//	//��˽�ɫ����������.�Լ�Ҫ�л���ͼ������
		//	bool ret= server->SendMsg (rol->getProp()->sock,(char*)rol->getProp(),sizeof(RoleProperty));

		//	
		//}
	}

	//////---------------------------------------------------------------------------------------------///////////////
	LeaveCriticalSection(&g_cs_rolelist);

	*/

}

//�Ƿ��Ѵ���
bool MyRoleMgr::isalreadyinlist(SOCKET sock)
{
	//���ҽ�ɫ.����Ҳ����ͷ���null
	MyRole* rr=getRoleByID(sock);

	if(rr) return true;
	else return false;



}