#pragma once

#include "stdafx.h"


/************************************************************************/
// 
//		场景的草
//		
//
/************************************************************************/


class MySceneGrass
{
public:
	MySceneGrass( int worldMap);
	~MySceneGrass(void);

	void DrawAllGrass();
	void drop();

	void setVisible(bool v);

private:
	IrrlichtDevice* device;
	irr::core::list<irr::core::vector3df> mylist;  //list存要刷的草坐标

	irr::core::list<irr::scene::IAnimatedMeshSceneNode*> mygrass_list; //存储所有grass

	void ReadFile();
	void DrawAGrass(vector3df pos);

	//草地是在哪个世界地图的
	int worldMap;

	//保存草地模型路径
	char grass_model_path[255];
	//草地分布在世界的列表文件路径
	char grass_list_path[255];

	bool m_bUseGrass; //地图是否使用了草地
};
