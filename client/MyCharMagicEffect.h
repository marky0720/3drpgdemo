#pragma once
#include "stdafx.h"


#include "IMyCharMagicEffect.h"


/************************************************************************/
// 
//		魔法效果.围绕角色的粒子
//		
//
/************************************************************************/


namespace Magic
{

class MyCharMagicEffect1  : public IMyCharMagicEffect
{
public:
	MyCharMagicEffect1();
	~MyCharMagicEffect1(void);

	void update();

	bool init( ISceneNode* parentNode,vector3df pos, float speed,ITexture* tex);

	void drop();

	void setParentNode(ISceneNode* parentNode);



	void OnSetConstants(video::IMaterialRendererServices* services, s32 userData);

	s32 get();

	ISceneNode* parentNode;
private:

	float RandNum2(float  begin,float end,float vlaue);


	//引擎core对象
	IrrlichtDevice* dev;	
	irr::video::IVideoDriver *drv; 
	irr::gui::IGUIEnvironment* ui;
	irr::scene::ISceneManager *smgr; 

	scene::IParticleSystemSceneNode* ps ;//粒子系统


	scene::IAnimatedMeshSceneNode* node;
	vector3df pos;
	 ITexture* tex;
	 float speed;

	float num;
	float mAlpha;




};


} // namespace Magic