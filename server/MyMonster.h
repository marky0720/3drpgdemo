/*

怪物类,显示怪物

*/


#pragma once

#include "stdafx.h"


#include<time.h> //使用随机数

#include "mysockserver.h"



class MyMonster
{
public:

	~MyMonster(void);

	MyMonster(MonsterProperty  p,	SMonsterInfo* sm,	Socket::MySOCKServer* server ,int  worldmap);

	
	//设置怪物id
	void SetId(int id){prop.ID=id;};

	int	 GetId(){return prop.ID;};

	bool IsDead(){return prop.isDeath;};

	//怪物重生
	void reborn();
	
	bool CharacterIsInMonsterView();
	bool CharacterIsInMonsterAttackRect();
	void RandMove();

	//返回true表示怪物超过了允许的最大移动范围
	bool MyMonster::CheckMax_move_distense();


	void SetCharacter(RoleProperty* role);

	void Update();

	int UnderAttack( int attack ,SOCKET attacker );	//怪物遭受攻击,参数是角色的攻击力,返回实际受到的伤害值

	void attack();

	void SetState(int s);

	void SetInitRotValue(float rot);

	void characterUnderAttack(int attack);

	RoleProperty* getRolePointer(){return role;};

	MonsterProperty getMonsterProperty(){return prop;};

	void setMonsterHP(int hp){prop.HP=hp;};

	int getMonsterMaxHP(){return prop.MAX_HP;};

	void setMonsterCMD(char cmd){prop.cmd=cmd;};

	void setMonsterDamage(int damage){prop.damage=damage;};

	//怪物基本信息
	SMonsterInfo m_info;

	//怪物死亡时间
	DWORD dwDieTime;

	//是否重生标志
	bool m_breborn;
private:
	float getradian(float x,float y);
	bool MoveTarget(vector3df target_pos);
	void FaceToTarget(vector3df target_pos);

	//网络
	Socket::MySOCKServer* server;

	//怪物属性
	MonsterProperty prop;

	//怪物初始化的点.中心点
	vector3df center_pos;





	//怪物攻击的角色属性
	RoleProperty* role;

	vector2df orient; //面对的向量

	bool isArrived;	//是否已到达

	float movespeed;	//移动速度

	float yrot;				//怪物绕y轴旋转角度

	//时间间隔
	DWORD dwStart,dwEnd;

	DWORD attackStart,attackdwEnd;


};
