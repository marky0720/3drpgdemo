#include ".\mysceneloader.h"

//���������ļ�
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
	//ѩ����
	if(Snow) Snow->Update();
}

//��ȡ��ͼ���ƶ������ļ�
void MySceneLoader::ReadLevelCollisionFile()
{
	//ͨ�������ļ��õ�����
	
	char inBuf[255];     //���ڱ����ȡ���ַ���
	char temp[255]="";
	sprintf(temp,"scene%d",currWorldMap); 

	bool ret=false;
	ret=IniGetString(FilePath,temp,"CollisionMap",inBuf);

	//δ������ײ�����ļ�
	if(ret==false){
		return;
	}

	mylist.clear();

	FILE* log;
	log = fopen(inBuf, "a+");
	//�ļ�������.����
	if (log == NULL)
		return ;
	char strtemp[255]="";
	GetTxtByRowNum(inBuf,strtemp,1);
	int size=atoi(strtemp);
	printf("������size:%d\n",size);
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
	//����һ������ѡ����,���洢��һЩ������ѡ����,���Թ�����
	if(meta)meta->removeAllTriangleSelectors();
	else
		meta = smgr->createMetaTriangleSelector();
	//����array �洢���г����ڵ�
	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes); // �ҳ����нڵ�

	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i]; //
		//node->setMaterialFlag(video::EMF_WIREFRAME, true);
		scene::ITriangleSelector * selector = 0; //������ѡ����]

		//id =10�ǽ�ɫģ�� ����
		if(node->getID()==10){
			continue;
		}
		

		//�ж�����
		switch(node->getType())
		{
		case scene::ESNT_CUBE:
		case scene::ESNT_ANIMATED_MESH:
			//��Ϊ���ѡ����������mesh�˶�,������ֻ�����ں��������ײ,,����ֻ��Ҫ������box.
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
		case scene::ESNT_SPHERE: // Derived from IMeshSceneNode ���� ����IMeshSceneNode
			if(node->getID()==100 ){
				selector = smgr->createTriangleSelector(((scene::IMeshSceneNode*)node)->getMesh(), node);
			}
			break;

		case scene::ESNT_TERRAIN:
			//����ѡ����
		
			//selector = smgr->createTerrainTriangleSelector((scene::ITerrainSceneNode*)node);
			break;

		case scene::ESNT_OCT_TREE:
			//oct��ѡ����
			//selector = smgr->createOctTreeTriangleSelector(((scene::IMeshSceneNode*)node)->getMesh(), node);
			break;

		default:
			// Don't create a selector for this node type
			//���������;Ͳ�����������ѡ������
			break;
		}
		

		//��������ӽ�����ѡ������
		if(selector)
		{
			// Add it to the meta selector, which will take a reference to it
			// ��ӵ�����ѡ����
			meta->addTriangleSelector(selector);
			// And drop my reference to it, so that the meta selector owns it.
			selector->drop();
		}
	}
}

//ֻж�س����еĵ�������.
void MySceneLoader::RemoveTerrain()
{

	//����array �洢���г����ڵ�
	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes); // �ҳ����нڵ�

	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i]; //

		//�Ƴ����� ͳһ�������mesh id 100,Ҳ���Զ������ذ����Ҫ��ɫ����ȥ��ģ��.����ģ�ͻᱻ���ʰȡ
		if(node->getID()==100 && node->getType()==scene::ESNT_MESH){
			node->removeAnimators();
			node->removeAll();
			node->remove();
		
		}else if(node->getID()==101 && node->getType()==scene::ESNT_MESH){
				//ͳһ��,����ģ��101 �����Ǿ�̬mesh.����101��ģ��.���ᱻ���ʰȡ.����ɫ��������ȥ.ֻ�ᴩ��..
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

char* inBuf2=new char[255];    //���ڱ����ȡ���ַ���
	char strScene[255]="";
	sprintf(strScene,"scene%d",worldMap); //id

	//��ȡ bg_musicʧ��.��ʾ�ó�����ʹ�ñ�������
	if( IniGetString(FilePath,strScene,"bg_music",inBuf2) == FALSE){
		//MessageBox(0,"��ȡ�������������ļ�ʧ��!","",0);
		//˵�����������Ҫ����
		//exit(1);
//		getSoundEngine()->stopAllSounds();
	//	getSoundEngine()->removeAllSoundSources();
	}else{

		//getSoundEngine()->stopAllSounds();
		//getSoundEngine()->removeAllSoundSources();
		//getSoundEngine()->play2D(inBuf2,true);//ֱ�Ӳ���
	}


//��ͼ

	
  //��
if(IniGetString(FilePath,strScene,"Level",inBuf2) == FALSE){
	MessageBox(0,"��ȡ���������ļ�ʧ��!Level","",0);
	exit(1);
}


//IFileSystem * pFS = dev->getGUIEnvironment()->getFileSystem();
////�������ļ��ӿڣ������ر�ע���3�����������׺һ��Ҫ���ļ���ʽһ�£�����õ���bmp����Ӧ����testpng.bmp
//IReadFile * pMFR = pFS->createMemoryReadFile(terrain2,terrain2_size,"terrain2.irr",false);
//	smgr->loadScene(pMFR);
if(strlen(inBuf2)<3)
{
 MessageBox(0,"��ȡ���������ļ�ʧ��!Level","",0);
  exit(1);
}
	smgr->loadScene(inBuf2);




CalculMeta();
//


//��ȡsky

if(IniGetString(FilePath,strScene,"sky",inBuf2) ){
	//// create   skydome
	drv->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	skydome=smgr->addSkyDomeSceneNode(drv->getTexture( inBuf2 ),16,8,0.95f,2.0f);
	//skydome->setPosition(char_mgr->GetMainCharacter()->getPosition());

	skydome->setScale(vector3df(0.25,0.25,0.25));
	skydome->setMaterialFlag(video::EMF_FOG_ENABLE, true);//��
	skydome->setMaterialFlag(video::EMF_LIGHTING, true);//����
	drv->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);
	scene::ISceneNodeAnimator* anim;
	anim = smgr->createRotationAnimator(core::vector3df(0,0.003f,0));  //��Y����ת �ٶ�0.3
	skydome->addAnimator(anim);
	anim->drop();	





}




