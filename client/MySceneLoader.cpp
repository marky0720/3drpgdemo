#include ".\mysceneloader.h"

//场景配置文件
char FilePath[255]="scene//scene.ini";

MySceneLoader::MySceneLoader()
{
	this->dev=getIrrDevice();
	smgr=getIrrSmgr();
	drv=getIrrDriver();


	grass=0;
	Snow=0;
	skydome=0;
	light1=0;

	meta=0;
	height=0;


}

MySceneLoader::~MySceneLoader(void)
{
}

void MySceneLoader::update()
{
	//雪更新
	if(Snow) Snow->Update();
}

//读取地图可移动区域文件
void MySceneLoader::ReadLevelCollisionFile()
{
	//通过配置文件得到数据
	
	char inBuf[255];     //用于保存读取的字符串
	char temp[255]="";
	sprintf(temp,"scene%d",currWorldMap); 

	bool ret=false;
	ret=IniGetString(FilePath,temp,"CollisionMap",inBuf);

	//未配置碰撞区域文件
	if(ret==false){
		return;
	}

	mylist.clear();

	FILE* log;
	log = fopen(inBuf, "a+");
	//文件不存在.返回
	if (log == NULL)
		return ;
	char strtemp[255]="";
	GetTxtByRowNum(inBuf,strtemp,1);
	int size=atoi(strtemp);
	printf("三角形size:%d\n",size);
	for(int i=1;i<=size;i++){
		GetTxtByRowNum(inBuf,strtemp,1+i);
		triangle3df* s=new triangle3df();
		sscanf(strtemp, "%f %f %f %f %f %f %f %f %f", 
			&s->pointA.X,&s->pointA.Y,&s->pointA.Z,
			&s->pointB.X,&s->pointB.Y,&s->pointB.Z,
			&s->pointC.X,&s->pointC.Y,&s->pointC.Z
			); 
		mylist.push_back(s);
	}
}



void MySceneLoader::CalculMeta()
{
	// Create a meta triangle selector to hold several triangle selectors.
	//创建一个材质选择器,它存储了一些三角形选择器,可以管理场景
	if(meta)meta->removeAllTriangleSelectors();
	else
		meta = smgr->createMetaTriangleSelector();
	//定义array 存储所有场景节点
	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes); // 找出所有节点

	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i]; //
		//node->setMaterialFlag(video::EMF_WIREFRAME, true);
		scene::ITriangleSelector * selector = 0; //三角形选择器]

		//id =10是角色模型 忽略
		if(node->getID()==10){
			continue;
		}
		

		//判断类型
		switch(node->getType())
		{
		case scene::ESNT_CUBE:
		case scene::ESNT_ANIMATED_MESH:
			//因为这个选择器不会随mesh运动,并且他只是用于和摄象机碰撞,,所以只需要创建绑定box.
			// Because the selector won't animate with the mesh,
			// and is only being used for camera collision, we'll just use an approximate
			// bounding box instead of ((scene::IAnimatedMeshSceneNode*)node)->getMesh(0)
			//selector = smgr->createTriangleSelectorFromBoundingBox(node);
			if( node->getID()==101){
				//((scene::IAnimatedMeshSceneNode*)node)->addShadowVolumeSceneNode(0,0,false);
				//smgr->setShadowColor(video::SColor(80,0,0,0));
			//	selector = smgr->createTriangleSelectorFromBoundingBox(node);
			}
			break;

		case scene::ESNT_MESH:
		case scene::ESNT_SPHERE: // Derived from IMeshSceneNode 球面 来自IMeshSceneNode
			if(node->getID()==100 ){
				selector = smgr->createTriangleSelector(((scene::IMeshSceneNode*)node)->getMesh(), node);
			}
			break;

		case scene::ESNT_TERRAIN:
			//地形选择器
		
			//selector = smgr->createTerrainTriangleSelector((scene::ITerrainSceneNode*)node);
			break;

		case scene::ESNT_OCT_TREE:
			//oct树选择器
			//selector = smgr->createOctTreeTriangleSelector(((scene::IMeshSceneNode*)node)->getMesh(), node);
			break;

		default:
			// Don't create a selector for this node type
			//其他的类型就不创建三角形选择器了
			break;
		}
		

		//存在则添加进材质选择器里
		if(selector)
		{
			// Add it to the meta selector, which will take a reference to it
			// 添加到材质选择器
			meta->addTriangleSelector(selector);
			// And drop my reference to it, so that the meta selector owns it.
			selector->drop();
		}
	}
}

