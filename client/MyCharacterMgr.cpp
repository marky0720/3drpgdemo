#include ".\mycharactermgr.h"

MyCharacterMgr::MyCharacterMgr(MySceneManager* scenemgr)
{

	this->dev=getIrrDevice();
	myScene_Mgr=scenemgr;
	smgr=dev->getSceneManager();

	mouseDownL=false;

	character=0;//主角色

	timer=dev->getTimer();

	::InitializeCriticalSection(&g_cs_charlist);//初始化临界区对象

	::InitializeCriticalSection(&g_cs_msglist);//初始化临界区对象

	//消息清空
	msglist.clear();

	//在线角色列表清空
	char_list.clear();

}

MyCharacterMgr::~MyCharacterMgr(void)
{
}

//插入用户消息,来自服务器
void MyCharacterMgr::insertCharData(const char* pData,const int DataLength)
{
	printf(" MyCharacterMgr::insertCharData\n");
	ClientMsg* m=new ClientMsg();
	char* tmp=new char[DataLength];
	memcpy(tmp,pData,DataLength);
	m->pData=tmp;
	m->size=DataLength;


		//////---------------------------------------------------------------------------------------------///////////////	
		::EnterCriticalSection(&g_cs_msglist);
		msglist.push_back(m); //添加处理进链表
		::LeaveCriticalSection(&g_cs_msglist);
		//////---------------------------------------------------------------------------------------------///////////////	
	

}

//处理服务器发来的角色消息
void  MyCharacterMgr::DoListThread( )
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

			//主角色登陆
			if(cmd==CMD_S2C_User_LOGIN){
				CMD_USERLOGIN_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
				//其他在线角色登陆
			}else if(cmd==CMD_S2C_NEWUSER){
				CMD_NEWUSER_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
				//主角色和其他角色更新
			}else if(cmd==CMD_UserUPDATE){
				////MessageBox(0,"角色更新!","",0); 比如升级了.等等
				CMD_UserUPDATE_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			}else if(cmd==CMD_USERMSG){
				CMD_USERMSG_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			}else if(cmd==CMD_QUIT){
				CMD_QUIT_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			}else if(cmd==CMD_S2C_USERUNDERATTACK){
				CMD_USERUNDERATTACK_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			}else if(cmd==CMD_S2C_USERMoneyUpdate){
				////MessageBox(0,"角色金钱更新消息in!","",0);
				//我日,把下面的方法写成了被攻击的proc
				CMD_USERMoneyUpdate_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			}else if(cmd==CMD_S2C_Response_Prop){
				//服务器响应角色请求的属性面版信息
				CMD_ServerResponse_Prop_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			}else if(cmd==CMD_S2C_Response_ChangeWeapon){
				//MessageBox(0,"服务器更新在线角色的武器!!!","",0);
				//服务器更新在线角色的武器
				CMD_Response_ChangeWeapon_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			}else if(cmd==CMD_S2C_Response_Package){
				//服务器返回角色的包裹
				CMD_S2C_Response_Package_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			}else if(cmd==CMD_S2C_Response_ChangeWorld){
				//服务器要求角色切换地图
				CMD_S2C_Response_ChangeWorld_PROC((*msg_oper)->pData,(*msg_oper)->size);
				delete (*msg_oper)->pData;
				delete (*msg_oper);
				msglist.erase(msg_oper);
				break;
			}else{
				char temp[255];
				sprintf(temp,"有未知包cmd:%d",cmd);
				MessageBox(0,temp,"",0);
			}



		}
	}

	::LeaveCriticalSection(&g_cs_msglist);
	//////---------------------------------------------------------------------------------------------///////////////	

	

}

//主角色向服务器请求所有在线角色
void MyCharacterMgr::getOnlineAllCharacter()
{
	RoleProperty p;
	memset((char*)&p,'\0',sizeof(RoleProperty));
	p.cmd=CMD_C2S_Request_AllOnlineRole;
	myScene_Mgr->client->SendMsg((char*)&p,4); //偷懒4位固定大小吧.反正没什么重要数据

}

//设置所有角色武器显示?
void MyCharacterMgr::setAllRoleWeaponVisible(bool v)
{
	character->m_weapon_mgr->setWeaponVisible(v);

	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator  list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{

		MyCharacter *p=*list_oper;
		p->m_weapon_mgr->setWeaponVisible(v);
	}
	::LeaveCriticalSection(&g_cs_charlist);
	//////---------------------------------------------------------------------------------------------///////////////	


}