//��ʼ����ѩ
Snow=new MySceneSnow(cameraNode);

//�ݵ�
grass=new MySceneGrass(worldMap);
grass->DrawAllGrass();


//��ȡ��ͼ���ƶ������ļ�
ReadLevelCollisionFile();



//���ش˵�ͼ�Ĵ��͵�

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



//���в����ƶ������������,����һ���Ƿ����ڲ����ж�
bool MySceneLoader::Pointisin(core::vector3df test )
{		
	//��������ײ����.�������߶�
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


//��ɫ�����ƶ���pos��
bool MySceneLoader::canmovehere(vector3df pos)
{
	if( mylist.getSize()==0) return true;

	core::list<core::triangle3df*>::Iterator  list_oper;
	//���в����ƶ������������,����һ���Ƿ����ڲ����ж�
	for(list_oper= mylist.begin();list_oper!= mylist.end();++list_oper)
	{
		core::triangle3df temp_triangle;
		temp_triangle= *(*list_oper);
		if(temp_triangle.isPointInside(pos)){
			//pos�ڲ����ƶ�����,����false,�����ƶ�
			return false;
		}
	}
	//���ƶ�
	return true;
}

//�õ����θ߶�
float MySceneLoader::getHeight(float x,float z)
{
	float ret=0;


	core::triangle3df triangle;
	core::line3d<f32> line;//��ײ��
	line.start.X=x;
	line.start.Z=z;
	line.start.Y=1000;  //�������  -��+ �ᵼ����������ײ������mesh.���ȷ��ص�.������Ҫ���ȷ������ϲ�,�������²�
	line.end.X=x;
	line.end.Z=z;
	line.end.Y=-1000;

	//�͵�����ײ

	const ISceneNode* outnode;
	if (smgr->getSceneCollisionManager()->getCollisionPoint(
		line, //[in] ����
		meta, //[in] �����ڵ��������ѡ����
		line.end, //[out] ������ײ��
		triangle,outnode)) //[out] ������ײ������
	{
		
		ret=float(line.end.Y+height)/(float)2; //
		height=line.end.Y;//�����ϴθ߶�
	}

	return ret;
}



//���ü��صĳ����Ƿ�ɼ�
void MySceneLoader::setVisiable(bool v)
{

	if(grass) grass->setVisible(v);

	if(Snow) Snow->setVisible(v);

	if(light1) light1->getNode()->setVisible(v);

	if(skydome) skydome->setVisible(v);
	
	//����array �洢���г����ڵ�
	core::array<scene::ISceneNode *> nodes;
	smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes); // �ҳ����нڵ�
	for (u32 i=0; i < nodes.size(); ++i)
	{
		scene::ISceneNode * node = nodes[i]; //
		//���� ͳһ�������mesh id 100
		if(node->getID()==100 || node->getID()==101 ){
			node->setVisible(v);
			continue;
		}

		//�ж�����
		switch(node->getType())
		{
		case scene::ESNT_CUBE:
		case scene::ESNT_ANIMATED_MESH:
			//node->setVisible(v);
			break;

		case scene::ESNT_MESH:
		case scene::ESNT_SPHERE: // Derived from IMeshSceneNode ���� ����IMeshSceneNode
			node->setVisible(v);
			break;

		case scene::ESNT_TERRAIN:
			//����ѡ����
			node->setVisible(v);
			
			break;

		case scene::ESNT_OCT_TREE:
			node->setVisible(v);
			//oct��ѡ����
			break;

		default:
			// Don't create a selector for this node type
			//���������;Ͳ�����������ѡ������
			break;
		}

	}

}

//ж��
void MySceneLoader::DropCurrScene()
{
	//ֻ�Ƴ�����
	RemoveTerrain();


	if(grass) grass->drop();

	if(Snow) Snow->drop();

	if(light1) light1->drop();
	

	if(skydome){
		scene::ISceneNodeAnimator* anim;
		anim = smgr->createDeleteAnimator(1);  //ɾ��
		skydome->addAnimator(anim);
		anim->drop();	
	}
	

}