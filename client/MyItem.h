#pragma once

#include "stdafx.h"


/************************************************************************/
// 
//		 场景地面掉落物品类.加载模型显示在场景里的.并显示物品名称
//		
//
/************************************************************************/



class MyItem
{
public:
	MyItem(  char* meshPath,SceneItem item);
	~MyItem(void);
void drop();
void Draw();

void setPosition(core::vector3df pos){node->setPosition(pos);}; 
vector3df getPosition(){return node->getPosition();};
void setRotation(core::vector3df rot){node->setRotation(rot);}; 
vector3df getRotation(){return node->getRotation();};
void setScale(vector3df scale){node->setScale(scale);};

void UIRender();


	core::line3d<f32> line;//碰撞点


public:

	scene::IAnimatedMeshSceneNode* node ;
	 scene::ITriangleSelector* selecter;
	 IrrlichtDevice* dev;
	irr::scene::ISceneManager *smgr;

	
	stringc modelPath;

	SceneItem item;

private:

	vector2di	TextShowPos;

};
