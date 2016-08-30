/*

������,��ʾ����

*/


#pragma once

#include "stdafx.h"


#include<time.h> //ʹ�������

#include "mysockserver.h"



class MyMonster
{
public:

	~MyMonster(void);

	MyMonster(MonsterProperty  p,	SMonsterInfo* sm,	Socket::MySOCKServer* server ,int  worldmap);

	
	//���ù���id
	void SetId(int id){prop.ID=id;};

	int	 GetId(){return prop.ID;};

	bool IsDead(){return prop.isDeath;};

	//��������
	void reborn();
	
	bool CharacterIsInMonsterView();
	bool CharacterIsInMonsterAttackRect();
	void RandMove();

	//����true��ʾ���ﳬ�������������ƶ���Χ
	bool MyMonster::CheckMax_move_distense();


	void SetCharacter(RoleProperty* role);

	void Update();

	int UnderAttack( int attack ,SOCKET attacker );	//�������ܹ���,�����ǽ�ɫ�Ĺ�����,����ʵ���ܵ����˺�ֵ

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

	//���������Ϣ
	SMonsterInfo m_info;

	//��������ʱ��
	DWORD dwDieTime;

	//�Ƿ�������־
	bool m_breborn;
private:
	float getradian(float x,float y);
	bool MoveTarget(vector3df target_pos);
	void FaceToTarget(vector3df target_pos);

	//����
	Socket::MySOCKServer* server;

	//��������
	MonsterProperty prop;

	//�����ʼ���ĵ�.���ĵ�
	vector3df center_pos;





	//���﹥���Ľ�ɫ����
	RoleProperty* role;

	vector2df orient; //��Ե�����

	bool isArrived;	//�Ƿ��ѵ���

	float movespeed;	//�ƶ��ٶ�

	float yrot;				//������y����ת�Ƕ�

	//ʱ����
	DWORD dwStart,dwEnd;

	DWORD attackStart,attackdwEnd;


};
