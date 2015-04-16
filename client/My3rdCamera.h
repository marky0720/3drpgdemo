#pragma once

#include "stdafx.h"

/*
  
  第三人称摄象机类

 
 1.初始化 
  // setup the camera 
 myCamera=new My3rdCamera(dev,rolenode); //传入device和要跟随的角色节点
 
 2.接收事件
 在
bool Core::OnEvent (const SEvent& e) { 
	if(myCamera) myCamera->OnEvent(e); //传入事件到camera里处理

	   
        return false; 
} 


 3.循环处理
 
	while (dev->run () && !quit) { 
                if (dev->isWindowActive ()) { 

					    myCamera->Run(); //处理camera
                        drv->beginScene (true, true, SColor (150, 50, 50, 50)); 
                        smgr->drawAll (); ...
                        drv->endScene (); ...
                } 
        } 
 */



class My3rdCamera
{
public:
	My3rdCamera(scene::IAnimatedMeshSceneNode* rolenode);

	//设置目标节点.设置好了后,就可以旋转查看节点了
	void setTargetNode(scene::IAnimatedMeshSceneNode*r){rolenode=r;};
	
	~My3rdCamera(void);
    bool OnEvent (const irr::SEvent& e);
	void update();
	ICameraSceneNode* 	getNode(){return myCamera;};

	f32 getcameraDistance(){return cameraDistance;};
private:
	
    ICameraSceneNode* myCamera;
	scene::IAnimatedMeshSceneNode* rolenode;	//要跟随的角色节点

	// camera registry 
	f32 cameraOrbit ;
	f32 cameraAngle ;
	f32 cameraDistance ;
	f32 cameraOrbitOld ;
	f32 cameraAngleOld ;

	// player registry 
	f32 playerX ;
	f32 playerY ;
	f32 playerZ ;
	f32 playerCompass ;
	f32 playerTurnTo ;
	f32 playerTurnSpeed ;
	f32 playerMoveSpeed ;

	bool smooth;
	//目标高度
	float  target_Height;
	float lasty;

	// keyboard registry 
	bool keys[irr::KEY_KEY_CODES_COUNT];
	    
	// mouse registry 
	bool mouseDownL; 
	bool mouseDownM; 
	bool mouseDownR;
	f32 lastWheelMovement;
	position2d<f32> cursor;
	position2d<f32> cursorOld;
	position2d<f32> cursorDelta;

	vector3df My3rdCamera::sphericalXYZ(f32 compassAngle, f32 elevationAngle, f32 radius);
};
