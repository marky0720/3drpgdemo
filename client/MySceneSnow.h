#pragma once


#include<stdlib.h> 
#include<stdio.h> 
#include<time.h>

/************************************************************************/
// 
//		下雪效果类  
//					//初始化下雪	Snow=new MySnowEffect(dev,camera);
//						更新方向			Snow->Update();
//
/************************************************************************/


class MySceneSnow
{
public:
	MySceneSnow(ICameraSceneNode* cam);
	~MySceneSnow(void);
	
	void drop();

	void Update( );

	void setVisible(bool v);

private:

	irr::IrrlichtDevice *device;
	scene::IParticleSystemSceneNode* ps ;//粒子系统

	//摄象机 打算雪始终朝摄象机的面向目标吹
	ICameraSceneNode* cam;

	////创建盒子发射器
	scene::IParticleEmitter* em;

	

	float x_offset,y_offset;//
	float snow_downspeed;
};
