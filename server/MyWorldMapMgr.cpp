#include "StdAfx.h"
#include ".\myworldmapmgr.h"

MyWorldMapMgr::MyWorldMapMgr(void)
{
}

MyWorldMapMgr::~MyWorldMapMgr(void)
{
}

//��ȡ�����ͼ�����д��͵�
void MyWorldMapMgr::ReadWorldMapTeleport()
{
	//���������ļ�
	char sceneConfigFile[255]="./scene.ini";

	char inBuf[255];     //���ڱ����ȡ���ַ���


	//�洢��������
	int scenenum=0;
	//��ȡ��������
	bool ret=IniGetString(sceneConfigFile,"SceneConfig","Scenenum",inBuf);

	if(ret){
		scenenum=atoi(inBuf);
		printf("===��ȡ�����ͼ�����д��͵�===\n===��%d������===\n",scenenum);
	}else{
		MessageBox(0,"���������ļ���ȡʧ��! [Config]-Scenenum","",0);
		return;
	}


	//ѭ�������е�ͼ���͵�
	for(int i=1;i<=scenenum;i++){
		char temp[255]="";
		vector3df p,StartPos; 

		STeleport* tel=new STeleport;

		//������Ŵ�1����
		tel->worldMap_id=i;

		sprintf(temp,"scene%d",i);

		//���ؽ�ɫ���������ͼ�Ĵ��͵�

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

		//�õ��л���ĵ�ͼid
		int NewWorld=0;
		ret=IniGetString(sceneConfigFile,temp,"teleport1_NextScene",inBuf);
		if(ret){
			tel->NextworldMap_id=atoi(inBuf);
		}

		worldtelport_list.push_back(tel);

	}



}
