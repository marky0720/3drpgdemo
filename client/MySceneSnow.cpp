#include "StdAfx.h"
#include ".\myscenesnow.h"

MySceneSnow::MySceneSnow( ICameraSceneNode* c)
{
	device=getIrrDevice();
	cam=c;

	x_offset=0;
	y_offset=0;

	snow_downspeed=-0.16f;

		// 创建 粒子系统	
	
		ps = device->getSceneManager()->addParticleSystemSceneNode(false); //创建一个粒子系统,并使用自定义的发射器
		ps->setPosition(c->getPosition()+vector3df(0,50,0));//位置
		//ps->setScale(core::vector3df(2,2,2));//缩放
		ps->setParticleSize(core::dimension2d<f32>(1.0f, 2.0f));//设置粒子大小
		////创建盒子发射器
		 em = ps->createBoxEmitter(
			core::aabbox3d<f32>(-400,-400,-400,400,400,400),//设置盒子的6个顶点
			core::vector3df(0.0f,snow_downspeed,0.0f),//设置发射方向和速度
			500,700,//每秒最小粒子数80,最大100
			video::SColor(0,0,0,0), video::SColor(0,255,255,255),//开始颜色和结束颜色
			1000,1000);//存活时间,800-2000毫秒
		ps->setEmitter(em);//粒子系统设置发射器
		em->drop();

		scene::IParticleAffector* paf =ps->createFadeOutParticleAffector();
		ps->addAffector(paf);
		paf->drop();

		ps->setMaterialFlag(video::EMF_LIGHTING, false);
		ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
		ps->setMaterialTexture(0, device->getVideoDriver()->getTexture("textures/flare.bmp"));
		ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
}

MySceneSnow::~MySceneSnow(void)
{
}

void MySceneSnow::setVisible(bool v)
{
   ps->setVisible(v);
}

/*
  更新下雪状态,比如下雪方向,下雪的中心位置
 */
void MySceneSnow::Update()
{
	
	ps->setPosition(cam->getPosition() + vector3df(0,50,0));//位置

	vector3df dir=cam->getTarget()-cam->getPosition();

//指定随机数区间

     int RANGE_MIN = -3;
     int RANGE_MAX = 3;
	 int i;
     for (i = 0;    i < 3; i++ )
     {
         float rand100 = (((double) rand() / 
                         (double) RAND_MAX) * RANGE_MAX + RANGE_MIN);
     //    printf( "  %6d\n", rand100);
		 
		 //随机偏移方向
		 if(i==0) dir.X+=rand100;
		 if(i==1) dir.Y+=rand100;
		 if(i==2)  dir.Z+=rand100;
     }

	
	dir.normalize();
	

	//设置喷射方向
	  em->setDirection(dir);
	
	
}



void MySceneSnow::drop()
{
    ps->removeAllAffectors();
	ps->removeAll();
	ps->remove();
}