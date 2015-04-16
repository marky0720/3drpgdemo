#pragma once






#include "VxHealthSceneNode.h"

#include "mycharheadermsg.h" //显示喊话的

#include "MyMonster.h"

#include "NetClient.h"

#include "mycharclothmgr.h"

#include "mysceneloader.h"


#include "MyWeaponMgr.h"   //武器

#include "MyBall.h"

#include "MyCharMagicEffectMgr.h"

/************************************************************************/
// 
//		角色类 游戏里的主角色和其他在线角色都用此类
//		
//
/************************************************************************/




#define ANIMATION_SPEED 11

class MyMonster   ;
class MyWeaponMgr;

class MyCharacter : public irr::scene::IAnimationEndCallBack  
{
public:
	  
	
	MyCharacter( RoleProperty p,bool isMaincharacter ,MySceneLoader* sceneloader); 
	

	//加载角色/武器模型,关联的节点
	void Draw();

	~MyCharacter(void);

    enum eAttacktype{ 
                 MagicAttack,  //魔法攻击
                 PowerAttack,  //物理攻击
    }; 

	//当前场景
	MySceneLoader* sceneloader;

	//保存攻击类型
	eAttacktype curAttackType; 

	//角色服装模型管理器
	MyCharClothMgr* m_charClothMgr; 

	//武器管理器
	MyWeaponMgr* m_weapon_mgr;
	//得到当前武器id
	int getCurWeapon(){return m_weapon_mgr->getCurWeaponId();};

	//设置当前拿的武器,并向服务器请求更新武器
	void setCurWeapon(int weaponid,int weaponparam1){
		m_weapon_mgr->ChangeWeapon(weaponid,weaponparam1);
	};


	//设置世界地图
	void MyCharacter::SetWorldMap(int worldMap);

	//角色接收处理自己的消息
	void OnEvent (const irr::SEvent& e); 

	//角色被攻击显示的数字
	void ShowAttackTextNum(int num);

	//用户喊话
	void PostMsg(const wchar_t* msg);



	//卸载
	void drop();

	//ui绘制
	void UIRender();

	//跑步动作.
	void run(bool checkState);

	void stop()
	{
		isArrived=true;
		idle(true);
	}

	void OneTimeDie();
	bool isDieAnimation;//已经执行死亡倒地动作了

	//改变跑步/走模式
	void ChangeRunMode();

	//得到血百分比
	f32 getHPper(){return f32 ((float)property.HP/(float)property.MAX_HP);};

	//攻击动作
	void attack(vector3df attackpos,eAttacktype e); 

	//站立动作
	void idle(bool checkState);  

	//攻击预备姿势
	void readyAttack(bool checkState); 

	//取消攻击
	void CancelAttack();

	//改变状态攻击预备/战立
	void ChangeState();

	//得到角色头上hp条
	VxHealthSceneNode* getHPBar(){return m_pgHealthBar;};

	//角色已死?
	bool isDeath(){return property.isDeath;};

	//角色遭受攻击,参数是伤害值
	void UnderAttack( int damage);

	//设置要攻击的怪物
	void SetWantToAttackTarget(MyMonster* m){monster=m;isfollow=true;};
	//跟踪怪物
	void FollowAndAttackTarget();
	MyMonster* monster;//想攻击的怪物
	bool isfollow; //是否在跟踪?
	//怪物是否在角色攻击范围内
	bool getDistenceFromMonster(ISceneNode* monster);



	irr::scene::IAnimatedMeshSceneNode* getNode(); 
	core::vector3df faceTarget(irr::core::vector3df targetpos);
	void setPosition(core::vector3df pos); 
	void setRotate(core::vector3df rot); 
	core::vector3df getPosition(); 
	core::vector3df getRotate(); 

	int getState();
	void setState(int newState);

	//得到要移动到的坐标
	vector3df getTarget(){return property.target;};

	//开始移动
	void Beginmove(core::vector3df pos);

	//更新
	void update(); 
	void MyCharacter::Onlyupdate() ;





	//-----------------------------------------------------------------------------------
	//这段是用于显示其他在线角色的.而不是用于当前主角色
	bool runstate;
	bool lastRunmode;
	void setRunAnimation( )
	{
		if(runstate==false ){
			runstate=true;			////模式未改变
		}else if(runstate && lastRunmode==property.RunMode){
			return;
		}else if(lastRunmode!=property.RunMode){
			lastRunmode=property.RunMode;
		}
		//跑?走
		if(property.RunMode){
			fSpeed = 0.7f; //速度
			m_charClothMgr->Animate(ANIMATION_SPEED+2,61,79,true);

		}else{
			fSpeed = 0.350f; //速度
			m_charClothMgr->Animate(ANIMATION_SPEED+1,1,19,true);
		}
	}
	//--------------------------------------------------------------------


	//动作完成的callback
	virtual void OnAnimationEnd(IAnimatedMeshSceneNode* node) ;

	//网络
	Socket::CNetClient* cnet;

	//是否是主角色
	bool isMain;

	//网络,发送给服务器的
	void SetNetServer(Socket::CNetClient* net)
	{
		cnet=net;
	};

	//更新角色信息到服务器 CMD_UserUPDATE
	void ReoprtToServer( );



	void setHP(int hp)
	{
		property.HP=hp;
	};

	int getMoney()
	{
		return package.money;
	};

	void setMoney(int moeny)
	{
		package.money=moeny;
	};

	//只更新角色属性面版属性.
	void setPropPlane(RoleProperty* rr);

	//角色加属性点.向服务器发送
	void AddPoint(int proptype);


	//捡地面上掉落物品,或金币
	void CollectDropItem();

	RoleProperty getRoleproperty(){return property;};
	void setRoleproperty(RoleProperty pp){
		memcpy((char*)&property,(char*)&pp,sizeof(RoleProperty));
	};

	//角色根据属性立即更新
	void UpdateByProperty();


	ITimer* timer;
	u32 lasttime;
	u32 lastattacktime;

	u32 lastRecvtime; //最后一次接收到角色信息的时间


Package* getPackage()
{
	return &package;
};

void setPackage(Package p)
{
	memcpy((char*)&package,(char*)&p,sizeof(Package));
};

//鼠标指向了角色
bool mouseCurCharacter;
     

float getMoveSpeed (){ return fSpeed;};

 private: 

	 MyBall* ball;
	 Magic::MyCharMagicEffectMgr* m_pMagic_Mgr;
	 //Magic::IMyCharMagicEffect* magic3;
	 //Magic::IMyCharMagicEffect* magic;
	 //Magic::IMyCharMagicEffect* magic2;

	 //显示角色名字的text 在角色模型下面
	 ITextSceneNode* char_textnode;

	 irr::scene::VxHealthSceneNode* m_pgHealthBar;  //显示hp

	 RoleProperty property; //角色属性
	 Package package; //角色包裹

	 //角色移动速度
	 float fSpeed;


	 //引擎core对象
	 IrrlichtDevice* dev;						
	 ISceneManager* smgr;


	 int laststate;//最后的状态


protected:
	void move();  //仅自己内部调用
	bool isArrived;			//是否已到达目的地


	//角色移动
	void moveto(irr::scene::ISceneNode *node, //node to move 
		irr::core::vector3df vel); //velocity vector 


};
