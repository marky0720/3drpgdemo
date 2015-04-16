#include "StdAfx.h"
#include ".\my3rdcamera.h"

vector3df My3rdCamera::sphericalXYZ(f32 compassAngle, f32 elevationAngle, f32 radius){ 

   compassAngle = compassAngle * -1; 
   elevationAngle = elevationAngle * -1; 

   elevationAngle = elevationAngle + 90; 

   f32 x = radius * cos(compassAngle * PI/180.0f ) * sin(elevationAngle * PI/180.0f ); 
   f32 z = radius * sin(compassAngle * PI/180.0f ) * sin(elevationAngle * PI/180.0f ); 
   f32 y = radius * cos(elevationAngle * PI/180.0f ); 

   vector3df result; 
   result.X = x; 
   result.Y = y; 
   result.Z = z; 
   return result; 
} 

#define MAXcameraDistance 150

My3rdCamera::My3rdCamera( scene::IAnimatedMeshSceneNode* _rolenode)
{


	rolenode=_rolenode;
	
	for(int x=0; x<irr::KEY_KEY_CODES_COUNT; x++) keys[x] = false; 

	// camera registry 
	 cameraOrbit = 30;   //角 0-360度,摄象机绕Y轴旋转的角度数
	 cameraAngle = 30;  //角度,摄象机和水平面的夹角
	 cameraDistance = 120; //距离,摄象机和角色节点距离 
	 cameraOrbitOld = 0; 
	 cameraAngleOld = 0; 

	 if(rolenode==0){
		// player registry 
		playerX = 0;
		playerY =0;
		playerZ = 0;
	 }else{
	  	// player registry 
		playerX = rolenode->getPosition().X; 
		playerY = rolenode->getPosition().Y; 
		playerZ = rolenode->getPosition().Z;  
	 }
	 
	 playerCompass = 30; 
	 playerTurnTo = 0; 
	 playerTurnSpeed = 1; 
	 playerMoveSpeed = 2.01; 
	
	 // setup the camera 
	 if(rolenode){
		 myCamera = getIrrDevice()->getSceneManager()->addCameraSceneNode(0, sphericalXYZ(cameraOrbit,cameraAngle,cameraDistance), rolenode->getPosition()); 
	 }
	 else{
		 myCamera = getIrrDevice()->getSceneManager()->addCameraSceneNode(0, sphericalXYZ(cameraOrbit,cameraAngle,cameraDistance)); 
	 }

	 myCamera->setFarValue(700);

	// myCamera->setAutomaticCulling( EAC_BOX  );

	 mouseDownR=false;
	 mouseDownL=false;
	 mouseDownM=false;

	 target_Height=0;

	  lasty= 10; 

	  //平滑标志 true表示使用平滑cam.抖动不明显
	  smooth=false;

}

My3rdCamera::~My3rdCamera(void)
{

}

bool My3rdCamera::OnEvent (const SEvent& e)
{

        if (e.EventType == EET_MOUSE_INPUT_EVENT) 
        { 
			// left mouse button state check 
			if(e.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)   mouseDownL = true; 
			if(e.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)      mouseDownL = false; 

			// middle mouse button state check 
			if(e.MouseInput.Event == EMIE_MMOUSE_PRESSED_DOWN)   mouseDownM = true; 
			if(e.MouseInput.Event == EMIE_MMOUSE_LEFT_UP)      mouseDownM = false; 

			if(e.MouseInput.Event == EMIE_MOUSE_WHEEL) 
			{ 
				cameraDistance -= e.MouseInput.Wheel * (cameraDistance / 20) * 2; 
				if(cameraDistance < 5) cameraDistance = 5; 
				if(cameraDistance > MAXcameraDistance) cameraDistance = MAXcameraDistance; 
			} 

			// right mouse button state check 8
			//鼠标中键旋转
			if(e.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)  //EMIE_RMOUSE_PRESSED_DOWN EMIE_MMOUSE_PRESSED_DOWN
			{ 
				mouseDownR = true; 
				cursorOld.X = e.MouseInput.X; 
				cursorOld.Y = e.MouseInput.Y; 
				cursorDelta.X = 0; 
				cursorDelta.Y = 0; 
				cameraOrbitOld = cameraOrbit; 
				cameraAngleOld = cameraAngle; 

			} 
			 if(e.MouseInput.Event == EMIE_RMOUSE_LEFT_UP)      mouseDownR = false; 
          
			// mouse move check 
			if(e.MouseInput.Event == EMIE_MOUSE_MOVED) 
			{ 
				
					// add condition that right mouse button be down 
					if(mouseDownR == true){ 
					//		if(cursorDelta.Y > 176) cursorDelta.Y = 176; 
					//		if(cursorDelta.Y < -176) cursorDelta.Y = -176; 
							cursor.X = e.MouseInput.X; 
							cursor.Y = e.MouseInput.Y; 
						cursorDelta.X = cursor.X - cursorOld.X; 
						cursorDelta.Y = cursor.Y - cursorOld.Y; 
					//	if(cursorDelta.Y > 100) cursorDelta.Y = 100; 
					//	if(cursorDelta.Y < -100) cursorDelta.Y = -100; 
						cameraOrbit = (int)(cameraOrbitOld + cursorDelta.X) % 360; 
						cameraAngle = (int)(cameraAngleOld + cursorDelta.Y) % 360; 
						if(cameraAngle > 88) cameraAngle = 88;  //限制最大角88度
						//if(cameraAngle < 0.5) cameraAngle = 0.5; 
 							if(cameraAngle < -88) cameraAngle = -88; //限制最小角-88度
						
					} 

			} 

		}



	return false;
}