//设置所有角色服装显示?
void MyCharacterMgr::setAllRoleClothVisible(bool v)
{
	character->m_charClothMgr->setClothVisible(v);

	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{

		MyCharacter *p=*list_oper;
		p->m_charClothMgr->setClothVisible(v);
	}
	::LeaveCriticalSection(&g_cs_charlist);
	//////---------------------------------------------------------------------------------------------///////////////	

}

//服务器要求角色切换地图
void MyCharacterMgr::CMD_S2C_Response_ChangeWorld_PROC(char* pData,unsigned long length)
{
	RoleProperty srp;
	memcpy((char*)&srp,(char*)pData,sizeof(RoleProperty));

	//角色更新世界地图id
	character->SetWorldMap(srp.worldMap_id ) ;

	//角色更新坐标
	character->setPosition(srp.pos);

	//切换场景模型
	myScene_Mgr->ChangeWorld(srp.worldMap_id);
}

//角色包裹更新
void MyCharacterMgr::CMD_S2C_Response_Package_PROC(char* pData,unsigned long length)
{
	//MessageBox(0,"包裹更新.继续查找角色","",0);

	if(sizeof(Package)!=length){
		 MessageBox(0,"CMD_S2C_Response_Package_PROC 获取大小和结构大小不一致!","",0);
	}
	Package pack;
	memcpy((char*)&pack,(char*)pData,sizeof(Package));

	//先判断sock.是否主角色
	if(character->getRoleproperty().sock==pack.sock){
		//MessageBox(0,"主角色包裹更新","",0);

		//必须先更新武器,如果武器编号改变或参数改变的话.
		if(character->getPackage()->currWeapon!=pack.currWeapon || character->getPackage()->weaponparam1!=pack.weaponparam1 ){
			//更新显示武器
			character->setCurWeapon(pack.currWeapon,pack.weaponparam1);
			char temp[255];
			sprintf(temp,"更新武器%d",pack.currWeapon);
			////MessageBox(0,temp,"",0);


		}
		//更新包裹
		character->setPackage(pack);
	}


	//查找其他角色
	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{

		MyCharacter *p=*list_oper;
		if(p->getRoleproperty().sock==pack.sock)
		{
			////MessageBox(0,"其他角色包裹更新","",0);
			//更新武器
			////MessageBox(0,"CMD_Response_ChangeWeapon_PROC!!!","",0);
			if(p->getPackage()->currWeapon != pack.currWeapon) p->m_weapon_mgr->ChangeWeapon(pack.currWeapon,pack.weaponparam1);
			//更新包裹
			p->setPackage(pack);
			break;
		}
	}
	::LeaveCriticalSection(&g_cs_charlist);
	//////---------------------------------------------------------------------------------------------///////////////	


	
	
}

//其他在线角色武器更新
void  MyCharacterMgr::CMD_Response_ChangeWeapon_PROC(char* pData,unsigned long length)
{
	//MessageBox(0,"其他在线角色武器更新 ","",0);
	
	//只收取9字节...
	if(length!=9)
	{
		 MessageBox(0,"CMD_Response_ChangeWeapon_PROC 获取大小和结构大小不一致!","",0);
		return;
	}
	Package srp;
	memset((char*)&srp,0,sizeof(Package));
	memcpy((char*)&srp,(char*)pData, 9 );

	//查找其他角色
	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator  list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{

		MyCharacter *p=*list_oper;
		if(p->getRoleproperty().sock==srp.sock)
		{
			//更新武器
			//char str[255];
			p->m_weapon_mgr->ChangeWeapon(srp.currWeapon,srp.weaponparam1);
			printf("==在线其他角色%s更新武器param%d\n",p->getRoleproperty().roleName,srp.weaponparam1);
			//MessageBox(0,str,"",0);
			break;
		}
	}
	::LeaveCriticalSection(&g_cs_charlist);
	//////---------------------------------------------------------------------------------------------///////////////	

}

//属性面版更新
void MyCharacterMgr::CMD_ServerResponse_Prop_PROC(char* pData,unsigned long length)
{
	if(sizeof(RoleProperty)!=length){
	 	 MessageBox(0,"CMD_ServerResponse_Prop_PROC 获取大小和结构大小不一致!","",0);
	}
	//MessageBox(0,"属性面版更新","",0);
	RoleProperty srp;
	memcpy((char*)&srp,(char*)pData,sizeof(RoleProperty));

	//直接设置属性面版相关属性
	character->setPropPlane(&srp);
}

