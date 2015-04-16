
#include "mylogincharclothmgr.h"

namespace Login
{


MyLoginCharClothMgr::MyLoginCharClothMgr(irr::IrrlichtDevice * device,int char_id ,int charSex)
{
	dev=device;
	smgr=dev->getSceneManager();

	this->char_id=char_id;
		

	body=0;
	trousers=0;
	dress=0;
	shoes=0;


	this->charSex=charSex;
	if(charSex==1){
		sprintf(modelPath,"man");
	}
	else if(charSex==2) {
		sprintf(modelPath,"girl");
	}

	


}

void MyLoginCharClothMgr::setClothVisible(bool v)
{

	if(body){
		body->setVisible(v); 
	}

	if(dress){
		dress->setVisible(v); 
	}

	if(trousers){
		trousers->setVisible(v); 
	}

	if(shoes){
		shoes->setVisible(v); 
	}
}

MyLoginCharClothMgr::~MyLoginCharClothMgr(void)
{
}

void MyLoginCharClothMgr::setPosition(core::vector3df pos)
{
      root->setPosition(pos); 
}

vector3df MyLoginCharClothMgr::getAbsolutePosition()
{
   return root->getAbsolutePosition(); 
}

vector3df MyLoginCharClothMgr::getPosition( )
{
	return root->getPosition(); 
}

void MyLoginCharClothMgr::setRotation(vector3df rot)
{
	  root->setRotation(rot); 
}

vector3df MyLoginCharClothMgr::getRotation()
{
	return root->getRotation(); 
}

//上身
bool MyLoginCharClothMgr::LoadBody(int id)
{
	char strrootmesh[255]="";
	sprintf(strrootmesh,"%s//%d//root.ms3d",modelPath,id);

	root=smgr->addAnimatedMeshSceneNode(smgr->getMesh(strrootmesh));

	root->setScale(core::vector3df(0.2,0.2,0.2));//模型缩放
	root->setAnimationSpeed(11);
	root->setLoopMode(false); 
	root->setID(char_id); //设置id
	root->setFrameLoop(21,22);


	char strmesh[255]="";
	sprintf(strmesh,"%s//%d//body.ms3d",modelPath,id);

	//如果已加载过其他的鞋子,就先移除
	if(body){
		body->removeAll();
		body->remove();
		//scene::ISceneNodeAnimator* anim = 0;
		//shoes->addAnimator(anim=dev->getSceneManager()->createDeleteAnimator(1));//创建删除动作,删除自己

	}

	body=smgr->addAnimatedMeshSceneNode(smgr->getMesh(strmesh),root);

	body->setMaterialFlag(video::EMF_LIGHTING, false); //不光照
	//body->setScale(core::vector3df(0.2,0.2,0.2));//模型缩放
	body->getMaterial(0).NormalizeNormals=true;

	body->addShadowVolumeSceneNode(0,0,false);
	smgr->setShadowColor(video::SColor(80,0,0,0));
	body->setID(char_id); //设置id
	body->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	body->setMaterialFlag(video::EMF_ANTI_ALIASING,true);//开启抗锯齿

	ITriangleSelector* selector= smgr->createTriangleSelector(body);
	body->setTriangleSelector(selector);//设置选择器
	////	node->setDebugDataVisible(scene::EDS_BBOX);
	//		node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF );
	//	node->setDebugDataVisible(scene::EDS_BBOX ); //设置显示包围盒

	body->setAnimationSpeed(speed);
	body->setLoopMode(false); 
	body->setFrameLoop(21,22);
	body->setCurrentFrame(21);


	//共享root骨骼.不用自己的
	ISkinnedMesh* rootmesh=(ISkinnedMesh*)root->getMesh();
	ISkinnedMesh* bodymesh=(ISkinnedMesh*)body->getMesh();
//	bool ret=bodymesh->useAnimationFrom(rootmesh);



	//每次都要加载围腰.
//	LoadDress(id);

	return true;
}

//裤子
bool MyLoginCharClothMgr::LoadTrousers(int id)
{
	char strmesh[255]="";
	sprintf(strmesh,"%s//trousers%d.ms3d",modelPath,id);

	//如果已加载过其他的鞋子,就先移除
	if(trousers){
		trousers->removeAll();
		trousers->remove();
		//scene::ISceneNodeAnimator* anim = 0;
		//shoes->addAnimator(anim=dev->getSceneManager()->createDeleteAnimator(1));//创建删除动作,删除自己

	}

	trousers = smgr->addAnimatedMeshSceneNode( smgr->getMesh(strmesh) ,root);
	trousers->setMaterialFlag(video::EMF_LIGHTING, false);//照明	
	trousers->setMaterialFlag(video::EMF_ANTI_ALIASING,true);//开启抗锯齿
	ITriangleSelector* selector= smgr->createTriangleSelector(trousers);
	trousers->setTriangleSelector(selector);//设置选择器
	trousers->addShadowVolumeSceneNode(0,0,false);
	trousers->setID(char_id); //设置id

	trousers->setAnimationSpeed(speed);
	trousers->setLoopMode(false); 
	trousers->setFrameLoop(21,22);
	trousers->setCurrentFrame(21);


	//共享root骨骼.不用自己的
	ISkinnedMesh* rootmesh=(ISkinnedMesh*)root->getMesh();
	ISkinnedMesh* trousersmesh=(ISkinnedMesh*)trousers->getMesh();
	bool ret=trousersmesh->useAnimationFrom(rootmesh);

	return true;
}

//围腰
bool MyLoginCharClothMgr::LoadDress(int id)
{
	char strmesh[255]="";
	sprintf(strmesh,"%s//%d//dress.ms3d",modelPath,id);

	dress = smgr->addAnimatedMeshSceneNode( smgr->getMesh(strmesh) );
	dress->addShadowVolumeSceneNode(0,0,false);
	dress->setMaterialFlag(video::EMF_LIGHTING, false);//照明
	dress->setMaterialFlag(video::EMF_ANTI_ALIASING,true);//开启抗锯齿
	ITriangleSelector* selector= smgr->createTriangleSelector(dress);
	dress->setTriangleSelector(selector);//设置选择器

	dress->setMaterialFlag(EMF_BACK_FACE_CULLING ,false); //关闭背面剔除



	dress->setID(char_id); //设置id
	scene::ISceneNode* pJointNode =0;
	pJointNode = body->getMS3DJointNode("joint1");  //  
	if(pJointNode)
	{
		// 不要removeAll 因为joint1是根节点啊
		//	pJointNode->removeAll();
		//	n->setPosition(vector3df(0,1,0));
		//绑定到背上
		body->addChild(dress );
	}

	dress->setAnimationSpeed(speed);
	dress->setLoopMode(false); 
	dress->setFrameLoop(1,2);
	dress->setCurrentFrame(1);

	return true;
}


//加载鞋子
bool MyLoginCharClothMgr::LoadShoes(int id)
{
	char strmesh[255]="";
	sprintf(strmesh,"%s//%d//shoes.ms3d",modelPath,id);
	
//	smgr->getMesh("man//shoes//shoes1.ms3d") ;
//	smgr->getMesh("man//shoes2.ms3d") ;

	//如果已加载过其他的鞋子,就先移除
	if(shoes){
		shoes->removeAll();
		shoes->remove();
		//scene::ISceneNodeAnimator* anim = 0;
		//shoes->addAnimator(anim=dev->getSceneManager()->createDeleteAnimator(1));//创建删除动作,删除自己
		
	}

	shoes = smgr->addAnimatedMeshSceneNode( smgr->getMesh(strmesh) ,root);
	shoes->setMaterialFlag(video::EMF_LIGHTING, false);//照明	
	shoes->setMaterialFlag(video::EMF_ANTI_ALIASING,true);//开启抗锯齿
	shoes->addShadowVolumeSceneNode(0,0,false);
	ITriangleSelector* selector= smgr->createTriangleSelector(shoes);
	shoes->setTriangleSelector(selector);//设置选择器

	shoes->setID(char_id); //设置id

	shoes->setAnimationSpeed(speed);
	shoes->setLoopMode(false); 
	shoes->setFrameLoop(21,22);
	shoes->setCurrentFrame(21);

	//让鞋子动作同步




	//共享root骨骼.不用自己的
	ISkinnedMesh* rootmesh=(ISkinnedMesh*)root->getMesh();
	ISkinnedMesh* shoesmesh=(ISkinnedMesh*)shoes->getMesh();
	bool ret=shoesmesh->useAnimationFrom(rootmesh);

	return true;
}

//做动作
void MyLoginCharClothMgr::Animate(int speed,int start,int end,bool loop)
{
	currStart=start;
	currEnd=end;
	speed=speed;
	this->m_bloop=loop;


	root->setAnimationSpeed(speed);
	root->setLoopMode(loop); 
	root->setFrameLoop(start,end);
	


	if(body){
		body->setAnimationSpeed(speed); 
		body->setLoopMode(loop); 
		body->setFrameLoop(start,end); 
	}

	if(dress){
		dress->setAnimationSpeed(speed); 
		dress->setLoopMode(loop); 
		dress->setFrameLoop(start,end); 
	}

	if(trousers){
		trousers->setAnimationSpeed(speed); 
		trousers->setLoopMode(loop); 
		trousers->setFrameLoop(start,end); 
	}

	if(shoes){
		shoes->setAnimationSpeed(speed); 
		shoes->setLoopMode(loop); 
		shoes->setFrameLoop(start,end); 
	}

}

//卸载所有角色的装备模型
void MyLoginCharClothMgr::DropAll()
{
	scene::ISceneNodeAnimator* anim = 0;

	if(root)
		root->addAnimator(anim=dev->getSceneManager()->createDeleteAnimator(1));//创建删除动作,删除自己

	//其他的身体部分不用在手动删除,因为他们都是root的子节点.父节点删除了.他们会自动删除的.


	anim->drop();

}


}