//只卸载场景中的地形数据.
void MySceneLoader::RemoveTerrain()
{

	//定义array 存储所有场景节点
	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes); // 找出所有节点

	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i]; //

		//移除地形 统一定义地形mesh id 100,也可以定义比如地板等需要角色走上去的模型.该类模型会被鼠标拾取
		if(node->getID()==100 && node->getType()==scene::ESNT_MESH){
			node->removeAnimators();
			node->removeAll();
			node->remove();
		
		}else if(node->getID()==101 && node->getType()==scene::ESNT_MESH){
				//统一树,建筑模型101 必须是静态mesh.定义101的模型.不会被鼠标拾取.即角色不会走上去.只会穿过..
			node->removeAnimators();
			node->removeAll();
			node->remove();
		}
		
	}

}


bool MySceneLoader::LoadScene(int worldMap,irr::scene::ICameraSceneNode* cameraNode)
{
	this->currWorldMap=worldMap;
	this->cameraNode=cameraNode;


	drv->setFog(video::SColor(0,190,190,190),video::EFT_FOG_LINEAR, true, 400, 1500, 0.15, true);

char* inBuf2=new char[255];    //用于保存读取的字符串
	char strScene[255]="";
	sprintf(strScene,"scene%d",worldMap); //id

	//读取 bg_music失败.表示该场景不使用背景音乐
	if( IniGetString(FilePath,strScene,"bg_music",inBuf2) == FALSE){
		//MessageBox(0,"读取场景声音配置文件失败!","",0);
		//说明这个场景不要音乐
		//exit(1);
		getSoundEngine()->stopAllSounds();
		getSoundEngine()->removeAllSoundSources();
	}else{

		getSoundEngine()->stopAllSounds();
		getSoundEngine()->removeAllSoundSources();
		getSoundEngine()->play2D(inBuf2,true);//直接播放
	}


//地图

	
  //读
if(IniGetString(FilePath,strScene,"Level",inBuf2) == FALSE){
	MessageBox(0,"读取场景配置文件失败!Level","",0);
	exit(1);
}


//IFileSystem * pFS = dev->getGUIEnvironment()->getFileSystem();
////创建读文件接口，这里特别注意第3个参数，其后缀一定要和文件格式一致，如果用的是bmp，则应该是testpng.bmp
//IReadFile * pMFR = pFS->createMemoryReadFile(terrain2,terrain2_size,"terrain2.irr",false);
//	smgr->loadScene(pMFR);
if(strlen(inBuf2)<3)
{
 MessageBox(0,"读取场景配置文件失败!Level","",0);
  exit(1);
}
	smgr->loadScene(inBuf2);




CalculMeta();
//


//读取sky

if(IniGetString(FilePath,strScene,"sky",inBuf2) ){
	//// create   skydome
	drv->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	skydome=smgr->addSkyDomeSceneNode(drv->getTexture( inBuf2 ),16,8,0.95f,2.0f);
	//skydome->setPosition(char_mgr->GetMainCharacter()->getPosition());

	skydome->setScale(vector3df(0.25,0.25,0.25));
	skydome->setMaterialFlag(video::EMF_FOG_ENABLE, true);//雾
	skydome->setMaterialFlag(video::EMF_LIGHTING, true);//照明
	drv->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);
	scene::ISceneNodeAnimator* anim;
	anim = smgr->createRotationAnimator(core::vector3df(0,0.003f,0));  //绕Y轴旋转 速度0.3
	skydome->addAnimator(anim);
	anim->drop();	





}




//初始化下雪
Snow=new MySceneSnow(cameraNode);

//草地
grass=new MySceneGrass(worldMap);
grass->DrawAllGrass();


//读取地图可移动区域文件
ReadLevelCollisionFile();



