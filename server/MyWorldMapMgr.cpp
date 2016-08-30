#include "StdAfx.h"
#include ".\myworldmapmgr.h"

MyWorldMapMgr::MyWorldMapMgr(void)
{
}

MyWorldMapMgr::~MyWorldMapMgr(void)
{
}

//读取世界地图的所有传送点
void MyWorldMapMgr::ReadWorldMapTeleport()
{
	//场景配置文件
	char sceneConfigFile[255]="./scene.ini";

	char inBuf[255];     //用于保存读取的字符串


	//存储场景数量
	int scenenum=0;
	//读取场景数量
	bool ret=IniGetString(sceneConfigFile,"SceneConfig","Scenenum",inBuf);

	if(ret){
		scenenum=atoi(inBuf);
		printf("===读取世界地图的所有传送点===\n===有%d个场景===\n",scenenum);
	}else{
		MessageBox(0,"场景配置文件读取失败! [Config]-Scenenum","",0);
		return;
	}


	//循环出所有地图传送点
	for(int i=1;i<=scenenum;i++){
		char temp[255]="";
		vector3df p,StartPos; 

		STeleport* tel=new STeleport;

		//场景编号从1自增
		tel->worldMap_id=i;

		sprintf(temp,"scene%d",i);

		//加载角色所在世界地图的传送点

		ret=IniGetString(sceneConfigFile,temp,"teleport1_POS",inBuf);
		if(ret){
			sscanf(inBuf, "%f,%f,%f", 
				&tel->teleport_POS.X,&tel->teleport_POS.Y,&tel->teleport_POS.Z
				); 
		}

		ret=IniGetString(sceneConfigFile,temp,"teleport1_StartPos",inBuf);
		if(ret){
			sscanf(inBuf, "%f,%f,%f", 
				&tel->teleport_StartPos.X,&tel->teleport_StartPos.Y,&tel->teleport_StartPos.Z
				); 
		}

		//得到切换后的地图id
		int NewWorld=0;
		ret=IniGetString(sceneConfigFile,temp,"teleport1_NextScene",inBuf);
		if(ret){
			tel->NextworldMap_id=atoi(inBuf);
		}

		worldtelport_list.push_back(tel);

	}



}
