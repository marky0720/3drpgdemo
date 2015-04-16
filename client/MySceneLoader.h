#pragma once

#include "stdafx.h"

#include "MysceneSnow.h"

#include "MyNPC.h"

#include "MysceneGrass.h"

#include ".\myscenevolumelight.h"



/************************************************************************/
// 
//		场景加载类  根据场景配置文件加载场景
//		
//
/************************************************************************/

class MySceneLoader
{
public:
	MySceneLoader();
	~MySceneLoader(void);

	//加载
	bool LoadScene(int id ,ICameraSceneNode* parentNode);

	//得到地形高度
	float getHeight(float x,float z);

	//卸载
	void DropCurrScene();

	void update();

	//读取地图可移动区域文件
	void ReadLevelCollisionFile();

	irr::core::list<irr::core::triangle3df*> mylist;  //list存选取的三角形

	scene::IMetaTriangleSelector * getMeta(){return meta;};

	void CalculMeta();

	void RemoveTerrain();


	//所有不可移动区域的三角形,根据一点是否在内部来判断
	bool Pointisin(core::vector3df test );
	//角色可以移动到pos吗
	bool canmovehere(vector3df pos);



	//设置加载的场景是否可见
	void setVisiable(bool v);

private:

	//引擎核心
	irr::IrrlichtDevice *dev; 
	irr::video::IVideoDriver *drv; 
	irr::scene::ISceneManager *smgr; 


	//存储角色中心点.这里使用camera.因为camera跟随角色. gettarget就可以得到角色坐标,getposition就可以得到摄象机坐标.比一个角色node方便
	ICameraSceneNode* cameraNode;

	//当前世界地图id
	int currWorldMap; //
	
	float height;

	//若用.irr场景就要用meta
	scene::IMetaTriangleSelector * meta;

	MySceneVolumeLight* light1;
	MySceneSnow*  Snow; //下雪类
	MySceneGrass* grass;
	scene::ISceneNode* skydome;


	MyNPC* m_npc; 
};
