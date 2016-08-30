#include "StdAfx.h"
#include ".\mynpcmgr.h"

MyNPCMgr::MyNPCMgr(Socket::MySOCKServer* s)
{
	server=s;

	//加载世界所有npc信息
	LoadAllWorldNPC();
}

MyNPCMgr::~MyNPCMgr(void)
{
}

//添加npc到list
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
			//判断npc是否有商店,即和玩家有交互.
			if(p->npc_prop.cmd==-1 || p->shoppack==0) 
			{
				break;
			}

			//发送给用户.npc的商店信息
			p->shoppack->cmd=CMD_S2C_NPC_Shop;
			server->SendMsg(sock,(char*)p->shoppack,sizeof(ShopPackage));

			printf("找到对应npc,发送商店信息\n");

			break;
		}
	}

}


//得到npc 通过id和所属世界编号
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


//加载世界所有npc信息
void MyNPCMgr::LoadAllWorldNPC()
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
		printf("有%d个场景\n",scenenum);
	}else{
		MessageBox(0,"场景配置文件读取失败! [Config]-Scenenum","",0);
		return;
	}

	//循环出所有地图npc
	for(int i=1;i<=scenenum;i++){
		char temp[255]="";
		sprintf(temp,"scene%d",i);//构造场景字符串
		
		//存储第i个场景的npc总数
		int sceneNPCNum=0;
		sceneNPCNum=IniGetInt(sceneConfigFile,temp,"npc_num");
		if(sceneNPCNum==-1){
			printf("场景%d 无npc\n",i);
			continue;//说明该场景没有npc
		}

		printf("场景%d有%d个npc\n",i,sceneNPCNum);

		//循环此地图所有npc
		for(int j=1;j<=sceneNPCNum;j++){
			NPCProperty p;
			p.cmd=0;
			p.ID=j;			//id自增.1开始
			p.worldMap_id=i; //给npc设置他所属于的世界地图id

			char str[255];
			sprintf(str,"npc%d_POS",j); //读取第j个npc的坐标
			if( IniGetString(sceneConfigFile,temp,str,inBuf) ){

				sscanf(inBuf, "%f,%f,%f",   &p.pos.X,&p.pos.Y,&p.pos.Z  ); 

			}

			sprintf(str,"npc%d_name",j); //读取第j个npc的name
			if( IniGetString(sceneConfigFile,temp,str,inBuf) ){

				strcpy(p.strNPCName,inBuf);
			}

			sprintf(str,"npc%d_Rot",j); //读取第j个npc的旋转
			if( IniGetString(sceneConfigFile,temp,str,inBuf) ){

				sscanf(inBuf, "%f,%f,%f",   &p.rot.X,&p.rot.Y,&p.rot.Z); 

			}

			sprintf(str,"npc%d_talk",j); //读取第j个npc的对玩家说的话
			if( IniGetString(sceneConfigFile,temp,str,p.strNpcTalk) ){
				
			}

			sprintf(str,"npc%d_btnText",j); //读取第j个npc的对话框按钮的文字
			if( IniGetString(sceneConfigFile,temp,str,p.strBtnText) ){
				//MessageBox(0,p.strBtnText,"",0);
			}else{
				//读取失败,说明该npc没有进一步的事件.
				strcpy(p.strBtnText,"");
			}



			sprintf(str,"npc%d_btnClickcmd",j); //读取第j个npc的按钮对应的事件命令
			p.cmd=IniGetInt(sceneConfigFile,temp,str);

			printf("worldMap:%d id:%d name:%s talk:%s\n",p.worldMap_id,p.ID,p.strNPCName,p.strNpcTalk);

			AddNPC(p);
		}


	}


}