void My3rdCamera::update()
{
	//if(keys[KEY_KEY_A]){
	//	vector3df rot=rolenode->getRotation();
	//			rot.Y++;
	//			rolenode->setRotation(rot);
	//}
	//if(keys[KEY_KEY_D]){
	//			vector3df rot=rolenode->getRotation();
	//			rot.Y--;
	//			rolenode->setRotation(rot);
	//}
 //   if(keys[KEY_KEY_S]){ 

 //        // player movement vector 
 //        vector3df playerStep = sphericalXYZ(cameraOrbit,0,playerMoveSpeed); 
 //        playerX += playerStep.X; 
 //        playerY += playerStep.Y; 
 //        playerZ += playerStep.Z; 
	//	 rolenode->setPosition(vector3df(playerX,playerY,playerZ));
	//}
	//if(keys[KEY_KEY_W]){ 

 //        // player movement vector 
 //        vector3df playerStep = sphericalXYZ(cameraOrbit,0,playerMoveSpeed); 
 //        playerX -= playerStep.X; 
 //        playerY -= playerStep.Y; 
 //        playerZ -= playerStep.Z; 
	//	 rolenode->setPosition(vector3df(playerX,playerY,playerZ));
 //   }
 //   if(keys[KEY_KEY_Q]){ 

 //        // player movement vector 
 //        vector3df playerStep = sphericalXYZ(cameraOrbit,0,playerMoveSpeed); 
	//	 playerStep.normalize();
	//	 vector3df leftdir=playerStep.crossProduct(vector3df(0,1,0));
 //        playerX -= leftdir.X; 
 //        playerY -= leftdir.Y; 
 //        playerZ -= leftdir.Z; 
	//	 rolenode->setPosition(vector3df(playerX,playerY,playerZ));
 //   }
 //   if(keys[KEY_KEY_E]){ 

 //        // player movement vector 
 //        vector3df playerStep = sphericalXYZ(cameraOrbit,0,playerMoveSpeed); 
	//	 playerStep.normalize();
	//	 vector3df leftdir=playerStep.crossProduct(vector3df(0,1,0));
 //        playerX += leftdir.X; 
 //        playerY += leftdir.Y; 
 //        playerZ += leftdir.Z; 
	//	 rolenode->setPosition(vector3df(playerX,playerY,playerZ));
 //   }
	
	if(rolenode){

		playerX = rolenode->getPosition().X; 
		playerY = rolenode->getPosition().Y+10;  //稍微上移点,让视点看向角色的中部而不是底部
		playerZ = rolenode->getPosition().Z;

		//获得目标高度
		target_Height=playerY;

		
	}

	//目标高度和现在高度对比.然后平滑过度
	  if(target_Height>lasty && target_Height-lasty>0.25){
		  lasty+=0.25;
		 // campos=rolepos+ sphericalXYZ(cameraOrbit,cameraAngle,cameraDistance);
	  }else if(target_Height < lasty && target_Height-lasty<0.25 ){
		  lasty-=0.25;
	  }

	  //最后高度差别不大了就直接设置
	  if(abs(target_Height-lasty)<0.4){
			lasty=target_Height;
	  }

	  //计算角色高度
	  vector3df rolepos=vector3df(playerX,lasty,playerZ);

	  if(smooth==false) rolepos.Y=playerY;

	  //摄象机设置目标 
	  myCamera->setTarget(rolepos); 

	  vector3df campos;
	  campos=rolepos+ sphericalXYZ(cameraOrbit,cameraAngle,cameraDistance);


      // orient camera 
      myCamera->setPosition(campos); 
	
	

}