//加载此地图的传送点

	if( IniGetString(FilePath,strScene,"teleport1_POS",inBuf2) ){
		vector3df p;

		sscanf(inBuf2, "%f,%f,%f", 
			&p.X,&p.Y,&p.Z
			); 

		light1=new MySceneVolumeLight("textures/VolumeLight.bmp");
		light1->setPosition(p);
		light1->setScale(core::vector3df(5.0f, 25.0f,5.0f));
		ISceneNodeAnimator* anim = smgr->createRotationAnimator (core::vector3df(0,1,0));
		light1->getNode()->addAnimator(anim);

	}


delete inBuf2;

return true;
}



//所有不可移动区域的三角形,根据一点是否在内部来判断
bool MySceneLoader::Pointisin(core::vector3df test )
{		
	//不存在碰撞区域.可任意走动
	if(mylist.getSize()==0) return false;

	core::list<core::triangle3df*>::Iterator  list_oper;
	for(list_oper=mylist.begin();list_oper!=mylist.end();++list_oper)
	{
		core::triangle3df temp_triangle;
		temp_triangle= *(*list_oper);
		if(temp_triangle.isPointInside(test)){

			return true;
		}
	}
	return false;
}


//角色可以移动到pos吗
bool MySceneLoader::canmovehere(vector3df pos)
{
	if( mylist.getSize()==0) return true;

	core::list<core::triangle3df*>::Iterator  list_oper;
	//所有不可移动区域的三角形,根据一点是否在内部来判断
	for(list_oper= mylist.begin();list_oper!= mylist.end();++list_oper)
	{
		core::triangle3df temp_triangle;
		temp_triangle= *(*list_oper);
		if(temp_triangle.isPointInside(pos)){
			//pos在不可移动区域,返回false,不可移动
			return false;
		}
	}
	//可移动
	return true;
}

//得到地形高度
float MySceneLoader::getHeight(float x,float z)
{
	float ret=0;


	core::triangle3df triangle;
	core::line3d<f32> line;//碰撞点
	line.start.X=x;
	line.start.Z=z;
	line.start.Y=1000;  //有区别的  -和+ 会导致他优先碰撞两个层mesh.最先返回点.看你需要最先返回最上层,还是最下层
	line.end.X=x;
	line.end.Z=z;
	line.end.Y=-1000;

	//和地形碰撞

	const ISceneNode* outnode;
	if (smgr->getSceneCollisionManager()->getCollisionPoint(
		line, //[in] 射线
		meta, //[in] 场景节点的三角形选择器
		line.end, //[out] 返回碰撞点
		triangle,outnode)) //[out] 返回碰撞的三角
	{
		
		ret=float(line.end.Y+height)/(float)2; //
		height=line.end.Y;//保存上次高度
	}

	return ret;
}



//设置加载的场景是否可见
void MySceneLoader::setVisiable(bool v)
{

	if(grass) grass->setVisible(v);

	if(Snow) Snow->setVisible(v);

	if(light1) light1->getNode()->setVisible(v);

	if(skydome) skydome->setVisible(v);
	
	//定义array 存储所有场景节点
	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes); // 找出所有节点
	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i]; //
		//地形 统一定义地形mesh id 100
		if(node->getID()==100 || node->getID()==101 ){
			node->setVisible(v);
			continue;
		}

		//判断类型
		switch(node->getType())
		{
		case scene::ESNT_CUBE:
		case scene::ESNT_ANIMATED_MESH:
			//node->setVisible(v);
			break;

		case scene::ESNT_MESH:
		case scene::ESNT_SPHERE: // Derived from IMeshSceneNode 球面 来自IMeshSceneNode
			node->setVisible(v);
			break;

		case scene::ESNT_TERRAIN:
			//地形选择器
			node->setVisible(v);
			
			break;

		case scene::ESNT_OCT_TREE:
			node->setVisible(v);
			//oct树选择器
			break;

		default:
			// Don't create a selector for this node type
			//其他的类型就不创建三角形选择器了
			break;
		}

	}

}

//卸载
void MySceneLoader::DropCurrScene()
{
	//只移除地形
	RemoveTerrain();


	if(grass) grass->drop();

	if(Snow) Snow->drop();

	if(light1) light1->drop();
	

	if(skydome){
		scene::ISceneNodeAnimator* anim;
		anim = smgr->createDeleteAnimator(1);  //删除
		skydome->addAnimator(anim);
		anim->drop();	
	}
	

}