//角色金钱更新消息
void MyCharacterMgr::CMD_USERMoneyUpdate_PROC(char* pData,unsigned long length)
{
	if(sizeof(Package)!=length){
	  	 MessageBox(0,"CMD_USERMoneyUpdate_PROC 获取大小和结构大小不一致!","",0);
	}

	Package srp;
	memcpy((char*)&srp,(char*)pData,sizeof(Package));
	//printf("CMD_UserUPDATE: name:%s sock:%d  pos:%3.1f %3.1f %3.1f \n",srp.roleName,srp.sock,srp.pos.X,srp.pos.Y,srp.pos.Z);
	printf("--主角色%d金钱更新消息 money:%d sock:%d --\n",srp.sock,srp.money,srp.sock);
	//直接设置
	character->setMoney(srp.money);

}


//角色被攻击消息 包括主角色和其他角色
void MyCharacterMgr::CMD_USERUNDERATTACK_PROC(char* pData,unsigned long length)
{
	if(sizeof(RoleProperty)!=length){
		MessageBox(0,"CMD_USERUNDERATTACK_PROC 获取大小和结构大小不一致!","",0);
	}

	RoleProperty srp;
	memcpy((char*)&srp,(char*)pData,sizeof(RoleProperty));
	//printf("CMD_UserUPDATE: name:%s sock:%d  pos:%3.1f %3.1f %3.1f \n",srp.roleName,srp.sock,srp.pos.X,srp.pos.Y,srp.pos.Z);


	printf("--有角色被攻击 sock:%d--",srp.sock);

	//判断是否主角色被攻击
	if(character->getRoleproperty().sock==srp.sock){

		printf("--主角色被攻击name:%s-- hp:%d  currState:%d  \n",srp.roleName,srp.HP,srp.currState);
		//只更新hp值
		int lasthp=character->getRoleproperty().HP;
		int damage=lasthp-srp.HP;//计算伤害值

		//显示伤害数
		character->UnderAttack(damage);

		return;
	}

	//是否是其他角色被攻击
//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator  list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{
		
		MyCharacter *p=*list_oper;
		if(p->getRoleproperty().sock==srp.sock)
		{

			printf("--name:%s被攻击--  hp:%d currState:%d \n",srp.roleName,srp.HP,srp.currState);
			//只更新hp值
			int lasthp=p->getRoleproperty().HP;
			int damage=lasthp-srp.HP;//计算伤害值

			//显示伤害数
			//bug 把character写这了.造成了其他角色被攻击,主角色也被攻击了..
			p->UnderAttack(damage);
			break;
		}
	}
	::LeaveCriticalSection(&g_cs_charlist);
//////---------------------------------------------------------------------------------------------///////////////	

}

void MyCharacterMgr::CMD_QUIT_PROC(char* pData,unsigned long length)
{  
	if(sizeof(RoleProperty)!=length){
		MessageBox(0,"CMD_QUIT_PROC 获取大小和结构大小不一致!","",0);
	}


	RoleProperty srp;
	memcpy((char*)&srp,(char*)pData,sizeof(RoleProperty));
//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator  list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{
		MyCharacter *p=*list_oper;
		if(p->getRoleproperty().sock==srp.sock)
		{

			printf("--有人退出了 name:%s sock:%d !--\n",srp.roleName,srp.sock);
			
			p->drop();
			delete p;
			p=0;
			char_list.erase(list_oper);

			break;
		}
	}
	::LeaveCriticalSection(&g_cs_charlist);

//////---------------------------------------------------------------------------------------------///////////////	

}
void MyCharacterMgr::CMD_USERLOGIN_PROC(const char* pData,const unsigned long length)
{		
	if(sizeof(RoleProperty)!=length){
		MessageBox(0,"CMD_USERDATA_PROC 获取大小和结构大小不一致!","",0);
	}

	printf("主角色信息获取完毕 CMD_USERDATA\n");
	
	
	//添加主角色
	AddMainCharacter( pData, length);

	//设置摄象机焦点
	myScene_Mgr->myCamera->setTargetNode(character->getNode());

}

