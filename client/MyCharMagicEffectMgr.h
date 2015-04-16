#pragma once
#include "stdafx.h"

#include "MyCharMagicEffect.h"
#include "MyCharMagicEffect2.h"


namespace Magic
{

/************************************************************************/
// 
//		特效管理器.添加特效,删除特效,
//		
//
/************************************************************************/


#define FollowCircle 1  //跟随角色的粒子 绕圈的
#define FollowPlane 2   //跟随角色的面.  只自旋转的.




class MyCharMagicEffectMgr
{
public:
	MyCharMagicEffectMgr(void);
	~MyCharMagicEffectMgr(void);

	//添加一个魔法特效
	void AddMagic(int effect,ISceneNode* parenetNode,vector3df pos,float speed,vector3df rot ,ITexture* tex);

	void update();

	void drop();


private:

	irr::core::list<IMyCharMagicEffect*> magic_list;//保存魔法特效类


};

}// end namespace
