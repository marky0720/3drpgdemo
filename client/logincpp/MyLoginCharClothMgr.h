#pragma once

#include "../stdafx.h"


/************************************************************************/
// 
//		登陆时的 角色服装模型管理器 将角色服装分3部分 上身+手臂 裤子 鞋子
//		
//
/************************************************************************/
namespace Login
{

class MyLoginCharClothMgr
{
public:
	MyLoginCharClothMgr( irr::IrrlichtDevice * device,int id ,int charType); //1是角色sock 标识 2是角色类型 男 女

	~MyLoginCharClothMgr(void);

	void setClothVisible(bool v);

	//加载上身和双臂
	bool LoadBody(int id);

	//加载裤子
	bool LoadTrousers(int id);

	//加载围腰
	bool LoadDress(int id);

	//加载鞋子
	bool LoadShoes(int id);

	//设置主body的属性
	void setPosition(vector3df pos);
	void setRotation(vector3df rot);
	vector3df getPosition( );
	vector3df getRotation( );
	vector3df getAbsolutePosition();

	//做动作
	void Animate(int speed,int start,int end ,bool loop);

	//得到角色主node
	IAnimatedMeshSceneNode* getRootNode(){return root;};

	//卸载所有角色的装备模型
	void DropAll();


private:
	
	//角色唯一的标志
	int char_id;

	//角色性别
	int charSex;

	char modelPath[255];

	//根节点
	IAnimatedMeshSceneNode* root;

	//角色主node
	IAnimatedMeshSceneNode* body;     


	//裤子
	IAnimatedMeshSceneNode* trousers;

	//鞋子
	IAnimatedMeshSceneNode* shoes;

	//围裤子的围腰
	IAnimatedMeshSceneNode* dress;

	//保存当前动作的关键帧 参数,用于再次加载其他模型时读取
	int currStart,currEnd,speed;
	bool m_bloop;

	//引擎core对象
	IrrlichtDevice* dev;					
	ISceneManager* smgr;
};


}