void MyCharacterMgr::CMD_NEWUSER_PROC(char* pData,unsigned long length)
{
	if(sizeof(RoleProperty)!=length){
		MessageBox(0,"CMD_NEWUSER_PROC 获取大小和结构大小不一致!","",0);
	}

	printf("-CMD_NEWUSER ================------------\n");
	
	AddCharacter(pData, length);
	printf("在线角色信息获取一个 CMD_NEWUSER\n");
}



void MyCharacterMgr::CMD_USERMSG_PROC(char* pData,unsigned long length )
{
	if(sizeof(ServerRoleMSG)!=length){
		MessageBox(0,"CMD_USERMSG_PROC 获取大小和结构大小不一致!","",0);
	}

	ServerRoleMSG msg;
	memcpy((char*)&msg,(char*)pData,sizeof(ServerRoleMSG));
	printf("--有人喊话了,还不知谁--\n");
//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator  list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{
		MyCharacter *p=*list_oper;
		if(p->getRoleproperty().sock==msg.sock)
		{
			printf("--有人喊话了 name:%s sock:%d msg:%s--\n",p->getRoleproperty().roleName,msg.sock,msg.msg);
			WCHAR * pwStr = ConvertLPCSTRToLPWSTR( msg.msg ); 
			//角色头上显示字
			p->PostMsg((const wchar_t*)pwStr);

			//构造字符串 角色名:说的话
			WCHAR * pwname = ConvertLPCSTRToLPWSTR(p->getRoleproperty().roleName ); 
			stringw temp="";
			temp+=pwname;
			temp+=":";
			temp+=pwStr;
			delete pwname;
			//在gui的listbox显示一行喊话
			myScene_Mgr->myGUI_Mgr->PostMsg(temp.c_str());
			delete pwStr;
			break;
			
		}
	}
	::LeaveCriticalSection(&g_cs_charlist);
//////---------------------------------------------------------------------------------------------///////////////	

}


void MyCharacterMgr::CMD_UserUPDATE_PROC(char* pData,unsigned long length )
{
	if(sizeof(RoleProperty)!=length){
		MessageBox(0,"CMD_UserUPDATE_PROC 获取大小和结构大小不一致!","",0);
	}
		RoleProperty srp;
		memcpy((char*)&srp,(char*)pData,sizeof(RoleProperty));
		//printf("CMD_UserUPDATE: name:%s sock:%d  pos:%3.1f %3.1f %3.1f \n",srp.roleName,srp.sock,srp.pos.X,srp.pos.Y,srp.pos.Z);
		
		//判断是否主角色更新
		if(character->getRoleproperty().sock==srp.sock){

			printf("--主角色更新name:%s-- hp:%d  currState:%d  \n",srp.roleName,srp.HP,srp.currState);

		//	MessageBox(0,"主角色更新","",0);
			//主角色属性更新
			character->setRoleproperty(srp);
			return;
		}
		
		
		bool found=false;//找到指定id角色
//////---------------------------------------------------------------------------------------------///////////////		
		::EnterCriticalSection(&g_cs_charlist);
		irr::core::list<MyCharacter*>::Iterator  list_oper;
		for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
		{
			
			MyCharacter *p=*list_oper;
			if(p->getRoleproperty().sock==srp.sock)
			{
				//查找标志,
				found=true;

				printf("-有客户更新-name:%s-- HP:%d currState:%d \n",srp.roleName,srp.HP,srp.currState);
				p->lastRecvtime=timer->getRealTime();
				p->setRoleproperty(srp);
				
				//根据属性立即更新
				p->UpdateByProperty();
				break;
			}
		}

		::LeaveCriticalSection(&g_cs_charlist);
//////---------------------------------------------------------------------------------------------///////////////	
		//如果有未知角色,就请求服务器信息
		if(found==false){
			printf("有未知角色,就请求服务器信息\n");
			srp.cmd=CMD_C2S_UNKNOWUSER;
			myScene_Mgr->client->SendMsg((char*)&srp,sizeof(RoleProperty));
		}

}


