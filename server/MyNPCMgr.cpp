#include "StdAfx.h"
#include ".\mynpcmgr.h"

MyNPCMgr::MyNPCMgr(Socket::MySOCKServer* s)
{
	server=s;

	//������������npc��Ϣ
	LoadAllWorldNPC();
}

MyNPCMgr::~MyNPCMgr(void)
{
}

//���npc��list
void MyNPCMgr::AddNPC(NPCProperty npcprop)
{
	MyNPC* n=new MyNPC(&npcprop,server);
	npc_list.push_back(n);

}


void MyNPCMgr::SendNPCShopPackage(SOCKET sock,int npcID,int worldMap)
{
	std::vector<MyNPC*>::iterator  list_oper;
	for(list_oper=npc_list.begin();list_oper!=npc_list.end();++list_oper)
	{
		MyNPC* p=(*list_oper);
		if(p->npc_prop.ID==npcID && p->npc_prop.worldMap_id==worldMap) 
		{
			//�ж�npc�Ƿ����̵�,��������н���.
			if(p->npc_prop.cmd==-1 || p->shoppack==0) 
			{
				break;
			}

			//���͸��û�.npc���̵���Ϣ
			p->shoppack->cmd=CMD_S2C_NPC_Shop;
			server->SendMsg(sock,(char*)p->shoppack,sizeof(ShopPackage));

			printf("�ҵ���Ӧnpc,�����̵���Ϣ\n");

			break;
		}
	}

}


//�õ�npc ͨ��id������������
MyNPC* MyNPCMgr::getNPCbyID(int id,int worldMap)
{
	MyNPC* rp=0;
	
	std::vector<MyNPC*>::iterator  list_oper;
	for(list_oper=npc_list.begin();list_oper!=npc_list.end();++list_oper)
	{
		MyNPC* p=(*list_oper);
		if(p->npc_prop.ID==id && p->npc_prop.worldMap_id==worldMap) 
		{
			rp=p;
			break;
		}
	}


	return rp;

}


//������������npc��Ϣ
void MyNPCMgr::LoadAllWorldNPC()
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
		printf("��%d������\n",scenenum);
	}else{
		MessageBox(0,"���������ļ���ȡʧ��! [Config]-Scenenum","",0);
		return;
	}

	//ѭ�������е�ͼnpc
	for(int i=1;i<=scenenum;i++){
		char temp[255]="";
		sprintf(temp,"scene%d",i);//���쳡���ַ���
		
		//�洢��i��������npc����
		int sceneNPCNum=0;
		sceneNPCNum=IniGetInt(sceneConfigFile,temp,"npc_num");
		if(sceneNPCNum==-1){
			printf("����%d ��npc\n",i);
			continue;//˵���ó���û��npc
		}

		printf("����%d��%d��npc\n",i,sceneNPCNum);

		//ѭ���˵�ͼ����npc
		for(int j=1;j<=sceneNPCNum;j++){
			NPCProperty p;
			p.cmd=0;
			p.ID=j;			//id����.1��ʼ
			p.worldMap_id=i; //��npc�����������ڵ������ͼid

			char str[255];
			sprintf(str,"npc%d_POS",j); //��ȡ��j��npc������
			if( IniGetString(sceneConfigFile,temp,str,inBuf) ){

				sscanf(inBuf, "%f,%f,%f",   &p.pos.X,&p.pos.Y,&p.pos.Z  ); 

			}

			sprintf(str,"npc%d_name",j); //��ȡ��j��npc��name
			if( IniGetString(sceneConfigFile,temp,str,inBuf) ){

				strcpy(p.strNPCName,inBuf);
			}

			sprintf(str,"npc%d_Rot",j); //��ȡ��j��npc����ת
			if( IniGetString(sceneConfigFile,temp,str,inBuf) ){

				sscanf(inBuf, "%f,%f,%f",   &p.rot.X,&p.rot.Y,&p.rot.Z); 

			}

			sprintf(str,"npc%d_talk",j); //��ȡ��j��npc�Ķ����˵�Ļ�
			if( IniGetString(sceneConfigFile,temp,str,p.strNpcTalk) ){
				
			}

			sprintf(str,"npc%d_btnText",j); //��ȡ��j��npc�ĶԻ���ť������
			if( IniGetString(sceneConfigFile,temp,str,p.strBtnText) ){
				//MessageBox(0,p.strBtnText,"",0);
			}else{
				//��ȡʧ��,˵����npcû�н�һ�����¼�.
				strcpy(p.strBtnText,"");
			}



			sprintf(str,"npc%d_btnClickcmd",j); //��ȡ��j��npc�İ�ť��Ӧ���¼�����
			p.cmd=IniGetInt(sceneConfigFile,temp,str);

			printf("worldMap:%d id:%d name:%s talk:%s\n",p.worldMap_id,p.ID,p.strNPCName,p.strNpcTalk);

			AddNPC(p);
		}


	}


}