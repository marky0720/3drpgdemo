#pragma once


#include "myweapon.h"



/************************************************************************/
// 
//		武器管理类.. 由角色内部使用.由于要和角色类相互访问.所以在角色类h一起定义
//		
//
/************************************************************************/



class MyWeaponMgr
{
public:
	MyWeaponMgr(IAnimatedMeshSceneNode* parentNode);
	~MyWeaponMgr(void);

	//卸载所有武器
	void DropAll();

	//切换武器
	void ChangeWeapon(int id,int param1);

	//根据角色状态更新武器显示
	void update(int state );
	void updateUseLastState();

	void setWeaponVisible(bool v);

	//为当前武器使用hlsl效果
	void OpenHlslEffect(bool use,int weaponType);

	//查询是否已使用hlsl效果
	bool IsOpenHlslEffect(){return isusehlsleffect;};

	MyWeapon* getHandWeapon(){return handweapon;};
	MyWeapon* getBackWeapon(){return backweapon;};


	int getCurWeaponId(){return currentWeaponid;};

	void MyWeaponMgr::setParentNode(IAnimatedMeshSceneNode* p);
private:
	IrrlichtDevice* device;
	irr::video::IVideoDriver *drv; 
	ISceneManager* smgr ;

	IAnimatedMeshSceneNode* parentNode;

	//保存角色状态
	int state;
	//武器编号
	int currentWeaponid;
	int currparam1;

	MyWeapon* handweapon;//手
	MyWeapon* backweapon;//背

	bool isusehlsleffect; //是否已使用hlsl效果

};
