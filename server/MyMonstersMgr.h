

/*
* ������������
*/

#if !defined(_H____INCLUDED_)
#define _H____INCLUDED_

#include "stdafx.h"


#include "mysockserver.h"


#include "mymonster.h"

#include "myitemmgr.h"

class MyMonstersMgr
{
public:
	MyMonstersMgr(MyRoleMgr* role_mgr ,Socket::MySOCKServer* ser ,MyItemMgr* m_pItem_mgr);
	~MyMonstersMgr(void);

	//��ȡ���й�����Ϣ
	void ReadAllMonstersInfo();

	//���س�����ͼˢ���ļ�
	void ReadSceneMonster();

	SMonsterInfo* getMonsterBaseInfoByTypeID(int type);



	void AddMonster(int TypeID,int ID,vector3df pos ,int worldmap);

	MyMonster* GetMonsterByID(int id);

	//�õ����й�������
	int getAllMonsterNum();

	//�õ��������������ͼ
	int getMonsterWorldMap(int monsterID);

	void Update();

	//����������н�ɫ,���Ƿ���������.�ھͷ��ؽ�ɫָ��
	RoleProperty* GetNearestRole(MyMonster* m);
	//���ݹ���id�ù��ﱻ����
	MyMonster*  MyMonsterUnderAttack(int ID,int damage,SOCKET attacker);

	MyMonster* GetMonster(ISceneNode* node);




	void SendAMonsterToClient(SOCKET sock,int monsterID , int worldMap);
	void SendAllMonsterListToClient(SOCKET sock  , int worldMap);

	double getRandNum(int a,int b)
	{
		struct _timeb timebuffer;//����һ���ṹ
		_ftime(&timebuffer);//��ʼ��timebuffer
		srand(timebuffer.millitm);//timebuffer.millitm��ú���

		int RANGE_MIN = a;
		int RANGE_MAX = b;

		double rand100 = (((double) rand() /(double) RAND_MAX) * RANGE_MAX + RANGE_MIN);
		return rand100;
	};

	int count;



private:
	MyItemMgr* m_pItem_mgr;

	MyRoleMgr* role_mgr ;



	//ȫ�ֹ����ļ�������Ϣ
	std::list<SMonsterInfo*> monsters_info_list; //��ȫ�ֹ����ļ�������Ϣ

	//����list����
	void MonsterListLock();

	//����list����
	void MonsterListUnLock();

	//�����������
	std::vector<MyMonster*> monster_list; //�������Ϣ

	CRITICAL_SECTION g_cs_monsterlist; //�ٽ�


	//����
	Socket::MySOCKServer* server;



	u32 lasttime;


};

#endif