void MyCharacterMgr::AddCharacter(char *pData,unsigned long length )
{
	if(sizeof(RoleProperty) != length){
			MessageBox(0,"AddCharacter 获取大小和结构大小不一致!","",0);
	}

	//初始化角色属性
	RoleProperty pp;
	memset((char*)&pp,'\0',sizeof(RoleProperty));
	memcpy((char*)&pp,pData,sizeof(RoleProperty));

	//表示列表中不存在该id的怪物.
	if(  GetCharacterByID(pp.sock) ==NULL){
		printf("id:%d的角色不存在,添加! hp:%d\n",pp.sock,pp.HP);
		//	//MessageBox(0,"的怪物不存在,添加!","",0);
		printf("id:%d HP:%d\n",pp.sock,pp.HP);

		//初始化角色
		MyCharacter* c=new MyCharacter(pp,false,myScene_Mgr->myscene_loader); //最后一个参数 false 表示是配角.不是当前主角色

		//////---------------------------------------------------------------------------------------------///////////////	
		::EnterCriticalSection(&g_cs_charlist);
		char_list.push_back(c);
		::LeaveCriticalSection(&g_cs_charlist);
		//////---------------------------------------------------------------------------------------------//////////////

		c->Draw();
		//c->setPosition(pp.pos);
		c->lastRecvtime=timer->getRealTime();

	}	else{
		printf("id:%d的角色已存在或正在添加,不重复添加!\n",pp.sock);
	}


}


void MyCharacterMgr::AddMainCharacter(const char* pData,const unsigned long length)
{
	if(sizeof(RoleProperty) != length){
		MessageBox(0,"AddMainCharacter 获取大小和结构大小不一致!","",0);
	}


	//主角色已存在即已登陆,返回.
	if(character){
		
		MessageBox(0,"我靠,不会吧.又AddMainCharacter!!","",0);
		return ;
	}

    //初始化场景就
	//加载角色模型
	RoleProperty main_property;
	memset((char*)&main_property,'\0',sizeof(RoleProperty));
	memcpy((char*)&main_property,pData,sizeof(RoleProperty));

	//character=new MyCharacter(dev,"by//by.ms3d",main_property,true);
	//character=new MyCharacter(dev,"yunhe//yunhe.ms3d",main_property,true);
	character=new MyCharacter(main_property,true,myScene_Mgr->myscene_loader);
	character->Draw();
	character->setPosition(main_property.pos);
	character->SetNetServer(this->myScene_Mgr->client);


	//开始切换场景模型
	myScene_Mgr->ChangeWorld(main_property.worldMap_id);

	printf("添加完毕!主角色名%s sock:%d isDeath:%d\n",main_property.roleName, main_property.sock,main_property.isDeath	  );

}

//
MyCharacter*  MyCharacterMgr::GetCharacterByID(int sock)
{
	MyCharacter* p_char=0;
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator  list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{
		MyCharacter *p=*list_oper;
		if(p->getRoleproperty().sock==sock) 
		{
		   p_char=p;
		}
	}
	::LeaveCriticalSection(&g_cs_charlist);

	return p_char;
}

