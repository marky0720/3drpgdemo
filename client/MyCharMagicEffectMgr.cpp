#include ".\mycharmagiceffectmgr.h"

namespace Magic
{


MyCharMagicEffectMgr::MyCharMagicEffectMgr(void)
{
}

MyCharMagicEffectMgr::~MyCharMagicEffectMgr(void)
{
}

//添加一个魔法特效
void MyCharMagicEffectMgr::AddMagic(int e ,ISceneNode* parenetNode,vector3df pos,float speed,vector3df rot,ITexture* tex )
{
	if(e==FollowCircle){
		Magic::IMyCharMagicEffect* m=new Magic::MyCharMagicEffect1();
		m->init(parenetNode,pos,speed);

		magic_list.push_back(m);
	}else if(e==FollowPlane){
			Magic::IMyCharMagicEffect* magic=new Magic::MyCharMagicEffect2(rot);
		magic->init(parenetNode,pos,speed,tex);
		magic_list.push_back(magic);
	}

}

//特效更新
void MyCharMagicEffectMgr::update()
{
	irr::core::list<Magic::IMyCharMagicEffect*>::Iterator  list_oper;
	for(list_oper=magic_list.begin();list_oper!=magic_list.end();++list_oper)
	{
		Magic::IMyCharMagicEffect* magic=*list_oper;
		magic->update();
	}

}


//特效删除
void MyCharMagicEffectMgr::drop()
{
	irr::core::list<Magic::IMyCharMagicEffect*>::Iterator  list_oper;
	for(list_oper=magic_list.begin();list_oper!=magic_list.end();++list_oper)
	{
		Magic::IMyCharMagicEffect* magic=*list_oper;
		magic->drop();
		delete magic;
		magic=0;
	}

	magic_list.clear();

}



} // end namespace