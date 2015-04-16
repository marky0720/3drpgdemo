#include "StdAfx.h"
#include ".\myscenevolumelight.h"

MySceneVolumeLight::MySceneVolumeLight( char* texture)
{
	this->device=getIrrDevice();
	ISceneManager* smgr=device->getSceneManager();
	irr::video::IVideoDriver *drv=device->getVideoDriver(); 
	 node = smgr->addVolumeLightSceneNode(0, -1,
				8,                               // Subdivisions on U axis 32
				8,                               // Subdivisions on V axis 32
				video::SColor(0, 15, 15, 55),    // foot color
				video::SColor(0, 5, 5, 5));       // tail color

	if (node)
	{
	//	node->setScale(core::vector3df(1.0f, 256.0f,1.0f));
	//	node->setPosition(core::vector3df(1,0,40));
	//	node->setMaterialTexture(0, drv->getTexture(texture)); //"VolumeLight/1.bmp"
	//	nlignt->setMaterialFlag(EMF_BACK_FACE_CULLING ,true); //关闭背面剔除
		node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR); //加了这个就透明了..
			
		// create  animator
		scene::ISceneNodeAnimator* rot = smgr->createRotationAnimator(vector3df(0,0.03,0));
		node->addAnimator(rot);
		rot->drop();
	}

}

MySceneVolumeLight::~MySceneVolumeLight(void)
{
}

void MySceneVolumeLight::drop(void)
{	
	scene::ISceneNodeAnimator* anim;		
	node->addAnimator(anim=device->getSceneManager()->createDeleteAnimator(1));//创建删除动作,删除自己,场景节点
	anim->drop();
}