void MyCharacterMgr::update()
{
	
	//先处理服务器发来的角色消息
    DoListThread();


DWORD static startmovetime=0;


   //鼠标左键, 鼠标没指向怪物或npc 就让角色移动
  if(mouseDownL && myScene_Mgr->monster_mgr->GetNodeIsselected()==false && myScene_Mgr->npc_mgr->IsselectedNPC()==false 
	 ) 
  {
	
	  //超过500毫秒才发送
	  if(GetTickCount()-startmovetime>100){
		 startmovetime=GetTickCount();
	 
		 //  //取消攻击
		    character->CancelAttack();

		    //角色移动到目标
			
			//得到屏幕鼠标转换到3d场景的一条射线
			
			core::triangle3df triangle;
			core::line3d<f32> line;//碰撞点
			line=smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(dev->getCursorControl()->getPosition());
			//和地形碰撞
			
			const ISceneNode* outnode;
			if (smgr->getSceneCollisionManager()->getCollisionPoint(
				 			line, //[in] 射线
				 			myScene_Mgr->myscene_loader->getMeta(), //[in] 场景节点的三角形选择器
				 			line.end, //[out] 返回碰撞点
				 			triangle,outnode)) //[out] 返回碰撞的三角
			{
							vector3df target_pos;
				 			target_pos.X=line.end.X;
				 			target_pos.Z=line.end.Z;
				 			target_pos.Y=line.end.Y;
					 		 
							//判断选择的移动坐标是否在不可移动区域内,不在,则角色移动
							if(character->sceneloader->Pointisin(target_pos)){
								
							}else{
							
								//避免鼠标选择移动点和角色太近,导致动作bug
								f32 len=character->getPosition().getDistanceFrom(target_pos);
								if(len>4){
									character->Beginmove(target_pos);
								}
							}
			}


	  }


	}

  //主角色更新
  if(character)	character->update();

//////---------------------------------------------------------------------------------------------///////////////	
	//其他在线角色根据属性更新
	u32 thistime=timer->getRealTime();
	::EnterCriticalSection(&g_cs_charlist);
		irr::core::list<MyCharacter*>::Iterator  list_oper;
   		for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
		{
				
			    MyCharacter *p=*list_oper;

				if(p) 
				{
					//先标志未鼠标指向角色
					p->mouseCurCharacter=false;
					
					p->Onlyupdate();

					////待加载模型
					//if(p->ModelState==1){
					//	p->Draw();
					//}else if(p->ModelState==3){
					//	//待删除模型
					//	p->drop();
					//	delete p;
					//	p=0;
					//	char_list.erase(list_oper);
					//	break;
					//}
					//else 
					  if(thistime-p->lastRecvtime >5000  || p->getRoleproperty().worldMap_id!=character->getRoleproperty().worldMap_id) {
						//超过5秒没响应,就认定角色退出了
					//	p->drop();
					//	delete p;
					//	p=0;
					//	char_list.erase(list_oper);
					//	break;
					}
					
				}
		}
	 ::LeaveCriticalSection(&g_cs_charlist);
//////---------------------------------------------------------------------------------------------///////////////	

	 scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
	 //得到屏幕鼠标转换到3d场景的一条射线
	 vector2d<s32> curpos=dev->getCursorControl()->getPosition();
	 core::line3d<f32> line;
	 line=smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(curpos);
	 //方法2.
	 core::vector3df intersection;
	 core::triangle3df hitTriangle;

	 //拾取其他在线角色
	 ISceneNode * select_char_node =collMan->getSceneNodeAndCollisionPointFromRay(
		 line,
		 intersection, // This will be the position of the collision
		 hitTriangle, // This will be the triangle hit in the collision
		 0, // This ensures that only nodes that we have
		 // set up to be pickable are considered
		 0); // Check the entire scene (this is actually the implicit default)

	 //拾取了角色
	 if(select_char_node){
		//printf("拾取%d!\n",select_char_node->getID() );
		 //根据节点得到怪物类
		 MyCharacter* sel_char=GetCharacterByID(select_char_node->getID());
		 if(select_char_node->getID() == GetMainCharacter()->getRoleproperty().sock){
			// printf("拾取了主角色!\n");
			return;
		 }
		 
		 if( sel_char==NULL ){
			
		 }else {

			 //标志这个角色类被拾取了.他将会显示血条
             sel_char->mouseCurCharacter=true;
			//  printf("拾取了角色%s \n", sel_char->getRoleproperty().roleName );
		 }
	 }else{
		// printf("拾取失败!!!!\n");
	 }
}


//角色ui渲染
void MyCharacterMgr::UIRender()
{
	if(character) character->UIRender();

//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator  list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{

		MyCharacter *p=*list_oper;
		if(p) p->UIRender();
	}
	::LeaveCriticalSection(&g_cs_charlist);
//////---------------------------------------------------------------------------------------------///////////////	
}

//移出所有在线角色.用于切换场景时的.
void MyCharacterMgr::RemoveAllCharacter()
{
	//////---------------------------------------------------------------------------------------------///////////////	
	::EnterCriticalSection(&g_cs_charlist);
	irr::core::list<MyCharacter*>::Iterator  list_oper;
	for(list_oper=char_list.begin();list_oper!=char_list.end();++list_oper)
	{

		MyCharacter *p=*list_oper;
		if(p){
			p->drop();
			delete p;
			p=0;
			
		}
	}
	char_list.clear();

	::LeaveCriticalSection(&g_cs_charlist);
	//////---------------------------------------------------------------------------------------------///////////////	
}

void MyCharacterMgr::OnEvent (const irr::SEvent& e)
{ 
	//主角色接收事件
	if(character) character->OnEvent(e);

		if(e.EventType==EET_MOUSE_INPUT_EVENT)
		{
			 if(e.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)   mouseDownL = true;
		   
			 if(e.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)      mouseDownL = false; 
		}



}
