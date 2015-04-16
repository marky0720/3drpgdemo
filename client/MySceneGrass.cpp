#include ".\myscenegrass.h"




MySceneGrass::MySceneGrass(int worldMap)
{


  this->device=getIrrDevice();
  this->worldMap=worldMap;

  //场景配置文件
  char configFilePath[255]="scene//scene.ini";

  char inBuf[255];     //用于保存读取的字符串
  char temp[255]="";
  sprintf(temp,"scene%d",worldMap); 

  //读取草地模型
  bool ret=IniGetString(configFilePath,temp,"grass_model",inBuf);  //读
  //未配置草地文件
  if(ret==false){
	  m_bUseGrass=false;
  }else{
	  //草地模型路径
	  sprintf(grass_model_path,"%s",inBuf);
	  m_bUseGrass=true;
  }

  //读取草地世界分布坐标文件
   ret=IniGetString(configFilePath,temp,"grass_list",inBuf);  //读
  //未配置草地文件
  if(ret==false){
	  m_bUseGrass=false;
  }else{
	  //草地模型路径
	  sprintf(grass_list_path,"%s",inBuf);
	   m_bUseGrass=true;
  }


}

void MySceneGrass::setVisible(bool v)
{
	core::list<irr::scene::IAnimatedMeshSceneNode*>::Iterator  list_oper;
	for(list_oper=mygrass_list.begin();list_oper!=mygrass_list.end();++list_oper)
	{

		irr::scene::IAnimatedMeshSceneNode* no=(*list_oper);//或core::triangle3df* temp_triangle; temp_triangle= *list_oper;triangle3df t=*(temp_triangle);
		no->setVisible(v);
		
	}
}

MySceneGrass::~MySceneGrass(void)
{
}
void MySceneGrass::DrawAGrass(vector3df pos)
{
	struct _timeb timebuffer;//定义一个结构
	_ftime(&timebuffer);//初始化timebuffer
	srand(timebuffer.millitm);//timebuffer.millitm获得毫秒


	




	irr::scene::IAnimatedMeshSceneNode* no= device->getSceneManager()->addAnimatedMeshSceneNode(
		device->getSceneManager()->getMesh(grass_model_path),0,0,pos);  
	no->setMaterialFlag(video::EMF_LIGHTING, false);//照明
	no->setScale(vector3df(5,5,5));
	no->setMaterialFlag(video::EMF_FOG_ENABLE,true);//设置fog开关
	//no->setMaterialTexture(0,device->getVideoDriver()->getTexture("SAND1.BMP"));
	no->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL );// 加了这个就透明了..
//	no->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR ); EMT_TRANSPARENT_ALPHA_CHANNEL EMT_TRANSPARENT_VERTEX_ALPHA


	no->setMaterialFlag(video::EMF_BACK_FACE_CULLING ,false); //关闭背面剔除

	mygrass_list.push_back(no);
	
}


void  MySceneGrass::DrawAllGrass()
{
	//该地图配置文件未使用草地
	if(m_bUseGrass==false) return;

	ReadFile();

	//输出到文件
	core::list<core::vector3df>::Iterator  list_oper;
	for(list_oper=mylist.begin();list_oper!=mylist.end();++list_oper)
	{

		vector3df pos(*list_oper);//或core::triangle3df* temp_triangle; temp_triangle= *list_oper;triangle3df t=*(temp_triangle);
		DrawAGrass(pos);
	}
}

//从文件读取草地在世界的分布坐标
void  MySceneGrass::ReadFile()
{

	FILE* log;
	log = fopen(grass_list_path, "a+");
	if (log == NULL)
		return ;

	char strtemp[255]="";
	GetTxtByRowNum(grass_list_path,strtemp,1);

	int size=atoi(strtemp);
	printf("size:%d\n",size);

	for(int i=1;i<=size;i++){

		GetTxtByRowNum(grass_list_path,strtemp,1+i);
		vector3df s;
		sscanf(strtemp, "%f %f %f", 
			&s.X,&s.Y,&s.Z
			); 

		mylist.push_back(s);
	}
	//

}

void MySceneGrass::drop()
{
	core::list<irr::scene::IAnimatedMeshSceneNode*>::Iterator  list_oper;
	for(list_oper=mygrass_list.begin();list_oper!=mygrass_list.end();++list_oper)
	{

		irr::scene::IAnimatedMeshSceneNode* no=(*list_oper);//或core::triangle3df* temp_triangle; temp_triangle= *list_oper;triangle3df t=*(temp_triangle);
		no->removeAnimators();
		no->removeAll();
		no->remove();
	}
}
