#pragma once

#include "MyHLSLEffect.h"

/************************************************************************/
// 
//		 武器类
//		
//
/************************************************************************/


class MyWeapon
{
public:
	MyWeapon( );
	~MyWeapon(void);
	void drop();

	IAnimatedMeshSceneNode* getNode(){return weaponNode;};
	void setNode(IAnimatedMeshSceneNode* n){weaponNode=n;};
	void ChangeWeapon(int id);
	void setPosition(core::vector3df pos){weaponNode->setPosition(pos);}; 
	void HLSL();//使用hlsl
	s32 newMaterialType1 ;//hlsl材质
	s32 oldMaterialType; //保存旧武器材质
	void setUseHLSL(bool b,bool transparentMeta , int weaponType); //设置是否使用hlsl weaponType表示武器的流动纹理
	bool getUseHLSL(){return usehlsl;}; //得到是否使用hlsl
	void AddLight();


	void useBillboard(void);

	MyHLSLEffect* getEffect(){return effect;};

	void useParticleEffect();

	void update();

		scene::IParticleSystemSceneNode* ps ;//粒子系统
private:
	IrrlichtDevice* device;
    irr::video::IVideoDriver *drv; 
	ISceneManager* smgr ;

	scene::IAnimatedMesh* weaponmesh;						//武器mesh
	scene::IAnimatedMeshSceneNode* 	weaponNode;				//武器节点
	int weaponType; //武器效果类型

MyHLSLEffect* effect; //效果类

	bool usehlsl;


	////创建盒子发射器
	scene::IParticleEmitter* em;

};
