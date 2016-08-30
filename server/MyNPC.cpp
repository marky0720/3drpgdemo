#include "StdAfx.h"
#include ".\mynpc.h"


//初始化NPC
MyNPC::MyNPC(NPCProperty* npcporp,Socket::MySOCKServer* netser)
{
	this->netser=netser;

	shoppack=0;

	memcpy((char*)&npc_prop,(char*)npcporp,sizeof(RoleProperty));

	//表示该npc有客户操作交互
	if(npc_prop.cmd!=-1){

       ShopPackage* sh=new ShopPackage;
		shoppack=sh;
		shoppack->NPC_ID=npc_prop.ID;
		shoppack->worldMap_id=npc_prop.worldMap_id;


	   //npc商店包裹清零
	   for(int i=0;i<4;i++){
		   for(int j=0;j<6;j++){
			   sh->itembox[i][j]=0; //空物品
			   sh->itemParam1[i][j]=0; //物品属性1清零
			   sh->itemParam2[i][j]=0; //物品属性2清零
			   sh->itemParam3[i][j]=0; //物品属性3清零
			   sh->itemParam4[i][j]=0; //物品属性4清零
			   sh->itemParam5[i][j]=0; //物品属性5清零
			   sh->itemParam6[i][j]=0; //物品属性6清零
			   sh->itemPrice[i][j]=0;		   //价格
		   }
	   }

	   //从配置文件中读取商店物品到shoppack
	   ReadNPCShopConfig();


	}
}

MyNPC::~MyNPC(void)
{
}


void MyNPC::ReadNPCShopConfig()
{

		char File[255]="data//shop_npc_%d_%d.txt"; //npc商店文件 先世界编号 后npc id
		sprintf(File,"data//shop_npc_%d_%d.txt",npc_prop.worldMap_id, npc_prop.ID);
		FILE* log;
		log = fopen(File, "r");
		//文件不存在.返回
		if (log == NULL)
		{
			printf("文件%s不存在.返回\n",File);
			return ;
		}	
		char strtemp[255]="";


		int rownum=GetTxtRowNum(File);
		for(int i=1;i<=rownum;i++){
			//每行读取
			GetTxtByRowNum(File,strtemp,i);
			if(strtemp[0]=='/' && strtemp[1]=='/'){
				//printf("%d行是注释\n",i-1);
				continue; //该行是注释.继续下一个
			}else{

				//即第一个是数字.表示是个物品信息
				if(strtemp[0]>=48 && strtemp[0]<=57 ){
					SItem item ;

					
					//从一行里读取格式化的商店物品数据
					sscanf(strtemp, "%d  %d  %d  %d  %d  %d  %d", 
						&item.item_index,
						&item.itemParam1,&item.itemParam2,
						&item.itemParam3,&item.itemParam4,
						&item.itemParam5,&item.itemParam6
						
						); 

					//物品价格不在商店里定义.在全局物品txt里
					item.itemPrice=getItembyID(item.item_index)->itemPrice;

					printf("npc%d map:%d有商品%d  %d  %d  %d  %d  %d  %d  %d\n",npc_prop.ID , npc_prop.worldMap_id,item.item_index,
						item.itemParam1,item.itemParam2,
						item.itemParam3,item.itemParam4,
						item.itemParam5,item.itemParam6,item.itemPrice);

					//1000以内是武器
					if(item.item_index<1000){
						//npc商店里的武器攻击力属性不在商店里定义.而是item.txt里统一武器攻击力 
						item.itemParam2=getItemAttackById(item.item_index);
						if(item.itemParam1>0) item.itemParam2*=1.8; //param1是光武器效果.攻击力加倍
					}else if(item.item_index>=1000 && item.item_index<2000){
						//消耗品
						//恢复值属性在统一文件里定义
						if(item.item_index==1000)  //超级金创药 在param4
						    item.itemParam1=getItembyID(item.item_index)->itemParam4;
						if(item.item_index==1001)  //超级魔法药 在param5
							item.itemParam1=getItembyID(item.item_index)->itemParam5;
						  
					}

					
					PushItemInShopPack(item);


				

				
				}
			}
		}


}


//向商店空格装入一个物品
void MyNPC::PushItemInShopPack(SItem item )
{

	//找一个空格装入商品
	for(int i=0;i<4;i++){
		for(int j=0;j<6;j++){
			if(shoppack->itembox[i][j]==0)  //空物品
			{
					shoppack->itembox[i][j]=item.item_index;
					shoppack->itemParam1[i][j]=item.itemParam1;
					shoppack->itemParam2[i][j]=item.itemParam2;
					shoppack->itemParam3[i][j]=item.itemParam3;
					shoppack->itemParam4[i][j]=item.itemParam4;
					shoppack->itemParam5[i][j]=item.itemParam5;
					shoppack->itemParam6[i][j]=item.itemParam6;
					shoppack->itemPrice[i][j]=item.itemPrice;
					 
				return;
			}
		}
	}

}