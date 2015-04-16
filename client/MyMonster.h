

//************************************************************************/
// 
//		怪物类
//		
//
/************************************************************************/


#pragma once

#include "NetClient.h"

#include "VxHealthSceneNode.h"

#include "mycharattacknumtextshow.h" //被攻击显血的

#include<time.h> //使用随机数




#include "MyCharacter.h"
class MyCharacter   ;

class MyMonster : public irr::scene::IAnimationEndCallBack  
{
public:



	virtual void OnAnimationEnd(IAnimatedMeshSceneNode* node) ;

	~MyMonster(void);

	MyMonster( MonsterProperty prop,const c8* filename);

	void SetPosition(core::vector3df  pos); 
	void SetScale(float  scale);

	void SetMonsterId(int id){prop.ID=id;};
	int	 GetMonsterId(){return prop.ID;};

	bool IsDead(){return prop.isDeath;};

	//怪物被攻击显示的数字
	void ShowAttackTextNum(int num);

	//网络
	Socket::CNetClient* cnet;
	//网络,发送给服务器的
	void SetNetServer(Socket::CNetClient* net)
	{
		cnet=net;
	};

	void Draw();
	void UIRender();
	void drop();
	void Update();
	void UnderAttack(unsigned int attack);	//怪物遭受攻击,参数是攻击值
	void attack();
	void SetState(int s);

	void SetInitRotValue(float rot);
	scene::IAnimatedMeshSceneNode* GetNode(){return node;};


	void AddGhostFlyEffect();

	MonsterProperty* getMonsterProperty(){return &prop;};


	void setMonsterProperty(MonsterProperty p);

	void setMonsterHP(int hp){prop.HP=hp;};

	void setMonsterCMD(char cmd){prop.cmd=cmd;};

	void setMonsterDamage(int damage){prop.damage=damage;};


	 VxHealthSceneNode* getHPBar(){return m_pgHealthBar;}; //得到hp血条

	 core::line3d<f32> line;//碰撞点

	 bool isdrop; //标志是否这个怪物已调用了drop
private:
	IrrlichtDevice *device;						//引擎指针c
	scene::ISceneManager* smgr;					//场景管理

	core::vector3df Node_scale;					//缩放比例

	scene::IAnimatedMeshSceneNode* node;		//场景节点
	

	MonsterProperty prop;

	const c8* file;							


	float movespeed;	//移动速度

	float yrot;				//怪物绕y轴旋转角度



	ITimer* timer;
	u32 lasttime;
	u32 lastattacktime;

	int lastState;//最后的状态

	irr::scene::VxHealthSceneNode* m_pgHealthBar;  //显示hp

	float getradian(float x,float y);
};
