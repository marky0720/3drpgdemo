#pragma once
#include "stdafx.h"
#include "MyCharacter.h"

#include "Myscenemanager.h"




/************************************************************************/
// 
//		角色管理器.管理主角色和其他在线角色.
//		
//
/************************************************************************/


class MySceneManager;

class MyCharacterMgr
{
public:
	MyCharacterMgr(MySceneManager* mm );
	~MyCharacterMgr(void);

		//接收事件
	void OnEvent (const irr::SEvent & e);

	//主角色登陆
	void CMD_USERLOGIN_PROC(const char* pData,const unsigned long length);

	//添加其他角色
	void AddCharacter(char* pData,unsigned long length);

	//移出所有在线角色.用于切换场景时的.
	void RemoveAllCharacter();

	//设置所有角色武器显示?
	void setAllRoleWeaponVisible(bool v);

	void setAllRoleClothVisible(bool v);


	//得到主角色
	MyCharacter*  GetMainCharacter(){return character;};
	void  SetMainCharacter(MyCharacter* c){character=c;};

	//根据sock得到角色 sock是角色的唯一标志
	MyCharacter*  GetCharacterByID(int sock);
	
	void RemoveCharacter(int id);
	void update();

	void UIRender();

	//得到角色list数
	int getOnlineCharacterNum(){return char_list.getSize();};

	//向服务器请求所有在线角色
	void getOnlineAllCharacter();

	//有角色的网络事件到达
	void insertCharData(const char* pData ,const int DataLength);

	//得到charlist指针
	irr::core::list<MyCharacter*>* getCharListPointer(){return &char_list;};

	void Quit(){m_bquit=true;};

private:
	//添加主角色
	void AddMainCharacter(const char* pData,const unsigned long length);

	//主角色包裹更新
	void CMD_S2C_Response_Package_PROC(char* pData,unsigned long length);
	//服务器要求角色切换地图
	void CMD_S2C_Response_ChangeWorld_PROC(char* pData,unsigned long length);
	
	//在线其他角色换武器
	void CMD_Response_ChangeWeapon_PROC(char* pData,unsigned long length);
	//角色被攻击消息
	void CMD_USERUNDERATTACK_PROC(char* pData,unsigned long length);
	//角色金钱更新消息
	void CMD_USERMoneyUpdate_PROC(char* pData,unsigned long length);
	void CMD_QUIT_PROC(char* pData,unsigned long length);


	void CMD_NEWUSER_PROC(char* pData,unsigned long length);
	void CMD_UserUPDATE_PROC(char* pData,unsigned long length );
	void CMD_USERMSG_PROC(char* pData,unsigned long length );

	void CMD_ServerResponse_Prop_PROC(char* pData,unsigned long length);

	//处理服务器发来的角色消息
	void  DoListThread( );

	irr::core::list<ClientMsg*> msglist;    //存server传来的消息链,更新客户信息的

	CRITICAL_SECTION g_cs_charlist,g_cs_msglist; //临界

	irr::core::list<MyCharacter*> char_list;  //list存当前其他人角色


	MyCharacter* character; //主角色


	//引擎指针
	IrrlichtDevice *dev;		

	//场景管理
	scene::ISceneManager* smgr;
	
	//
	MySceneManager* myScene_Mgr;

	bool mouseDownL;


	ITimer* timer;

	bool m_bquit; //退出
};
